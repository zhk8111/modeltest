#ifndef CONSOLE
#include "modeltest_gui.h"
#include <QApplication>
#else
#include "modeltest.h"
#endif

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <getopt.h>

using namespace std;

static bool parse_arguments(int argc, char *argv[], mt_options & exec_opt)
{
    bool input_file_ok = false;
    dna_subst_schemes dna_ss = ss_undef;
    mt_size_t n_sequences, n_sites;
    string user_candidate_models = "";

    /* defaults */
    dna_subst_schemes default_ss = ss_11;

    /* set default options */
    exec_opt.n_catg        = DEFAULT_GAMMA_RATE_CATS;
    exec_opt.epsilon_param = DEFAULT_PARAM_EPSILON;
    exec_opt.epsilon_opt   = DEFAULT_OPT_EPSILON;
    exec_opt.rnd_seed      = DEFAULT_RND_SEED;
    exec_opt.model_params  = 0;
    exec_opt.datatype      = dt_dna;
    exec_opt.subst_schemes = ss_undef;

    static struct option long_options[] =
    {
        { "categories", required_argument, 0, 'c' },
        { "datatype", required_argument, 0, 'd' },
        { "epsilon", required_argument, 0, 'e' },
        { "model-freqs", required_argument, 0, 'F' },
        { "help", no_argument, 0, 'h' },
        { "model-het", required_argument, 0, 'H' },
        { "input", required_argument, 0, 'i' },
        { "models", required_argument, 0, 'm' },
        { "tree", required_argument, 0, 't' },
        { "partitions", required_argument, 0, 'q' },
        { "rngseed", required_argument, 0, 'r' },
        { "schemes", required_argument, 0, 'S' },
        { "output", required_argument, 0, 'o' },
        { 0, 0, 0, 0 }
    };

    int opt = 0, long_index = 0;
    while ((opt = getopt_long(argc, argv, "c:d:e:F:hH:i:m:t:q:r:S:o:", long_options,
                              &long_index)) != -1) {
        switch (opt) {
        case 'c':
            exec_opt.n_catg = (mt_index_t) atoi(optarg);
            break;
        case 'd':
            if (!strcasecmp(optarg, "nt"))
            {
                exec_opt.datatype = dt_dna;
            }
            else if (!strcasecmp(optarg, "aa"))
            {
                exec_opt.datatype = dt_protein;
            }
            else
            {
                cerr << "ERROR: Invalid datatype " << optarg << endl;
                return false;
            }
            break;
        case 'e':
            exec_opt.epsilon_opt = atof(optarg);
            exec_opt.epsilon_param = atof(optarg);
            break;
        case 'F':
            for (mt_index_t i=0; i<strlen(optarg); i++)
            {
                switch(optarg[i])
                {
                case 'f':
                case 'F':
                    /* equal freqs (DNA) / empirical freqs (AA) */
                    exec_opt.model_params  |= MOD_PARAM_FIXED_FREQ;
                    break;
                case 'e':
                case 'E':
                    /* ML freqs (DNA) / model defined freqs (AA) */
                    exec_opt.model_params  |= MOD_PARAM_ESTIMATED_FREQ;
                    break;
                default:
                    cerr << "ERROR: Unrecognised rate heterogeneity parameter " << optarg[i] << endl;
                    return false;
                }
            }
            break;
        case 'h':
            cerr << "ModelTest Help:" << endl;
            return false;
        case 'H':
            for (mt_index_t i=0; i<strlen(optarg); i++)
            {
                switch(optarg[i])
                {
                case 'u':
                case 'U':
                    exec_opt.model_params  |= MOD_PARAM_NO_RATE_VAR;
                    break;
                case 'i':
                case 'I':
                    exec_opt.model_params  |= MOD_PARAM_INV;
                    break;
                case 'g':
                case 'G':
                    exec_opt.model_params  |= MOD_PARAM_GAMMA;
                    break;
                case 'f':
                case 'F':
                    exec_opt.model_params  |= MOD_PARAM_INV_GAMMA;
                    break;
                default:
                    cerr << "ERROR: Unrecognised rate heterogeneity parameter " << optarg[i] << endl;
                    return false;
                }
            }
            break;
        case 'i':
            exec_opt.msa_filename = optarg;
            input_file_ok = strcmp(optarg, "");
            break;
        case 'm':
        {
            user_candidate_models = optarg;
            break;
        }
        case 't':
            exec_opt.tree_filename = optarg;
            exec_opt.starting_tree = tree_user_fixed;
            break;
        case 'q':
            exec_opt.partitions_filename = optarg;
            //partitionsFileDefined = true;
            break;
        case 'o':
            exec_opt.output_filename = optarg;
            break;
        case 'r':
            exec_opt.rnd_seed = (unsigned int) atoi(optarg);
            break;
        case 'S':
            if (!strcmp(optarg, "3"))
            {
                dna_ss = ss_3;
            }
            else if (!strcmp(optarg, "5"))
            {
                dna_ss = ss_5;
            }
            else if (!strcmp(optarg, "7"))
            {
                dna_ss = ss_7;
            }
            else if (!strcmp(optarg, "11"))
            {
                dna_ss = ss_11;
            }
            else if (!strcmp(optarg, "203"))
            {
                dna_ss = ss_203;
            }
            else
            {
                cerr << "ERROR: Invalid number of substitution schemes " << optarg << endl;
                return false;
            }
            break;
        default:
            cerr << "Unrecognised argument -" << opt << endl;
            return false;
        }
    }

    srand(exec_opt.rnd_seed);

    /* validate input file */
    if (input_file_ok) {
        if (modeltest::MsaPll::test(exec_opt.msa_filename, &n_sequences, &n_sites))
        {
            cout << "Read OK: " << n_sequences << " x " << n_sites << endl;
        }
        else
        {
            cerr << "Error parsing alignment: " << exec_opt.msa_filename << endl;
            return false;
        }
    }
    else
    {
        cerr << "Alignment file (-i) is required" << endl;
        return false;
    }

    if (exec_opt.datatype == dt_protein)
    {
        if (dna_ss != ss_undef)
            cerr << "Warning: Substitution schemes will be ignored" << endl;

        if (user_candidate_models.compare(""))
        {
            int prot_matrices_bitv = 0;
            /* parse user defined models */
            istringstream f(user_candidate_models);
            string s;
            while (getline(f, s, ',')) {
                mt_index_t i, c_matrix = 0;
                for (i=0; i<N_PROT_MODEL_MATRICES; i++)
                {
                    if (!strcasecmp(prot_model_names[i].c_str(), s.c_str()))
                    {
                        c_matrix = i;
                        break;
                    }
                }
                if (i == N_PROT_MODEL_MATRICES)
                {
                    cerr << "Invalid protein matrix: " << s << endl;
                    return false;
                }
                prot_matrices_bitv |= 1<<c_matrix;
            }
            for (mt_index_t i=0; i<N_PROT_MODEL_MATRICES; i++)
            {
                if (prot_matrices_bitv&1)
                {
                    exec_opt.candidate_models.push_back(i);
                }
                prot_matrices_bitv >>= 1;
            }
        }
        else
        {
            /* the whole set is used */
            for (mt_index_t i=0; i<N_PROT_MODEL_MATRICES; i++)
                exec_opt.candidate_models.push_back(i);
        }
    }
    else
    {
        if (user_candidate_models.compare("") && (dna_ss == ss_undef))
        {
            int dna_matrices_bitv = 0;
            /* parse user defined models */
            istringstream f(user_candidate_models);
            string s;
            while (getline(f, s, ',')) {
                mt_index_t i, c_matrix = 0;
                for (i=0; i<N_DNA_MODEL_MATRICES; i++)
                {
                    if (!strcasecmp(dna_model_names[2*i].c_str(), s.c_str()) ||
                        !strcasecmp(dna_model_names[2*i+1].c_str(), s.c_str()))
                    {
                        c_matrix = i;
                        break;
                    }
                }
                if (i == N_DNA_MODEL_MATRICES)
                {
                    cerr << "Invalid dna matrix: " << s << endl;
                    return false;
                }
                dna_matrices_bitv |= 1<<c_matrix;
            }
            for (mt_index_t i=0; i<N_PROT_MODEL_MATRICES; i++)
            {
                if (dna_matrices_bitv&1)
                {
                    exec_opt.candidate_models.push_back(
                                dna_model_matrices_indices[i]);
                }
                dna_matrices_bitv >>= 1;
            }
        }
        else
        {
            if (dna_ss == ss_undef)
                exec_opt.subst_schemes = default_ss;
            else
                exec_opt.subst_schemes = dna_ss;
        }

        /* fill candidate matrices */
        switch(exec_opt.subst_schemes)
        {
        case ss_11:
            for (mt_index_t i=0; i<N_DNA_MODEL_MATRICES; i++)
                exec_opt.candidate_models.push_back(dna_model_matrices_indices[i]);
            break;
        case ss_7:
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[6]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[9]);

            exec_opt.candidate_models.push_back(dna_model_matrices_indices[2]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[3]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[0]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[1]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[10]);
            break;
        case ss_5:
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[2]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[3]);

            exec_opt.candidate_models.push_back(dna_model_matrices_indices[0]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[1]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[10]);
            break;
        case ss_3:
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[0]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[1]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[10]);
            break;
        case ss_203:
            for (mt_index_t i=0; i<203; i++)
                exec_opt.candidate_models.push_back(i);
            break;
        case ss_undef:
            /* ignore */
            break;
        }
    }

    assert(exec_opt.candidate_models.size() > 0);

    /* if there are no model specifications, include all */
    if (!exec_opt.model_params)
        exec_opt.model_params =
                MOD_PARAM_NO_RATE_VAR |
                MOD_PARAM_INV |
                MOD_PARAM_GAMMA |
                MOD_PARAM_INV_GAMMA;

    if (!(exec_opt.model_params &
         (MOD_PARAM_FIXED_FREQ | MOD_PARAM_ESTIMATED_FREQ)))
        exec_opt.model_params |= MOD_PARAM_FIXED_FREQ;

    return true;
}

