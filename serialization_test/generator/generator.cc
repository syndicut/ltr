// Copyright 2012 Yandex

#include <boost/lexical_cast.hpp>

#include "serialization_test/generator/generator_utility.h"

#include "ltr/measures/measure.h"
#include "ltr/learners/best_feature_learner.h"
#include "ltr/learners/id3_learner.h"
#include "ltr/measures/abs_error.h"

using ltr::BestFeatureLearner;
using ltr::AbsError;
using ltr::Measure;
using ltr::ID3_Learner;

using namespace serialization_test;

int main(int argc, char* argv[]) {
  Generator generator;

  Measure<Object>::Ptr abs_error(new AbsError());
  BestFeatureLearner<Object>::Ptr bf_learner(new BestFeatureLearner<Object>(abs_error));
  bf_learner->learn(generator.train_data);
  generator.setScorerTest(bf_learner, "bf_learner");

  ID3_Learner<Object>::Ptr id3_learner;
  id3_learner->learn(generator.train_data);
  generator.setScorerTest(id3_learner, "id3_learner");

  generator.setMainCode();
  generator.write(argv[1]);
  return 0;
}