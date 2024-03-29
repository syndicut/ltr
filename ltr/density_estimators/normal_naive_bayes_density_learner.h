// Copyright 2012 Yandex
#ifndef LTR_DENSITY_ESTIMATORS_NORMAL_NAIVE_BAYES_DENSITY_LEARNER_H_
#define LTR_DENSITY_ESTIMATORS_NORMAL_NAIVE_BAYES_DENSITY_LEARNER_H_

#include <Eigen/Dense>

#include <map>
#include <vector>

#include "ltr/density_estimators/base_probability_density_estimator.h"
#include "ltr/density_estimators/non_linear_discriminant_density_learner.h"
#include "ltr/data/data_set.h"
#include "ltr/data/object.h"
#include "ltr/utility/statistics_calculation.h"

using ltr::BaseProbabilityDensityEstimator;
using ltr::NonLinearDiscriminantDensityLearner;
using ltr::DataSet;
using ltr::Object;
using ltr::LabelToCovarianceMatrix;
using ltr::utility::CalculateFeaturesVariance;

namespace ltr {
/**
 * \class Probability density learner that uses Normal Naive Bayesian
 * approach to generate probability density estimator.
 */
template<class TElement>
class NormalNaiveBayesDensityLearner
  : public NonLinearDiscriminantDensityLearner<TElement> {
 public:
  NormalNaiveBayesDensityLearner() {
  }

  explicit NormalNaiveBayesDensityLearner(
    const ParametersContainer& parameters) {
  }

 private:
  /**
   * Function calculates the variance of every class from the training data set
   */
  void calculateVariance(const DataSet<TElement>& data_set,
                         map<double, VectorXd>* result) {
    vector<DataSet<Object> > splitted_data;
    SplitDataSetByActualLabel(data_set, &splitted_data);
    for (int subset_index = 0;
         subset_index < splitted_data.size();
         ++subset_index) {
      vector<double> variance;
      CalculateFeaturesVariance(splitted_data[subset_index], &variance);
      double label = splitted_data[subset_index][0].actual_label();
      (*result)[label] = StdVectorToEigenVector(variance);
    }
  }

  virtual void calculateCovarianceMatrix(const DataSet<TElement>& data_set,
                                         LabelToCovarianceMatrix* result) {
    LabelToMean mean;
    map<double, VectorXd> variance;
    this->calculateMean(data_set, &mean);
    calculateVariance(data_set, &variance);

    for (map<double, VectorXd>::iterator variance_iterator = variance.begin();
         variance_iterator != variance.end();
         ++variance_iterator) {
      double label = variance_iterator->first;
      int features_count = variance_iterator->second.size();
      (*result)[label] = MatrixXd(features_count, features_count);
      InitEigenMatrix(&(*result)[label]);

      for (int feature_index = 0;
           feature_index < features_count;
           ++feature_index) {
        (*result)[label](feature_index, feature_index) =
          variance[label](feature_index);
      }
    }
  };
};
};

#endif  // LTR_DENSITY_ESTIMATORS_NORMAL_NAIVE_BAYES_DENSITY_LEARNER_H_
