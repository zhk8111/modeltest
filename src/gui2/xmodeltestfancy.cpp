#include "gui2/xmodeltestfancy.h"
#include "ui_xmodeltestfancy.h"
#include "gui/xutils.h"

#include "service/modeltestservice.h"
#include "gui/progressdialog.h"
#include "meta.h"

#ifdef QT_WIDGETS_LIB
#include <QtWidgets>
#else
#include <QtGui/QFileDialog>
#include <QStandardItemModel>
#include <QTableView>
#include <QtGui/QMessageBox>
#include <QtConcurrentRun>
#endif

#define TREE_MP     0
#define TREE_ML     1
#define TREE_ML_JC  2
#define TREE_ML_GTR 3
#define TREE_USER   4
#define TREE_DEFAULT TREE_MP

using namespace std;
using namespace modeltest;

static unsigned int model_index;

XModelTestFancy::XModelTestFancy(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::XModelTestFancy)
{
  qRegisterMetaType<partition_id_t>();
  qRegisterMetaType<mt_size_t>();

  ui->setupUi(this);

  ui->slider_nthreads->setRange(1, QThread::idealThreadCount());
  ui->slider_nthreads->setValue(modeltest::Utils::count_physical_cores());

  scheme = 0;
  status = st_active;

  /* Redirect Console output to QTextEdit */
  redirect = new MyDebugStream(std::cout);
  QObject::connect(redirect, SIGNAL(newText(char *)), this, SLOT(set_text(char *)), Qt::QueuedConnection);

#ifdef QT_WIDGETS_LIB
    addAction(ui->actionLoad_MSA);
    addAction(ui->actionLoad_Partitions);
    addAction(ui->actionLoad_Tree);
    addAction(ui->actionConsole);
    addAction(ui->actionData);
    addAction(ui->actionResults);
    addAction(ui->actionSettings);
    addAction(ui->action_quit);
#endif

  update_gui();

  set_active_tab("Console");
  Meta::print_ascii_logo();
  Meta::print_header();
}

XModelTestFancy::~XModelTestFancy()
{
  delete ui;
}

typedef enum { LABEL_FAIL, LABEL_UNDEF, LABEL_OK } oklabel_t;

static void set_oklabel(QLabel * label, oklabel_t ok)
{
  switch(ok)
  {
  case LABEL_FAIL:
    label->setText(QString("NO"));
    label->setStyleSheet("color: #aa0000;");
    break;
  case LABEL_UNDEF:
    label->setText(QString("-"));
    label->setStyleSheet("");
    break;
  case LABEL_OK:
    label->setText(QString("OK"));
    label->setStyleSheet("color: #00aa00;");
    break;
  }
}

void XModelTestFancy::update_gui()
{
  /* get values */
  tree_type = ui->cmb_tree->currentIndex();
  n_threads = ui->slider_nthreads->value();

  /* apply changes */
  ui->lbl_nthreads->setText(QString::number(n_threads));
  set_oklabel(ui->lbl_okmsa, (status & st_msa_loaded)?LABEL_OK:LABEL_FAIL);
  if (status & st_tree_loaded)
    set_oklabel(ui->lbl_oktree, LABEL_OK);
  else
  {
    if (tree_type == TREE_USER)
      set_oklabel(ui->lbl_oktree, LABEL_FAIL);
    else
      set_oklabel(ui->lbl_oktree, LABEL_UNDEF);
  }
  set_oklabel(ui->lbl_okparts, (status & st_parts_loaded)?LABEL_OK:LABEL_UNDEF);

  int n_cats = ui->sliderNCat->value();
  if (!(ui->cbGModels->isChecked() || ui->cbIGModels->isChecked()))
      n_cats = 1;
  compute_size(n_cats, ui->slider_nthreads->value());

  /* enabling */
  ui->sliderNCat->setEnabled(ui->cbGModels->isChecked() || ui->cbIGModels->isChecked());
  ui->btn_loadtree->setEnabled(status & st_msa_loaded);
  ui->actionLoad_Tree->setEnabled(status & st_msa_loaded);
  ui->cmb_tree->setEnabled(status & st_msa_loaded);
  ui->btn_loadparts->setEnabled(status & st_msa_loaded);
  ui->actionLoad_Partitions->setEnabled(status & st_msa_loaded);
  ui->btn_run->setEnabled(status & st_msa_loaded);
  ui->btn_report->setEnabled(status & st_optimized);
  ui->tabResults->setEnabled(status & st_optimized);

  ui->frame_settings->setEnabled(!(status & (st_optimizing | st_optimized)));
}

