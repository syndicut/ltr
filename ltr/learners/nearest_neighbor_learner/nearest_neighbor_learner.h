// Copyright 2012 Yandex

#ifndef LTR_LEARNERS_NEAREST_NEIGHBOR_LEARNER_NEAREST_NEIGHBOR_LEARNER_H_
#define LTR_LEARNERS_NEAREST_NEIGHBOR_LEARNER_NEAREST_NEIGHBOR_LEARNER_H_

#include <logog/logog.h>

#include <string>
#include <vector>

#include "ltr/predictions_aggregators/predictions_aggregator.h"
#include "ltr/learners/learner.h"
#include "ltr/metrics/metric.h"
#include "ltr/parameters_container/parameters_container.h"
#include "ltr/scorers/nearest_neighbor_scorer.h"
#include "ltr/utility/shared_ptr.h"
#include "ltr/utility/neighbor_weighter.h"

using ltr::PredictionsAggregator;
using ltr::BaseMetric;
using ltr::NNScorer;
using ltr::ParametersContainer;
using ltr::utility::NeighborWeighter;

namespace ltr {
/**
 \brief NNLearner
 Implements nearest neighbor approach.
 \tparam TElement object container of those the dataset consists (it should
 be Object).
 */
template<class TElement>
class NNLearner : public BaseLearner<TElement, NNScorer> {
 public:
  typedef ltr::utility::shared_ptr<NNLearner> Ptr;

  NNLearner() {
    // DO NOTHING
  }

  explicit NNLearner(const ParametersContainer& parameters) {
    metric_ = parameters.Get<BaseMetric::Ptr>("metric");
    neighbor_weighter_ =
      parameters.Get<NeighborWeighter::Ptr>("neighbor_weighter");
    predictions_aggregator_
      = parameters.Get<PredictionsAggregator::Ptr>("predictions_aggregator");
    number_of_neighbors_to_process_ =
      parameters.Get<int>("number_of_neighbors_to_process");
  }

  NNLearner(BaseMetric::Ptr metric,
            NeighborWeighter::Ptr neighbor_weighter,
            PredictionsAggregator::Ptr predictions_aggregator,
            int number_of_neighbors_to_process) :
    metric_(metric),
    neighbor_weighter_(neighbor_weighter),
    predictions_aggregator_(predictions_aggregator),
    number_of_neighbors_to_process_(number_of_neighbors_to_process) {
  }

  GET_SET(typename BaseMetric::Ptr, metric);
  GET_SET(NeighborWeighter::Ptr, neighbor_weighter);
  GET_SET(PredictionsAggregator::Ptr, predictions_aggregator);
  GET_SET(int, number_of_neighbors_to_process);

 private:
  void learnImpl(const DataSet<TElement>& data, NNScorer* scorer);
  virtual string getDefaultAlias() const {return "NearestNeightborLearner";}

  BaseMetric::Ptr metric_;
  NeighborWeighter::Ptr neighbor_weighter_;
  PredictionsAggregator::Ptr predictions_aggregator_;
  int number_of_neighbors_to_process_;
};

/** The implementation of nearest neighbor approach.
  * \param data DataSet that will be stored in the scorer.
  */
template<class TElement>
void NNLearner<TElement>::learnImpl(const DataSet<TElement>& data,
                                    NNScorer* scorer) {
  INFO("Learning started");
  DataSet<Object> object_data;

  for (int element_index = 0;
       element_index < data.size();
       ++element_index) {
    PerObjectAccessor<const TElement>
      per_object_accessor(&data[element_index]);
    for (int object_index = 0;
         object_index < per_object_accessor.object_count();
         ++object_index) {
       object_data.add(per_object_accessor.object(object_index));
    }
  }

  *scorer =  NNScorer(metric_,
                      object_data,
                      neighbor_weighter_,
                      predictions_aggregator_,
                      number_of_neighbors_to_process_);
}
};

#endif  // LTR_LEARNERS_NEAREST_NEIGHBOR_LEARNER_NEAREST_NEIGHBOR_LEARNER_H_