int main(int argc, char *argv[])
{
    int return_val = EXIT_SUCCESS;
    time_t ini_global_time = time(NULL);

    if (argc > 1)
    {
        /* command line */
        mt_options opts;
        modeltest::ModelTest mt;
        mt_index_t cur_model;

        if (!parse_arguments(argc, argv, opts))
        {
            return(EXIT_FAILURE);
        }

        mt.build_instance(opts, tree_user_fixed);

        //mt.evaluate_models();
        cur_model = 0;
        for (cur_model=0; cur_model<mt.get_models().size(); cur_model++)
        {
            modeltest::Model *model = mt.get_models()[cur_model];
            time_t ini_t = time(NULL);
            if (!mt.evaluate_single_model(model, 0, opts.epsilon_param, opts.epsilon_opt))
            {
                cerr << "ERROR OPTIMIZING MODEL" << endl;
                return(MT_ERROR_OPTIMIZE);
            }

            /* print progress */
            cout << setw(5) << right << (cur_model+1) << "/"
                 << setw(5) << left << mt.get_models().size()
                 << setw(20) << left << model->get_name()
                 << setw(18) << right << setprecision(MT_PRECISION_DIGITS) << fixed
                 << model->get_lnl()
                 << setw(8) << time(NULL) - ini_t
                 << setw(8) << time(NULL) - ini_global_time << endl;
        }

        modeltest::ModelSelection bic_selection(mt.get_models(),
                                                modeltest::ic_bic);
        bic_selection.print(cout);
    }
    else
    {
        #ifndef CONSOLE
        /* launch GUI */
        QApplication a(argc, argv);
        modeltest::jModelTest w;
        w.show();
        return_val = a.exec();
        #endif
    }

    return return_val;
}