static void fill_results(QTableView * result_table,
                         modeltest::ModelSelection &model_selection,
                         QLabel *imp_inv, QLabel *imp_gamma, QLabel *imp_gammainv, QLabel *imp_freqs)
{
    QStandardItemModel * results_table_items;
    double cum_weight = 0.0;

    if (result_table->model() != NULL)
    {
        delete result_table->model();
    }
    results_table_items = new QStandardItemModel(0,7);

    int cur_column = 0;
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("Model")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("K")));

    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("lnL")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("score")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("delta")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("weight")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("cum weight")));
    result_table->setModel(results_table_items);

    for (size_t i=0; i<model_selection.size(); i++)
    {
        modeltest::Model * model = model_selection.get_model(i).model;
        results_table_items->setItem(i, 0, new QStandardItem(QString(model->get_name().c_str())));
        results_table_items->setItem(i, 1, new QStandardItem(QString::number(model->get_n_free_variables())));

        results_table_items->setItem(i, 2, new QStandardItem(QString::number(model->get_loglh(), 'f', 4)));
        results_table_items->setItem(i, 3, new QStandardItem(QString::number(model_selection.get_model(i).score, 'f', 4)));
        results_table_items->setItem(i, 4, new QStandardItem(QString::number(model_selection.get_model(i).delta, 'f', 4)));
        results_table_items->setItem(i, 5, new QStandardItem(QString::number(model_selection.get_model(i).weight, 'f', 4)));
        cum_weight += model_selection.get_model(i).weight;
        results_table_items->setItem(i, 6, new QStandardItem(QString::number(cum_weight, 'f', 4)));
    }

    if (imp_gamma)
        imp_gamma->setText(QString::number(model_selection.get_importance_gamma()));
    if (imp_inv)
        imp_inv->setText(QString::number(model_selection.get_importance_inv()));
    if (imp_gammainv)
        imp_gammainv->setText(QString::number(model_selection.get_importance_gamma_inv()));
    if (imp_freqs)
        imp_freqs->setText(QString::number(model_selection.get_importance_freqs()));
}

void XModelTestFancy::on_slider_nthreads_valueChanged(int value)
{
    ui->lbl_nthreads->setText(QString::number(value));
    update_gui();
}

void XModelTestFancy::on_cmb_tree_currentIndexChanged(int index)
{
    if (index == TREE_USER && !(status & st_tree_loaded))
        on_btn_loadtree_clicked();
    update_gui();
}

void XModelTestFancy::on_actionConsole_triggered()
{
    set_active_tab("Console");
}

void XModelTestFancy::on_actionData_triggered()
{
    set_active_tab("Data");
}

void XModelTestFancy::on_actionSettings_triggered()
{
    set_active_tab("Settings");
}

void XModelTestFancy::on_actionResults_triggered()
{
    set_active_tab("Results");
}

void XModelTestFancy::on_btn_loadmsa_clicked()
{
    action_open_msa();
}

void XModelTestFancy::set_active_tab(QString text)
{
    int i=0;
    for (i=0; i<ui->tab_view->count(); ++i)
        if (ui->tab_view->tabText(i) == text)
        {
            ui->tab_view->setCurrentIndex(i);
            break;
        }
     assert(i < ui->tab_view->count());
}

