// Copyright 2011 Yandex

#ifndef LTR_LEARNERS_GP_LEARNER_GP_LEARNER_H_
#define LTR_LEARNERS_GP_LEARNER_GP_LEARNER_H_

#include <boost/lexical_cast.hpp>

#include <string>
#include <functional>

#include "contrib/puppy/Puppy.hpp"

#include "ltr/learners/learner.h"
#include "ltr/learners/gp_learner/gp_primitives.h"
#include "ltr/learners/gp_learner/gp_functions.h"
#include "ltr/scorers/gp_scorer.h"
#include "ltr/measures/measure.h"



#include "ltr/parameters_container/parameters_container.h"


namespace ltr {
namespace gp {
/**
 \class GPLearner
 Implements genetic programming approach applied to learning to rank.
 \tparam TElement object container of those the dataset consists (it can be
 Object, ObjectPair, ObjectList).
 */
template <typename TElement>
class GPLearner : public Learner<TElement, GPScorer> {
  public:
  /** Constructor creates a GPLearner.
   * \param p_Measure shared pointer to the measure that would be maximized on
   * a dataset within learning.
   * \param parameters the ParametersContainer parameters from which would
   * overwrite the default parameters.
   */
  GPLearner(typename Measure<TElement>::Ptr p_Measure,
      const ParametersContainer& parameters = ParametersContainer())
  : featureCountInContext_(0),
  inPopulationBestTreeIdx_(0),
  Learner<TElement, GPScorer>("GPLearner") {
    this->setMeasure(p_Measure);
    this->setDefaultParameters();
    this->copyParameters(parameters);
  }
  /** Constructor creates a GPLearner. But leaves p_measure uninitialized.
   * \param parameters the ParametersContainer parameters from which would
   * overwrite the default parameters.
   */
  GPLearner(const ParametersContainer& parameters = ParametersContainer())
  : featureCountInContext_(0),
  inPopulationBestTreeIdx_(0),
  Learner<TElement, GPScorer>("GPLearner") {
    this->setDefaultParameters();
    this->copyParameters(parameters);
  }

  /** The function sets up default parameters for genetic learning process.
   */
  virtual void setDefaultParameters() {
    this->clearParameters();
    this->addNewParam("POP_SIZE", 10);
    this->addNewParam("NBR_GEN", 3);
    this->addNewParam("NBR_PART", 2);
    this->addNewParam("MAX_DEPTH", 35);
    this->addNewParam("MIN_INIT_DEPTH", 2);
    this->addNewParam("MAX_INIT_DEPTH", 5);
    this->addNewParam("INIT_GROW_PROBA", 0.5);
    this->addNewParam("CROSSOVER_PROBA", 0.9);
    this->addNewParam("CROSSOVER_DISTRIB_PROBA", 0.5);
    this->addNewParam("MUT_STD_PROBA", 0.05);
    this->addNewParam("MUT_MAX_REGEN_DEPTH", 5);
    this->addNewParam("MUT_SWAP_PROBA", 0.05);
    this->addNewParam("MUT_SWAP_DISTRIB_PROBA", 0.5);
    this->addNewParam("SEED", 1);
    this->addNewParam("USE_ADD", true);
    this->addNewParam("USE_SUB", true);
    this->addNewParam("USE_MUL", true);
    this->addNewParam("USE_DIV", true);
    this->addNewParam("USE_IF", true);
    this->addNewParam("USE_EFEM", true);
  }
  /** The method checks the correctness of the parameters in the parameters
   * container. If one of them is not correct it throws
   * std::logical_error(PARAMETER_NAME).
   */

