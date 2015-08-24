#include "modeltest.h"

#include "msapll.h"
#include "treepll.h"

#include <iostream>
#include <cassert>
#include <algorithm>

namespace modeltest {

unsigned int mt_errno;
char mt_errmsg[200] = {0};

using namespace std;

ModelTest::ModelTest(mt_size_t _number_of_threads)
    : number_of_threads(_number_of_threads)
{
    setlocale(LC_NUMERIC, "C");
    create_instance();
}

ModelTest::~ModelTest()
{
    free_stuff();
}

void ModelTest::create_instance()
{
    current_instance = new selection_instance;
    current_instance->msa = 0;
    current_instance->tree = 0;
}

static bool sort_forwards(Model * m1, Model * m2)
{
    mt_size_t p1 = m1->get_n_free_variables();
    mt_size_t p2 = m2->get_n_free_variables();
    if (m1->is_G())
        p1 += 20;
    if (m2->is_G())
        p2 += 20;
    if (m1->is_I())
        p1 += 10;
    if (m2->is_I())
        p2 += 10;

    return p1 < p2;
}

static bool sort_backwards(Model * m1, Model * m2)
{
    mt_size_t p1 = m1->get_n_free_variables();
    mt_size_t p2 = m2->get_n_free_variables();
    if (m1->is_G())
        p1 += 20;
    if (m2->is_G())
        p2 += 20;
    if (m1->is_I())
        p1 += 10;
    if (m2->is_I())
        p2 += 10;

    return p1 > p2;
}

static bool build_models(mt_options & options,
                         vector<Model *> &c_models,
                         bool eval_all_matrices)
{
    UNUSED(eval_all_matrices);
    mt_size_t n_matrices = (mt_size_t) options.candidate_models.size();
    mt_size_t n_models = 0;

    int freq_params = options.model_params & MOD_MASK_FREQ_PARAMS;
    int rate_params = options.model_params & MOD_MASK_RATE_PARAMS;
    if (!freq_params)
    {
        mt_errno = MT_ERROR_MODELS;
         snprintf(mt_errmsg, 200, "Model frequencies is empty");
        return false;
    }

    int it_model_params = rate_params;
    while (it_model_params)
    {
        if (it_model_params & 1) n_models += n_matrices;
        it_model_params >>= 1;
    }

    if (freq_params == 3)
        n_models *= 2;
    c_models.reserve(n_models);

    for (int i=1; i<64; i*=2)
    {
        int cur_rate_param = rate_params & i;
        if (cur_rate_param)
        {
            for (mt_index_t j=0; j<n_matrices; j++)
            {
                if (options.datatype == dt_dna)
                {
                    if (freq_params & MOD_PARAM_FIXED_FREQ)
                        c_models.push_back(
                                    new DnaModel(options.candidate_models[j], cur_rate_param | MOD_PARAM_FIXED_FREQ)
                                    );
                    if (freq_params & MOD_PARAM_ESTIMATED_FREQ)
                        c_models.push_back(
                                    new DnaModel(options.candidate_models[j], cur_rate_param | MOD_PARAM_ESTIMATED_FREQ)
                                    );
                }
                else if (options.datatype == dt_protein)
                {
                    if (freq_params & MOD_PARAM_FIXED_FREQ)
                        c_models.push_back(
                                    new ProtModel(options.candidate_models[j], cur_rate_param | MOD_PARAM_FIXED_FREQ)
                                    );
                    if (freq_params & MOD_PARAM_ESTIMATED_FREQ)
                        c_models.push_back(
                                    new ProtModel(options.candidate_models[j], cur_rate_param | MOD_PARAM_ESTIMATED_FREQ)
                                    );
                }
                else
                    assert(0);
            }
        }
    }
    assert(c_models.size() == n_models);

    return true;
}

ModelOptimizer * ModelTest::get_model_optimizer(Model * model,
                                     partition_t & partition,
                                     mt_index_t thread_number) const
{
    if( thread_number > number_of_threads)
    {
        return 0;
    }
    MsaPll *msa = static_cast<MsaPll *>(current_instance->msa);
    TreePll *tree = static_cast<TreePll *>(current_instance->tree);
    return new ModelOptimizerPll(msa, tree, model, partition,
                                 current_instance->n_catg,
                                 thread_number);
}

bool ModelTest::evaluate_single_model(Model * model,
                                      partition_t & partition,
                                      mt_index_t thread_number,
                                      double tolerance,
                                      double epsilon)
{
    ModelOptimizer * mopt = get_model_optimizer(model,
                                                partition,
                                                thread_number);
    assert(mopt);

    bool result = mopt->run(epsilon, tolerance);
    delete mopt;

    return result;
}

bool ModelTest::evaluate_models(partition_t & partition)
{
    assert(current_instance);

    for (size_t i=0; i<current_instance->c_models.size(); i++)
    {
        Model * model = current_instance->c_models[i];
        evaluate_single_model(model, partition);
    }
    return true;
}

bool ModelTest::test_msa(std::string const& msa_filename,
                         mt_size_t *n_tips,
                         mt_size_t *n_sites)
{
   return MsaPll::test(msa_filename, n_tips, n_sites);
}

bool ModelTest::test_tree(std::string const& tree_filename,
              mt_size_t *n_tips)
{
    return TreePll::test_tree(tree_filename, n_tips);
}

bool ModelTest::test_link(Msa const *msa,
                          Tree const *tree)
{
    mt_index_t i, j;
    mt_index_t n_tips;
    mt_size_t tree_taxa_found = 0;

    /* number of sequences and tips must agree */
    if (msa->get_n_sequences() == tree->get_n_tips())
        n_tips = msa->get_n_sequences();
    else
        return false;

    /* find sequences and link them with the corresponding taxa */
    for (i = 0; i < n_tips; ++i)
    {
        bool found = false;
        const char * header = msa->get_header (i);
        for (j = 0; j < n_tips; ++j)
        {
            if (!tree->get_label(j).compare(header))
            {
                if (tree_taxa_found &= 1<<j)
                {
                    mt_errno = MT_ERROR_ALIGNMENT_DUPLICATED;
                     snprintf(mt_errmsg, 200, "Duplicated sequence %s", header);
                    return false;
                }
                tree_taxa_found |= 1<<j;
                found = true;
                break;
            }
        }

        if (!found)
        {
            mt_errno = MT_ERROR_TREE_MISSING;
             snprintf(mt_errmsg, 200, "Missing taxon %s in tree", header);
            return false;
        }
    }

    i = 1;
    mt_index_t limit = (mt_index_t)1<<n_tips;
    while (i <= limit)
    {
        if (!(tree_taxa_found | i ))
        {
            mt_errno = MT_ERROR_ALIGNMENT_MISSING;
             snprintf(mt_errmsg, 200, "Missing sequence %s in MSA",tree->get_label(i).c_str());
            return false;
        }
        i = i<<1;
    }

    return true;
}

bool ModelTest::build_instance(mt_options & options, bool eval_all_matrices)
{
    free_stuff ();
    create_instance ();

    current_instance->msa = new MsaPll (options.msa_filename);
    current_instance->start_tree = options.starting_tree;

    if (options.partitions_desc)
    {
        if (options.partitions_eff)
            delete options.partitions_eff;
        options.partitions_eff = new std::vector<partition_t>(*options.partitions_desc);
        current_instance->msa->reorder_sites(*options.partitions_eff);
        //current_instance->msa->print();
    }
    switch (options.starting_tree)
    {
    case tree_user_fixed:
        if( options.tree_filename.compare ("") )
      {
        current_instance->tree = new TreePll (options.starting_tree, options.tree_filename, number_of_threads);
        if (!test_link(current_instance->msa, current_instance->tree))
        {
            /* clean memory */
            delete current_instance->msa;
            current_instance->msa = 0;
            delete current_instance->tree;
            current_instance->tree = 0;
            return false;
        }
      }
    else
      {
        /* unimplemented */
        mt_errno = MT_ERROR_TREE;
        snprintf(mt_errmsg, 200, "User tree is not defined");
        return false;
      }
        break;
    case tree_mp:
    case tree_ml_gtr_fixed:
    case tree_ml_jc_fixed:
        current_instance->tree = new TreePll (options.starting_tree, options.msa_filename, number_of_threads);
        current_instance->tree->print();
        break;
    case tree_ml:
        mt_errno = MT_ERROR_UNIMPLEMENTED;
        snprintf(mt_errmsg, 200, "Per-model ML tree is not implemented yet");
        return false;
    }

    if (current_instance->msa->get_n_sequences() !=
	current_instance->tree->get_n_tips())
      {
        mt_errno = MT_ERROR_TREE;
         snprintf(mt_errmsg, 200, "Tips do not agree! %d sequences vs %d tips",
                 current_instance->msa->get_n_sequences(),
                 current_instance->tree->get_n_tips());
        return false;
      }
    current_instance->n_tips = current_instance->msa->get_n_sequences();

    /* evaluate partitions */
    if( options.partitions_filename.compare (""))
    {

    }

    if (options.model_params & (MOD_PARAM_GAMMA | MOD_PARAM_INV_GAMMA))
      current_instance->n_catg = options.n_catg;
    else
      current_instance->n_catg = 1;

    if (!build_models (options,
               current_instance->c_models, eval_all_matrices))
      {
        return false;
      }

    /*
     * sort candidate models by
     * estimated computational needs
     */
    bool (*fsort)(Model *, Model *);
    if (number_of_threads == 1)
        fsort = &sort_forwards;
    else
        fsort = &sort_backwards;
    sort(current_instance->c_models.begin(),
         current_instance->c_models.end(),
         fsort);

    return true;
}

vector<Model *> const& ModelTest::get_models() const
{
    return current_instance->c_models;
}

bool ModelTest::set_models(const std::vector<modeltest::Model *> &c_models)
{
    /* validate */
    if (!current_instance || current_instance->c_models.size() != c_models.size())
        return false;
    for (size_t i=0; i<c_models.size(); i++)
        if (c_models[i]->get_name().compare(current_instance->c_models[i]->get_name()))
            return false;
    for (size_t i=0; i<c_models.size(); i++)
    {
        current_instance->c_models[i]->clone(c_models[i]);
    }
    return true;
}

void ModelTest::free_stuff()
{
    if (current_instance)
    {
        for (size_t i=0; i<current_instance->c_models.size(); i++)
            delete current_instance->c_models[i];

        if (current_instance->msa)
            delete current_instance->msa;
        if (current_instance->tree)
            delete current_instance->tree;

        delete current_instance;
    }
}
}