void XModelTestFancy::action_open_msa()
{
    if (!ui->btn_loadmsa->isEnabled())
        return;

    QString filters = tr("Multiple Sequence Alignment(*.phy *.nex *.fas);; All files(*)");
    QString file_name = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"),
                                                    "",
                                                    filters);

    const std::string loaded_file = file_name.toStdString();
    bool load_file = true;

    if ( loaded_file.compare(""))
    {
        if (msa_filename.compare(""))
        {
            load_file = false;
            if (loaded_file.compare(msa_filename))
            {
                QMessageBox msgBox;
                msgBox.setText("Setting a different MSA will reset modeltest");
                msgBox.setInformativeText("Are you sure?");
                msgBox.setStandardButtons(QMessageBox::Reset | QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Cancel);
                int ret = msgBox.exec();
                if (ret == QMessageBox::Reset)
                {
                    reset_xmt();
                    load_file = true;
                }
            }
        }

        if (load_file)
        {
            msa_filename = loaded_file;
            data_type_t test_dt;
            if (modeltest::ModelTest::test_msa(msa_filename,
                                     &n_taxa,
                                     &n_sites,
                                     &n_patterns,
                                     &msa_format,
                                     &test_dt))
            {
                cout << endl << "Loaded alignment" << endl;

                if (test_dt == dt_dna)
                {
                    ui->radDatatypeDna->setChecked(true);
                    on_radDatatypeDna_clicked();
                }
                else if (test_dt == dt_protein)
                {
                    ui->radDatatypeProt->setChecked(true);
                    on_radDatatypeProt_clicked();
                }
                ui->consoleRun->append(xutils::to_qstring("%1", msg_lvl_notify).arg(msa_filename.c_str()));
                cout << "Format:          ";
                switch(msa_format)
                {
                case mf_fasta:
                    ui->lbl_format->setText("FASTA");
                    cout << "FASTA" << endl;
                    break;
                case mf_phylip:
                    ui->lbl_format->setText("PHYLIP");
                    cout << "PHYLIP" << endl;
                    break;
                case mf_undefined:
                    ui->lbl_format->setText("Undefined");
                    cout << "Undefined" << endl;
                    break;
                }
                ui->lbl_datatype->setText(test_dt == dt_protein?"Protein":"DNA");
                cout << "Datatype:        " << (test_dt == dt_protein?"Protein":"DNA") << endl;
                ui->lbl_ntaxa->setText(QString::number(n_taxa));
                cout << "Num.Sequences:   " << n_taxa << endl;
                ui->lbl_nsites->setText(QString::number(n_sites));
                cout << "Sequence Length: " << n_sites << endl;
                ui->lbl_npatterns->setText(QString::number(n_patterns));
                cout << "Site patterns: " << n_patterns << endl;
                status |= st_msa_loaded;
            }
            else
            {
                ui->consoleRun->append(xutils::to_qstring("Error: Cannot load alignment %1", msg_lvl_error).arg(msa_filename.c_str()));
                msa_filename   = "";
                status &= ~st_msa_loaded;
            }
        }
    }

    if (status & st_msa_loaded)
    {
        //ui->lbl_msa_fname->setText(QString(modeltest::Utils::getBaseName(msa_filename).c_str()));
        ui->lbl_msa_fname->setText(QString(msa_filename.c_str()));
        set_active_tab("Data");
    }
    else
    {
        ui->lbl_msa_fname->setText("none");
        ui->lbl_tree_fname->setText("none");
        ui->lbl_parts_fname->setText("none");
        set_active_tab("Console");
    }

    update_gui();
}

size_t XModelTestFancy::compute_size(int n_cats, int n_threads)
{
    size_t mem_b = 0;
    int states = ui->radDatatypeDna->isChecked()?
                N_DNA_STATES :
                N_PROT_STATES;

    if ((status & st_msa_loaded) && n_taxa && n_sites && n_cats && states)
    {
        mem_b = modeltest::Utils::mem_size(n_taxa,
                        n_patterns,
                        n_cats,
                        states);
        mem_b *= n_threads;

        /* overestimating factor */
        mem_b *= 1.2;
        mem_b /= (1024*1024);

        ui->lbl_memsize->setText(QString("%1 MB").arg(
                    QString::number(
                        mem_b)));

        if (mem_b > 1000)
            ui->lbl_memsize->setStyleSheet("color: #f00;");
        else
            ui->lbl_memsize->setStyleSheet("");
    }
    else
    {
        ui->lbl_memsize->setText("-");
    }
    return (mem_b);
}

void XModelTestFancy::on_actionLoad_MSA_triggered()
{
    on_btn_loadmsa_clicked();
}

