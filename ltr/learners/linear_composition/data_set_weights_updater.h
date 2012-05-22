// Copyright 2011 Yandex

#ifndef LTR_LEARNERS_LINEAR_COMPOSITION_DATA_SET_WEIGHTS_UPDATER_H_
#define LTR_LEARNERS_LINEAR_COMPOSITION_DATA_SET_WEIGHTS_UPDATER_H_

#include <boost/shared_ptr.hpp>

#include <string>

#include "ltr/measures/measure.h"
#include "ltr/data/data_set.h"
#include "ltr/scorers/linear_composition_scorer.h"
#include "ltr/interfaces/aliaser.h"
#include "ltr/interfaces/parameterized.h"

using std::string;
using ltr::Measure;
using ltr::DataSet;
using ltr::LinearCompositionScorer;

namespace ltr {
namespace lc {
  /**
   * Is a parameter for linear composition learner. Implements a strategy of
   * updating dataset's element's weights while adding a new scorer to
   * linear composition scorer. The idea is to increase weights of "hard" elements
   * with bad measure values and decrease weights of "easy" elements with good
   * measure values. Thus next scorer in linear composition could be learned
   * primary on bad elements and level the limitations of previous scorers
   */
  template <class TElement>
  class DataSetWeightsUpdater : public Aliaser, public Parameterized {
  public:
    typedef boost::shared_ptr<DataSetWeightsUpdater> Ptr;

    explicit DataSetWeightsUpdater(const string& alias) : Aliaser(alias) {}

    /**
     * Updates dataset's weights
     * @param data - dataset to update it's weights
     * @param lin_scorer - linear composition scorer with already updated 
     * by ltr::lc::LCScorerWeightsUpdater composition weight's
     */
    virtual void updateWeights(const DataSet<TElement>* data,
        const LinearCompositionScorer& lin_scorer) const = 0;
    /**
     * Sets measure, used in DataSetWeightsUpdater. Note that some
     * DataSetWeightsUpdaters don't use measures, so they ignore 
     * the mesure setted
     */
    void setMeasure(typename Measure<TElement>::Ptr in_measure) {
      measure_ = in_measure;
    }
  protected:
    typename Measure<TElement>::Ptr measure_;
  };


  /**
   * Fake DataSetWeightsUpdater, does nothing
   */
  template <class TElement>
  class FakeDataSetWeightsUpdater : public DataSetWeightsUpdater<TElement> {
  public:
    typedef boost::shared_ptr<FakeDataSetWeightsUpdater> Ptr;

    /**
     * @param parameters Standart LTR parameter container with no parameters
     */
    explicit FakeDataSetWeightsUpdater(
        const ParametersContainer& parameters = ParametersContainer())
        : DataSetWeightsUpdater<TElement>("FakeDataSetWeightsUpdater") {
      this->setDefaultParameters();
      this->copyParameters(parameters);
    }

    void updateWeights(const DataSet<TElement>* data,
        const LinearCompositionScorer& lin_scorer) const {
      // doing nothing
    }
  };
};
};

#endif  // LTR_LEARNERS_LINEAR_COMPOSITION_DATA_SET_WEIGHTS_UPDATER_H_