  virtual void checkParameters() const {
    std::binder2nd<std::greater<int> > GreaterThen0 =
        std::bind2nd(std::greater<int>(), 0);
    std::binder2nd<std::greater<int> > GreaterThen1 =
        std::bind2nd(std::greater<int>(), 1);
    Parameterized::checkParameter<int>("POP_SIZE",            GreaterThen0);
    Parameterized::checkParameter<int>("NBR_GEN",             GreaterThen0);
    Parameterized::checkParameter<int>("NBR_PART",            GreaterThen1);
    Parameterized::checkParameter<int>("MAX_DEPTH",           GreaterThen0);
    Parameterized::checkParameter<int>("MIN_INIT_DEPTH",      GreaterThen0);
    Parameterized::checkParameter<int>("MUT_MAX_REGEN_DEPTH", GreaterThen0);

    Parameterized::checkParameter<int>("MAX_INIT_DEPTH",
       std::bind2nd(std::greater<int>(), this->parameters().
                    template Get<int>("MIN_INIT_DEPTH") - 1));


    const Belongs<double> belongs(0, 1);

    Parameterized::checkParameter<double>("INIT_GROW_PROBA",         belongs);
    Parameterized::checkParameter<double>("CROSSOVER_PROBA",         belongs);
    Parameterized::checkParameter<double>("CROSSOVER_DISTRIB_PROBA", belongs);
    Parameterized::checkParameter<double>("MUT_STD_PROBA",           belongs);
    Parameterized::checkParameter<double>("MUT_SWAP_PROBA",          belongs);
    Parameterized::checkParameter<double>("MUT_SWAP_DISTRIB_PROBA",  belongs);
  }
  /** The function recreates the context and reinitializes the population.
   */
  void reset() {
    this->initContext();
    this->initPopulation();
  }

  /** The function sets up context and population from the given GPScorer.
   * \param in_scorer GPScorer whose population and context would be set up.
   */
  void setInitialScorer(const GPScorer& in_scorer) {
    population_ = in_scorer.population_;
    context_ = in_scorer.context_;
    featureCountInContext_ = in_scorer.featureCountInContext_;
    inPopulationBestTreeIdx_ = in_scorer.inPopulationBestTreeIdx_;
  }
  /** The function return trained GPscorer after learning process
   */
  GPScorer makeImpl() const {
    return GPScorer(this->population_, this->context_,
        this->featureCountInContext_, this->inPopulationBestTreeIdx_);
  }

  private:
  template <class T>
  struct Belongs: public std::unary_function<T, bool> {
    Belongs(const T &min, const T &max): min_(min), max_(max) { }
    bool operator()(const T& x) const {
      return x >= min_ && x <= max_;
    }

  private:
    const T &min_;
    const T &max_;
  };

  /** Method clears and adds primitives to the context.
   */
  void initContext() {
    Puppy::Context newContext;
    context_= newContext;

    const ParametersContainer &params = this->parameters();

    context_.mRandom.seed(params.template Get<int>("SEED"));
    if (params.template Get<bool>("USE_ADD")) context_.insert(new Add);
    if (params.template Get<bool>("USE_SUB")) context_.insert(new Subtract);
    if (params.template Get<bool>("USE_MUL")) context_.insert(new Multiply);
    if (params.template Get<bool>("USE_DIV")) context_.insert(new Divide);
    if (params.template Get<bool>("USE_IF"))  context_.insert(new IfThenFunc);
    if (params.template Get<bool>("USE_EFEM")) context_.insert(new Ephemeral);

    for (int featureIdx = 0;
        featureIdx < featureCountInContext_;
        ++featureIdx) {
      std::string featureName = "feature[";
      featureName += boost::lexical_cast<string>(featureIdx);
      featureName += "]";
      context_.insert(new Puppy::TokenT<double>(featureName));
    }
  }

  /** Method creates initial population.
   */
  void initPopulation() {
    population_.clear();
    const ParametersContainer &params = this->parameters();

    population_.resize(params.template Get<int>("POP_SIZE"));
    Puppy::initializePopulation(population_, context_,
                                params.template Get<double>("INIT_GROW_PROBA"),
                                params.template Get<int>("MIN_INIT_DEPTH"),
                                params.template Get<int>("MAX_INIT_DEPTH"));
  }