void XModelTestFancy::on_btn_loadtree_clicked()
{
    if (!ui->btn_loadtree->isEnabled())
        return;

    QString filters = tr("Newick tree(*.tree *.newick);; All files(*)");
    QString file_name;
    if (status & st_optimized)
    {
            QMessageBox msgBox;
            msgBox.setText("Tree cannot be set after optimization");
            msgBox.setInformativeText("You must reset ModelTest before");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
            return;
    }

    file_name = QFileDialog::getOpenFileName(this,
                                             tr("Open File"),
                                             "",
                                             filters);
    const std::string loaded_file = file_name.toStdString();

    if ( loaded_file.compare(""))
    {
        mt_size_t n_tips;

        utree_filename = loaded_file;
        if (modeltest::ModelTest::test_tree(utree_filename,
                                            &n_tips))
        {
            if (n_tips == n_taxa)
            {
                cout << endl << "Loaded tree" << endl;
                ui->consoleRun->append(xutils::to_qstring("%1", msg_lvl_notify).arg(utree_filename.c_str()));
                status |= st_tree_loaded;
            }
            else
            {
                ui->consoleRun->append(xutils::to_qstring("Error: Tip number does not match in %1", msg_lvl_error).arg(utree_filename.c_str()));
                utree_filename = "";
                status &= ~st_tree_loaded;
            }
        }
        else
        {
            ui->consoleRun->append(xutils::to_qstring("%1", msg_lvl_error).arg(modeltest::mt_errmsg));
            utree_filename = "";
            status &= ~st_tree_loaded;
        }

        if (ui->cmb_tree->currentIndex() == TREE_USER)
        {
            if (!(status & st_tree_loaded))
                ui->cmb_tree->setCurrentIndex(TREE_DEFAULT);
        }
        else
        {
            if (status & st_tree_loaded)
                ui->cmb_tree->setCurrentIndex(TREE_USER);
        }
    }

    if (status & st_tree_loaded)
    {
        //ui->lbl_tree_fname->setText(QString(modeltest::Utils::getBaseName(utree_filename).c_str()));
        ui->lbl_tree_fname->setText(QString(utree_filename.c_str()));
        set_active_tab("Data");
    }
    else
    {
        ui->lbl_tree_fname->setText("none");
        ui->cmb_tree->setCurrentIndex(TREE_DEFAULT);
        set_active_tab("Console");
    }

    update_gui();
}

void XModelTestFancy::on_actionLoad_Tree_triggered()
{
    on_btn_loadtree_clicked();
}

void XModelTestFancy::reset_xmt( void )
{
    status = st_active;

    n_taxa  = 0;
    n_sites = 0;

    msa_filename   = "";
    utree_filename = "";
    parts_filename = "";
    asc_filename   = "";

    ui->lbl_msa_fname->setText("none");
    ui->lbl_tree_fname->setText("none");
    ui->lbl_parts_fname->setText("none");
    ui->lbl_datatype->setText("-");
    ui->lbl_format->setText("-");
    ui->lbl_ntaxa->setText("-");
    ui->lbl_nsites->setText("-");

    ui->consoleRun->clear();
    Meta::print_ascii_logo();
    Meta::print_header();

    if (scheme)
    {
        delete scheme;
        scheme = 0;
    }

    ModelTestService::instance()->destroy_instance();
}

void XModelTestFancy::on_btn_run_clicked()
{
    if (!ui->btn_run->isEnabled())
        return;

    set_active_tab("Console");

    status &= ~st_optimized;
    status |= st_optimizing;

    update_gui();
    //ini_t = time(NULL);

    run_modelselection();
    status &= ~st_optimizing;

    update_gui();
}

void XModelTestFancy::on_btn_reset_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("This action will reset modeltest");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Reset | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Reset)
    {
        reset_xmt();
    }
    update_gui();
}

