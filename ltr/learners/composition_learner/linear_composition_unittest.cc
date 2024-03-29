// Copyright 2011 Yandex

#include <gtest/gtest.h>

#include "ltr/data/data_set.h"
#include "ltr/data/object.h"
#include "ltr/data/feature_info.h"
#include "ltr/utility/numerical.h"

#include "ltr/learners/composition_learner/data_set_weights_updater.h"
#include "ltr/learners/composition_learner/composition_scorer_weights_updater.h"

#include "ltr/learners/composition_learner/composition_learner.h"
#include "ltr/learners/composition_learner/ada_boost_learner.h"
#include "ltr/learners/composition_learner/ada_rank_data_set_weights_updater.h"
#include "ltr/learners/composition_learner/ada_boost_data_set_weights_updater.h"
#include "ltr/learners/composition_learner/ada_rank_composition_scorer_weights_updater.h"
#include "ltr/learners/composition_learner/average_composition_scorer_weights_updater.h"

#include "ltr/scorers/composition_scorers/linear_composition_scorer.h"
#include "ltr/learners/best_feature_learner/best_feature_learner.h"
#include "ltr/measures/abs_error.h"
#include "ltr/measures/true_point.h"
#include "ltr/data_preprocessors/data_random_sampler.h"
#include "ltr/feature_converters/feature_random_sampler_learner.h"

using ltr::Object;
using ltr::DataSet;
using ltr::FeatureInfo;
using ltr::utility::DoubleEqual;

using ltr::composition::LinearCompositionScorer;
using ltr::composition::FakeDataSetWeightsUpdater;
using ltr::composition::FakeCompositionScorerWeightsUpdater;
using ltr::composition::AdaRankDataSetWeightsUpdater;
using ltr::composition::AdaRankScorerWeightsUpdater;
using ltr::composition::CompositionLearner;
using ltr::composition::AverageCompositionScorerWeightsUpdater;
using ltr::FeatureRandomSamplerLearner;
using ltr::DataRandomSampler;
using ltr::BestFeatureLearner;
using ltr::AbsError;
using ltr::TruePoint;

const int data_size = 11;

class LinearCompositionTest : public ::testing::Test {
  public:
  LinearCompositionTest()
    : data(FeatureInfo(4)) {}
  protected:
  virtual void SetUp() {
    for (int i = 0; i < data_size; ++i) {
      Object object;
      object << i << data_size - i << i * i << i * (data_size - i);
      object.set_actual_label(i);
      object.set_predicted_label(data_size - i);
      data.add(object);
    }
  }

  DataSet<Object> data;
};

TEST_F(LinearCompositionTest, SimpleLinearCompositionTest) {
  CompositionLearner<Object, LinearCompositionScorer> linear_composition_learner;

  AbsError::Ptr abs_error(new AbsError);

  BestFeatureLearner<Object>::Ptr 
    best_feature_learner(new BestFeatureLearner<Object>(abs_error));
  linear_composition_learner.set_weak_learner(best_feature_learner);

  linear_composition_learner.learn(data);
  LinearCompositionScorer::Ptr linear_scorer = linear_composition_learner.makeSpecific();

  EXPECT_EQ(10, linear_scorer->size());
  for (int i = 0; i < linear_scorer->size(); ++i) {
    linear_scorer->at(i).scorer->predict(data);
    EXPECT_TRUE(DoubleEqual(0.0, abs_error->average(data)))
      << abs_error->average(data);
  }

  AverageCompositionScorerWeightsUpdater<Object, LinearCompositionScorer>::Ptr
    average_composition_scorer_weights_updater(
      new AverageCompositionScorerWeightsUpdater<Object, LinearCompositionScorer>);

  CompositionLearner<Object, LinearCompositionScorer> average_linear_composition_learner;
  average_linear_composition_learner.set_composition_scorer_weights_updater(
    average_composition_scorer_weights_updater);
  average_linear_composition_learner.set_weak_learner(best_feature_learner);
  average_linear_composition_learner.learn(data);
  LinearCompositionScorer::Ptr average_linear_scorer =
    average_linear_composition_learner.makeSpecific();
  average_linear_scorer->predict(data);
  EXPECT_NEAR(0.0, abs_error->average(data), 1e-8);
}

TEST_F(LinearCompositionTest, BaggingSimpleLinearCompositionTest) {
  CompositionLearner<Object, LinearCompositionScorer> linear_composition_learner;

  AbsError::Ptr abs_error(new AbsError);  

  BestFeatureLearner<Object>::Ptr 
    best_feature_learner(new BestFeatureLearner<Object>(abs_error));
  DataRandomSampler<Object>::Ptr bagging(new DataRandomSampler<Object>);
  best_feature_learner->addDataPreprocessor(bagging);
  linear_composition_learner.set_weak_learner(best_feature_learner);

  linear_composition_learner.learn(data);
  LinearCompositionScorer::Ptr linear_scorer =
    linear_composition_learner.makeSpecific();

  EXPECT_EQ(10, linear_scorer->size());
  for (int i = 0; i < linear_scorer->size(); ++i) {
    linear_scorer->at(i).scorer->predict(data);
    EXPECT_TRUE(DoubleEqual(0.0, abs_error->average(data)))
      << abs_error->average(data);
  }

  bagging->set_sampling_fraction(2.);
  linear_composition_learner.set_number_of_iterations(15);
  linear_composition_learner.learn(data);
  linear_scorer = linear_composition_learner.makeSpecific();

  EXPECT_EQ(25, linear_scorer->size());
  for (int i = 0; i < linear_scorer->size(); ++i) {
    linear_scorer->at(i).scorer->predict(data);
    EXPECT_TRUE(DoubleEqual(0.0, abs_error->average(data)))
      << abs_error->average(data);
  }
}