  /** \brief This function implements the changes made in the population at each
   *  algorithm's iteration.
   */
  virtual void evaluationStrategyStepImpl() {
    std::cout << "Tournament.\n";
    const ParametersContainer &params = this->parameters();

    Puppy::applySelectionTournament(population_, context_,
        params.template Get<int>("NBR_PART"));

    std::cout << "Crossover.\n";
    Puppy::applyCrossover(population_, context_,
                         params.template Get<double>("CROSSOVER_PROBA"),
                         params.template Get<double>("CROSSOVER_DISTRIB_PROBA"),
                         params.template Get<int>("MAX_DEPTH"));

    std::cout << "Mutation standart.\n";
    Puppy::applyMutationStandard(population_, context_,
                                params.template Get<double>("MUT_STD_PROBA"),
                                params.template Get<int>("MUT_MAX_REGEN_DEPTH"),
                                params.template Get<int>("MAX_DEPTH"));

    std::cout << "Mutation swap.\n";
    Puppy::applyMutationSwap(population_, context_,
                         params.template Get<double>("MUT_SWAP_PROBA"),
                         params.template Get<double>("MUT_SWAP_DISTRIB_PROBA"));
  }

  /** The implementation of genetic programming optimization approach.
   * \param data DataSet on which the p_Measure would be maximized within the
   * learning procedure.
   */
  void learnImpl(const DataSet<TElement>& data) {
    if (data.featureCount() != featureCountInContext_) {
      featureCountInContext_ = data.featureCount();
      reset();
    }

    std::cout << "Evaluating data for the first time.\n";
    this->evaluatePopulation(data);

    std::cout << "The population looks like: \n";
    for (size_t treeIdx = 0; treeIdx < population_.size(); ++treeIdx) {
      using ::operator <<;
      std::cout << population_[treeIdx] << std::endl;
    }

    std::cout << "Evolution begins.\n";
    for (int generationIdx = 0;
       generationIdx < this->parameters().template Get<int>("NBR_GEN");
       ++generationIdx) {
      std::cout << "Generation "<< generationIdx << ".\n";

      std::cout << "Calling strategy\n";
      this->evaluationStrategyStepImpl();

      std::cout << "Evaluation.\n";
      this->evaluatePopulation(data);

      inPopulationBestTreeIdx_ = 0;
      for (size_t treeIdx = 1; treeIdx < population_.size(); ++treeIdx) {
        if (population_[inPopulationBestTreeIdx_].mFitness <
            population_[treeIdx].mFitness) {
          inPopulationBestTreeIdx_ = treeIdx;
        }
      }

      std::cout
      << "The best one is number " << inPopulationBestTreeIdx_ << ".\n";
      using ::operator <<;
      std::cout << population_[inPopulationBestTreeIdx_] << std::endl;
      std::cout << "with fitness " <<
          population_[inPopulationBestTreeIdx_].mFitness << "\n";
    }
  }

  /** Method evaluates the population, it sets individ tree fitness to the
   *  average on the data set metric value.
   *  @param data data set for calculation of the average metric value
   */
  void evaluatePopulation(const DataSet<TElement>& data) {
    for (size_t treeIdx = 0; treeIdx < population_.size(); ++treeIdx) {
      if (population_[treeIdx].mValid) {
        continue;
      }
      markDataSetWithTree<TElement>(data, &context_, &population_[treeIdx]);

      // This line yields a topic for research. Why so?
      //
      double measureVal = this->p_measure_->weightedAverage(data);

      population_[treeIdx].mFitness = static_cast<float>(measureVal);
      population_[treeIdx].mValid = true;
    }
  }

  /** Smart pointer to the measure object, the measure would be maximized
   * within the learning procedure.
   */
  typename Measure<TElement>::Ptr p_Measure_;
  size_t featureCountInContext_;
  /** The index of the best Puppy::tree (formula, individ) in current
   * population.
   */
  size_t inPopulationBestTreeIdx_;

  protected:
  /** The set of Puppy::trees (formulas that represent the optimization space),
   * that represent a population within the learning procedure.
   */
  Population population_;
  /** Context that contains all the routine needed to build Puppy::trees within
   * the population.
   */
  Puppy::Context context_;
  /** The number of features, to process dataset with the number of features
   * the context_ is constructed for.
   */
};

template class GPLearner<Object>;
template class GPLearner<ObjectPair>;
template class GPLearner<ObjectList>;
}
}
#endif  // LTR_LEARNERS_GP_LEARNER_GP_LEARNER_H_