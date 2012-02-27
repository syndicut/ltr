// Copyright 2011 Yandex

#ifndef LTR_DATA_PREPROCESSORS_FAKE_PREPROCESSOR_LEARNER_H_
#define LTR_DATA_PREPROCESSORS_FAKE_PREPROCESSOR_LEARNER_H_


#include "ltr/data_preprocessors/data_preprocessor_learner.h"
#include "ltr/data_preprocessors/fake_data_preprocessor.h"

using ltr::IDataPreprocessorLearner;
using ltr::FakeDataPreprocessor;

namespace ltr {
template <typename TElement>
class FakePreprocessorLearner
    : public IDataPreprocessorLearner<TElement> {
  public:
  typedef boost::shared_ptr<FakePreprocessorLearner> Ptr;

  explicit FakePreprocessorLearner(
      const ParametersContainer& parameters = ParametersContainer())
      : preprocessor_(new FakeDataPreprocessor<TElement>) {
    this->setDefaultParameters();
    this->copyParameters(parameters);
    this->checkParameters();
  }

  void learn(const DataSet<TElement>& data_set) {}
  typename DataPreprocessor<TElement>::Ptr make() const;

  private:
  typename FakeDataPreprocessor<TElement>::Ptr preprocessor_;
};

// template realizations
template <typename TElement>
typename DataPreprocessor<TElement>::Ptr
    FakePreprocessorLearner<TElement>::make() const {
  return preprocessor_;
}
};

#endif  // LTR_DATA_PREPROCESSORS_FAKE_PREPROCESSOR_LEARNER_H_