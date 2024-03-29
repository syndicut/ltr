// Copyright 2012 Yandex

#ifndef LTR_LEARNERS_DECISION_TREE_CONDITIONS_LEARNER_H_
#define LTR_LEARNERS_DECISION_TREE_CONDITIONS_LEARNER_H_

#include <vector>

#include "ltr/data/data_set.h"

#include "ltr/interfaces/parameterized.h"

#include "ltr/learners/decision_tree/condition.h"

#include "ltr/utility/shared_ptr.h"

using std::vector;

namespace ltr {
namespace decision_tree {
class ConditionsLearner : public Parameterized {
 public:
  typedef ltr::utility::shared_ptr<ConditionsLearner> Ptr;
  typedef ltr::utility::shared_ptr<ConditionsLearner> BasePtr;
  /**
   * Function used to gen the next conditions set.
   * Returns 0 if can't generate or 1 if it could.
   * @param result - pointer to the vector to save result in.
   */
  virtual int getNextConditions(vector<Condition::Ptr>* result) = 0;
  /**
   * Function used to set data set to generate conditions.
   */
  void setDataSet(DataSet<Object> data) {
    data_ = data;
    init();
  }
  /** Function used as a callback to tell ConditionsLearner
   * the quality of the last conditions set.
   */
  virtual void setLastSplittingQuality(double quality) {}

 private:
  virtual void init() = 0;

 protected:
  DataSet<Object> data_;
};

class FakeConditionsLearner : public ConditionsLearner {
 public:
  virtual int getNextConditions(vector<Condition::Ptr>* result) {
    return 0;
  }

 private:
  virtual void init() {}
};
};
};

#endif  // LTR_LEARNERS_DECISION_TREE_CONDITIONS_LEARNER_H_
