// Copyright 2011 Yandex

#include "ltr/scorers/nearest_neighbor_scorer.h"

#include <boost/lexical_cast.hpp>
#include <logog/logog.h>

#include <algorithm>
#include <string>
#include <sstream>

#include "ltr/data/object.h"

using std::string;
using std::pair;
using std::make_pair;
using std::min;
using std::nth_element;
using std::sort;

using boost::lexical_cast;

using ltr::NNScorer;
using ltr::Object;

namespace ltr {
string NNScorer::toString() const {
  string result = "Nearest neigbor scorer, that processes ";
  result += lexical_cast<string>(number_of_neighbors_to_process_);
  result += " nearest neighbors";
  return result;
}

struct DistanceToNeighbor {
  double distance;
  int neighbor_id;

  bool operator<(const DistanceToNeighbor& arg) const {
    return this->distance < arg.distance;
  }
};

double NNScorer::scoreImpl(const Object& object) const {
  INFO("Starting to score an object with the NNScorer");

  INFO("Starting to find distances to neighbors");
  vector<DistanceToNeighbor> distances_to_neighbors(data_.size());
  for (int neighbor_index = 0;
       neighbor_index < data_.size();
       ++neighbor_index) {
    distances_to_neighbors[neighbor_index].distance =
      metric_->distance(object, data_[neighbor_index]);
    distances_to_neighbors[neighbor_index].neighbor_id = neighbor_index;
  }

  int k_neighbors = min(number_of_neighbors_to_process_,
                        (int)(data_.size()));

  nth_element(distances_to_neighbors.begin(),
              distances_to_neighbors.begin() + k_neighbors,
              distances_to_neighbors.end());

  sort(distances_to_neighbors.begin(),
       distances_to_neighbors.begin() + k_neighbors);

  vector<double> labels(k_neighbors);
  for (int label_index = 0; label_index < k_neighbors; ++label_index) {
    labels[label_index] =
      data_[distances_to_neighbors[label_index].neighbor_id].actual_label();
  }

  vector<double> weights(k_neighbors);
  for (int weight_index = 0; weight_index < k_neighbors; ++weight_index) {
    weights[weight_index] =
      neighbor_weighter_->
        getWeight(distances_to_neighbors[weight_index].distance,
                  weight_index);

    weights[weight_index] *=
      data_.getWeight(distances_to_neighbors[weight_index].neighbor_id);
  }
  return predictions_aggregator_->aggregate(labels, weights);
}

string NNScorer::generateCppCodeImpl(const string& function_name) const {
  string result;
  result += metric_->generateCppCode("distance");
  result += predictions_aggregator_->generateCppCode("aggregator");
  result += neighbor_weighter_->generateCppCode("weighter");

  result += "double " + function_name +
    "(const std::vector<double>& features) {\n";
  result += "  Object object;\n";
  result += "  for (int index = 0; index < features.size(); ++index) {\n";
  result += "  object << features[index];\n";
  result += " }\n";
  result += "  DataSet<Object> data;\n";
  for (int neighbor_index = 0;
       neighbor_index < data_.size();
       ++neighbor_index) {
    string object_name = "object" + boost::lexical_cast<string>(neighbor_index);
    result += "Object " + object_name + ";\n";
    for (int feature_index = 0;
         feature_index < data_[neighbor_index].feature_count();
         ++feature_index) {
      result += object_name + " << " +
        lexical_cast<string>(data_[neighbor_index][feature_index]);
      result += ";\n";
    }
    result += " data.add(" + object_name + ");\n";
  }

  result += "int number_of_neighbors_to_process = " +
    lexical_cast<string>(number_of_neighbors_to_process_) + ";\n";

  result += "vector<std::pair<double, int> >";
  result += " distances_to_neighbors(data.size());\n";
  result += "for (int neighbor_index = 0;\n";
  result += "     neighbor_index < data.size();\n";
  result += "     ++neighbor_index) {\n";
  result += "  distances_to_neighbors[neighbor_index].first =\n";
  result += "    distance(object, data[neighbor_index]);\n";
  result += "  distances_to_neighbors[neighbor_index].second";
  result += " = neighbor_index;\n";
  result += "}\n";

  result += "int k_neighbors = std::min(number_of_neighbors_to_process,\n";
  result += "                      (int)(data.size()));\n";

  result += "nth_element(distances_to_neighbors.begin(),\n";
  result += "            distances_to_neighbors.begin() + k_neighbors,\n";
  result += "            distances_to_neighbors.end());\n";

  result += "sort(distances_to_neighbors.begin(),\n";
  result += "     distances_to_neighbors.begin() + k_neighbors);\n";

  result += "vector<double> labels(k_neighbors);\n";
  result += "for (int label_index = 0; label_index < k_neighbors;";
  result += " ++label_index) {\n";
  result += "  labels[label_index] =\n";
  result += "    data[distances_to_neighbors[label_index].second]";
  result += ".actual_label();\n";
  result += "}\n";

  result += "vector<double> weights(k_neighbors);\n";
  result += "for (int weight_index = 0; weight_index < k_neighbors;";
  result += "++weight_index) {\n";
  result += "  weights[weight_index] =\n";
  result += "    weighter(distances_to_neighbors[weight_index].first,\n";
  result += "             weight_index);\n";
  result += "}\n";
  result += "return aggregator(labels, weights);\n";
  result += "}\n";
  return result;
}
};