TEST_F(LinearCompositionTest, RSMSimpleLinearCompositionTest) {
  CompositionLearner<Object, LinearCompositionScorer> linear_composition_learner;

  AbsError::Ptr abs_error(new AbsError);  

  FeatureRandomSamplerLearner<Object>::Ptr
    rsm(new FeatureRandomSamplerLearner<Object>);

  BestFeatureLearner<Object>::Ptr 
    best_feature_learner(new BestFeatureLearner<Object>(abs_error));
  best_feature_learner->addFeatureConverterLearner(rsm);
  linear_composition_learner.set_weak_learner(best_feature_learner);

  linear_composition_learner.learn(data);
  LinearCompositionScorer::Ptr linear_scorer =
    linear_composition_learner.makeSpecific();

  EXPECT_NO_THROW(linear_scorer->predict(data));
}

TEST_F(LinearCompositionTest, AdaRankDSWUSimpleLinearCompositionTest) {
  CompositionLearner<Object, LinearCompositionScorer> ada_linear_composition_learner;

  AbsError::Ptr abs_error(new AbsError);
  TruePoint::Ptr true_point(new TruePoint);

  AdaRankDataSetWeightsUpdater<Object>::Ptr
    ada_rank_data_set_weight_updater(
      new AdaRankDataSetWeightsUpdater<Object>(true_point));
  ada_linear_composition_learner.set_data_set_weights_updater(ada_rank_data_set_weight_updater);

  BestFeatureLearner<Object>::Ptr 
    best_feature_learner(new BestFeatureLearner<Object>(abs_error));
  ada_linear_composition_learner.set_weak_learner(best_feature_learner);

  ada_linear_composition_learner.learn(data);
  LinearCompositionScorer::Ptr linear_scorer =
    ada_linear_composition_learner.makeSpecific();

  EXPECT_NO_THROW(linear_scorer->predict(data));
}

TEST_F(LinearCompositionTest, AdaRankCSWUSimpleLinearCompositionTest) {
  CompositionLearner<Object, LinearCompositionScorer> ada_linear_composition_learner;

  TruePoint::Ptr true_point(new TruePoint);
  AbsError::Ptr abs_error(new AbsError); 

  AdaRankScorerWeightsUpdater<Object, LinearCompositionScorer>::Ptr
    ada_rank_weight_updater(new
      AdaRankScorerWeightsUpdater<Object, LinearCompositionScorer>(true_point));
  ada_linear_composition_learner.set_composition_scorer_weights_updater(ada_rank_weight_updater);

  BestFeatureLearner<Object>::Ptr 
    best_feature_learner(new BestFeatureLearner<Object>(abs_error));
  ada_linear_composition_learner.set_weak_learner(best_feature_learner);

  ada_linear_composition_learner.learn(data);
  LinearCompositionScorer::Ptr linear_scorer =
    ada_linear_composition_learner.makeSpecific();

  EXPECT_NO_THROW(linear_scorer->predict(data));
}

TEST_F(LinearCompositionTest, AdaRankBeggingRSMSimpleLinearCompositionTest) {
  CompositionLearner<Object, LinearCompositionScorer> ada_learner;

  TruePoint::Ptr true_point(new TruePoint);
  AbsError::Ptr abs_error(new AbsError);

  AdaRankScorerWeightsUpdater<Object, LinearCompositionScorer>::Ptr
    ada_rank_weights_updater(new
      AdaRankScorerWeightsUpdater<Object, LinearCompositionScorer>);
  ada_rank_weights_updater->set_measure(true_point);
  ada_learner.set_composition_scorer_weights_updater(ada_rank_weights_updater);

  AdaRankDataSetWeightsUpdater<Object>::Ptr
    ada_rank_data_set_weights_updater(
      new AdaRankDataSetWeightsUpdater<Object>);
  ada_rank_data_set_weights_updater->set_measure(true_point);
  ada_learner.set_data_set_weights_updater(ada_rank_data_set_weights_updater);

  BestFeatureLearner<Object>::Ptr 
    best_feature_learner(new BestFeatureLearner<Object>(abs_error));
  ada_learner.set_weak_learner(best_feature_learner);

  FeatureRandomSamplerLearner<Object>::Ptr
    random_sampler_learner(new FeatureRandomSamplerLearner<Object>);
  best_feature_learner->addFeatureConverterLearner(
    random_sampler_learner);

  DataRandomSampler<Object>::Ptr bagging(new DataRandomSampler<Object>);
  best_feature_learner->addDataPreprocessor(bagging);

  ada_learner.learn(data);
  LinearCompositionScorer::Ptr linear_scorer = ada_learner.makeSpecific();

  EXPECT_NO_THROW(linear_scorer->predict(data));
}