bool XModelTestFancy::run_modelselection()
{
    partition_id_t part_id;
    int number_of_threads  = ui->slider_nthreads->value();

    QMessageBox msgBox;
    msgBox.setText("Start models optimizaion");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return false;

    model_index = 0;

    switch (ui->cmb_tree->currentIndex())
    {
      case TREE_MP:
        start_tree = tree_mp;
        break;
      case TREE_ML_JC:
        start_tree = tree_ml_jc_fixed;
        break;
      case TREE_ML_GTR:
        start_tree = tree_ml_gtr_fixed;\
        break;
      case TREE_ML:
        start_tree = tree_ml;
        break;
      case TREE_USER:
        start_tree = tree_user_fixed;
        break;
    }

    if (ui->radAscbiasNone->isChecked())
        asc_bias = asc_none;
    else if (ui->radAscbiasLewis->isChecked())
        asc_bias = asc_lewis;
    else if (ui->radAscbiasFelsenstein->isChecked())
        asc_bias = asc_felsenstein;
    else if (ui->radAscbiasStamatakis->isChecked())
        asc_bias = asc_stamatakis;
    else
        assert(0);

    /* validate */
    if (ui->radAscbiasFelsenstein->isChecked() || ui->radAscbiasStamatakis->isChecked())
    {
        bool ascbias_file_ok = false;

        /* TODO: validate asc bias file */

        if (!ascbias_file_ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Invalid composition file for ascertainment bias correction");
            msgBox.setInformativeText("Set a good one or select None or Lewis");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();

            return false;
        }
    }

    int model_params = 0;
    if (ui->cbEqualFreq->isChecked())
        model_params += MOD_PARAM_FIXED_FREQ;
    if (ui->cbNoRateVarModels->isChecked())
        model_params += MOD_PARAM_NO_RATE_VAR;
    if (ui->cbIModels->isChecked())
        model_params += MOD_PARAM_INV;
    if (ui->cbGModels->isChecked())
        model_params += MOD_PARAM_GAMMA;
    if (ui->cbIGModels->isChecked())
        model_params += MOD_PARAM_INV_GAMMA;
    if (ui->cbMlFreq->isChecked())
        model_params += MOD_PARAM_ESTIMATED_FREQ;

    std::vector<mt_index_t> matrices;
    if (ui->radDatatypeProt->isChecked())
    {
        for (int i=0; i < ui->modelsListView->count(); i++)
        {
            if (ui->modelsListView->item(i)->checkState() == Qt::CheckState::Checked)
                matrices.push_back(i);
        }
    }
    else
    {
        if (!ui->radSchemes203->isChecked())
        {
            for (int i=0; i < ui->modelsListView->count(); i++)
            {
                if (ui->modelsListView->item(i)->checkState() == Qt::CheckState::Checked)
                    matrices.push_back(dna_model_matrices_indices[i]);
            }
        }
        else
        {
            for (int i=0; i<N_DNA_ALLMATRIX_COUNT; i++)
            {
                matrices.push_back(i);
            }
        }
    }

    opts.write_checkpoint = false;
    opts.msa_format = msa_format;
    opts.asc_bias_corr = asc_bias;
    opts.n_taxa = n_taxa;
    opts.n_sites = n_sites;
    opts.compress_patterns = true;
    opts.rnd_seed = 12345;
    opts.model_params = model_params;
    opts.n_catg = ui->sliderNCat->value();
    opts.msa_filename = msa_filename;
    opts.tree_filename = utree_filename;
    opts.partitions_filename = "";
    if (ui->radDatatypeDna->isChecked())
        opts.nt_candidate_models = matrices;
    else
        opts.aa_candidate_models = matrices;
    opts.starting_tree = start_tree;
    opts.output_tree_to_file = false;
    opts.partitions_desc = NULL;
    opts.partitions_eff = NULL;
    opts.n_threads = number_of_threads;
    opts.epsilon_param = ui->txtParEpsilon->text().toDouble();
    opts.epsilon_opt = ui->txtOptEpsilon->text().toDouble();
    opts.verbose = VERBOSITY_LOW;

    data_type_t datatype = ui->radDatatypeDna->isChecked()?dt_dna:dt_protein;

    if (!scheme)
    {
        /* create single partition / single region */
        scheme = new partitioning_scheme_t();
        partition_region_t region;
        partition_descriptor_t partition;
        region.start = 1;
        region.end = n_sites;
        region.stride = 1;
        partition.gap_aware = false;
        partition.datatype = datatype;
        partition.states = datatype==dt_dna?N_DNA_STATES:N_PROT_STATES;
        partition.partition_name = "DATA";
        partition.regions.push_back(region);
        partition.unique_id = 1;
        scheme->push_back(partition);
    }

    for (partition_descriptor_t &partition : *scheme)
    {
      partition.starting_tree = start_tree;
    }

    opts.partitions_desc = scheme;
    //TODO: Create option for smoothing
    opts.smooth_freqs = false;

    ui->consoleRun->append(xutils::to_qstring("Building modeltest instance for %1 threads", msg_lvl_info).arg(number_of_threads));

    bool ok_inst = ModelTestService::instance()->reset_instance(opts);
    if (!ok_inst)
    {
        ui->consoleRun->append(xutils::to_qstring("Error building instance [%1]", msg_lvl_error).arg(modeltest::mt_errno));
        ui->consoleRun->append(xutils::to_qstring(modeltest::mt_errmsg, msg_lvl_error));
        return false;
    }

    /* print settings */
   Meta::print_options(opts);

   modeltest::PartitioningScheme & partitioning_scheme = ModelTestService::instance()->get_partitioning_scheme();

   //TODO: FIX
   mt_size_t n_models;
   if (partitioning_scheme.get_size() == 1)
   {
       part_id = {0};
        mythread = new xThreadOpt(part_id,
                                  number_of_threads,
                                  opts.epsilon_param,
                                  opts.epsilon_opt);
   }
   else
   {
       mythread = new xThreadOpt(partitioning_scheme,
                                 number_of_threads,
                                 opts.epsilon_param,
                                 opts.epsilon_opt);
   }
    n_models = mythread->get_number_of_models();

    QObject::connect(mythread,
                     SIGNAL(optimization_done()),
                     this,
                     SLOT(optimization_done()));
    QObject::connect(mythread,
                     SIGNAL(optimization_interrupted()),
                     this,
                     SLOT(optimization_interrupted()));
    QObject::connect(mythread,
                     SIGNAL(optimized_single_done(modeltest::Model *, unsigned int)),
                     this,
                     SLOT(optimized_single_model(modeltest::Model *, unsigned int)));
    QObject::connect(mythread,
                     SIGNAL(optimized_partition_done(partition_id_t)),
                     this,
                     SLOT(optimized_partition(partition_id_t)));

    ProgressDialog dialog( n_models,
                           number_of_threads );

    QObject::connect(mythread, SIGNAL(optimization_done( void )), &dialog, SLOT(reset( void )));
    QObject::connect(mythread,
                     SIGNAL(optimized_single_done(modeltest::Model *, unsigned int)),
                     &dialog,
                     SLOT(optimized_single_model( void )));
    QObject::connect(mythread, SIGNAL(next_model(modeltest::Model*,uint)), &dialog, SLOT(set_model_running(modeltest::Model*,uint)));
    QObject::connect(mythread, SIGNAL(next_parameter(uint,double,uint)), &dialog, SLOT(set_delta_running(uint,double,uint)));
    QObject::connect(&dialog, SIGNAL(canceled()), mythread, SLOT(kill_threads()));

    modeltest::on_run = true;

    ini_t = time(NULL);

    mythread->start();
    dialog.exec();

    return true;
}

