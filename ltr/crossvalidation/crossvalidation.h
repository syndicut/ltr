// Copyright 2011 Yandex

#ifndef LTR_CROSSVALIDATION_CROSSVALIDATION_H_
#define LTR_CROSSVALIDATION_CROSSVALIDATION_H_

#include <logog/logog.h>

#include <string>
#include <vector>

#include "ltr/learners/learner.h"
#include "ltr/measures/measure.h"
#include "ltr/crossvalidation/validation_result.h"
#include "ltr/crossvalidation/splitter.h"

using std::string;
using std::vector;

namespace ltr {
  namespace cv {
    /**
     * Performs crossvalidation
     * @param data_set - data set to run crossvalidation on
     * @param measures - measures, wich values will be calculated for every split
     * on test data
     * @param learner - learner used to produce scorers on train data
     * @param splitter - splitter used to split dataset into train + test sets
     * @returns ValidationResult with information about all splits performed
     */
    template<class TElement>
    ValidationResult Validate(
      const DataSet<TElement>& data_set,
      const vector< typename Measure<TElement>::Ptr >& measures,
      typename Learner<TElement>::Ptr learner,
      typename Splitter<TElement>::Ptr splitter);

    // template realizations
    template<class TElement>
    ValidationResult Validate(
        const DataSet<TElement>& data_set,
        const vector< typename Measure<TElement>::Ptr >& measures,
        typename Learner<TElement>::Ptr learner,
        typename Splitter<TElement>::Ptr splitter) {
      INFO("Starting validation");
      vector<string> measure_names;
      for (int i = 0; i < (int)measures.size(); ++i) {
        INFO("Adding %s measure, index eqaul to %d",
          measures[i]->alias().c_str(),
          i);
        measure_names.push_back(measures[i]->alias());
      }
      ValidationResult output(measure_names);

      for (int split_index = 0;
          split_index < splitter->splitCount(data_set);
          ++split_index) {
        INFO("Starting %d split", split_index);
        SplittedDataSet<TElement> current_splitted(
          splitter->split(split_index, data_set));

        learner->reset();
        learner->learn(current_splitted.train_set);

        string current_report = learner->report();
        typename Scorer::Ptr current_scorer = learner->make();

        current_scorer->predict(current_splitted.test_set);

        vector<double> current_measure_values;
        for (int measure_index = 0;
            measure_index < (int)measures.size();
            ++measure_index) {
          current_measure_values.push_back(
            measures[measure_index]->average(current_splitted.test_set));
        }

        output.addSplitInfo(current_scorer,
          current_report,
          current_measure_values);
      }

      return output;
    }
  };
};

#endif  // LTR_CROSSVALIDATION_CROSSVALIDATION_H_