/* SLOTS */

void XModelTestFancy::set_text(char * message)
{
    //TODO: Improve this allocation/deallocation
    ui->consoleRun->append(message);
    free(message);
    ui->consoleRun->show();
}

void XModelTestFancy::optimized_single_model(modeltest::Model * model, unsigned int n_models )
{
    /* print progress */
    cout << setw(5) << right << (++model_index) << "/"
         << setw(5) << left << n_models
         << setw(15) << left << model->get_name()
         << setw(18) << right << setprecision(MT_PRECISION_DIGITS)
         << fixed << model->get_loglh()
         << setw(18) << right << setprecision(MT_PRECISION_DIGITS)
         << fixed << model->get_bic() << endl;
}

void XModelTestFancy::optimized_partition( partition_id_t part_id )
{
    UNUSED(part_id);
    cout << "Finished partition " << endl;
}

void XModelTestFancy::optimization_done( )
{
    //TODO: FIX
//    const std::vector<modeltest::Model *> & modelsPtr = ModelTestService::instance()->get_modeltest()->get_models(part_id);
//        QVector<int> models;
//        for (int i=0; (size_t)i < modelsPtr.size(); i++)
//            models.append(i);

        status &= ~st_optimizing;
        status |= st_optimized;

        modeltest::on_run = false;

        update_gui();

        cout << setw(80) << setfill('-') << "" << setfill(' ') << endl;
        cout << "optimization done! It took " << time(NULL) - ini_t << " seconds" << endl;

        delete mythread;

        /* fill results */
        modeltest::PartitioningScheme & partitioning_scheme = ModelTestService::instance()->get_partitioning_scheme();

        ui->cmb_results_partition->setVisible(partitioning_scheme.get_size() > 1);
        model_selection.resize(partitioning_scheme.get_size());
        for (mt_index_t i=0; i<partitioning_scheme.get_size(); ++i)
        {
          //TODO: fix for multiple partition
          // if (scheme.get_size() > 1)
          //   ui->cmb_results_partition->addItem(scheme.get_partition(i).get_name().c_str());

          model_selection[i][modeltest::ic_aic]  =  ModelTestService::instance()->select_models(partitioning_scheme.get_partition(i).get_id(), modeltest::ic_aic);
          model_selection[i][modeltest::ic_aicc] =  ModelTestService::instance()->select_models(partitioning_scheme.get_partition(i).get_id(), modeltest::ic_aicc);
          model_selection[i][modeltest::ic_bic]  =  ModelTestService::instance()->select_models(partitioning_scheme.get_partition(i).get_id(), modeltest::ic_bic);

          //TODO: Allow this only for fixed trees
          if (false)
            model_selection[i][modeltest::ic_dt]   =  ModelTestService::instance()->select_models(partitioning_scheme.get_partition(i).get_id(), modeltest::ic_dt);
        }

        fill_results(ui->table_results_aic, *model_selection[0][modeltest::ic_aic],
                     ui->txt_imp_inv_aic, ui->txt_imp_gamma_aic,
                     ui->txt_imp_invgamma_aic, ui->txt_imp_freqs_aic);
        fill_results(ui->table_results_aicc, *model_selection[0][modeltest::ic_aicc],
                     ui->txt_imp_inv_aicc, ui->txt_imp_gamma_aicc,
                     ui->txt_imp_invgamma_aicc, ui->txt_imp_freqs_aicc);
        fill_results(ui->table_results_bic, *model_selection[0][modeltest::ic_bic],
                     ui->txt_imp_inv_bic, ui->txt_imp_gamma_bic,
                     ui->txt_imp_invgamma_bic, ui->txt_imp_freqs_bic);
        //TODO: Allow this only for fixed trees
        bool do_dt = false;
        if (do_dt)
          fill_results(ui->table_results_dt, *model_selection[0][modeltest::ic_dt],
                       ui->txt_imp_inv_dt, ui->txt_imp_gamma_dt,
                       ui->txt_imp_invgamma_dt, ui->txt_imp_freqs_dt);

        for (int c = 0; c < ui->table_results_bic->horizontalHeader()->count(); ++c)
        {
    #ifdef QT_WIDGETS_LIB
            ui->table_results_bic->horizontalHeader()->setSectionResizeMode(
                c, QHeaderView::Stretch);
            ui->table_results_aic->horizontalHeader()->setSectionResizeMode(
                c, QHeaderView::Stretch);
            ui->table_results_aicc->horizontalHeader()->setSectionResizeMode(
                c, QHeaderView::Stretch);
            if (do_dt)
              ui->table_results_dt->horizontalHeader()->setSectionResizeMode(
                c, QHeaderView::Stretch);
    #else
            ui->table_results_bic->horizontalHeader()->setResizeMode(
                c, QHeaderView::Stretch);
            ui->table_results_aic->horizontalHeader()->setResizeMode(
                c, QHeaderView::Stretch);
            ui->table_results_aicc->horizontalHeader()->setResizeMode(
                c, QHeaderView::Stretch);
            if (do_dt)
              ui->table_results_dt->horizontalHeader()->setResizeMode(
                c, QHeaderView::Stretch);
    #endif
        }
}

void XModelTestFancy::optimization_interrupted()
{
        status &= ~st_optimizing;
        status &= ~st_optimized;

        modeltest::on_run = false;

        update_gui();

        cout << setw(80) << setfill('-') << "" << setfill(' ') << endl;
        cout << "optimization interrupted after " << time(NULL) - ini_t << " seconds" << endl;

        ModelTestService::instance()->destroy_instance();
        delete mythread;
}

void XModelTestFancy::on_action_quit_triggered()
{
    QMessageBox msgBox;
    msgBox.setText("You will exit modeltest");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes)
    {
        close();
    }
}