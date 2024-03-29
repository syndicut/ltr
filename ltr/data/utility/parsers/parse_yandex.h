// Copyright 2012 Yandex

#ifndef LTR_DATA_UTILITY_PARSERS_PARSE_YANDEX_H_
#define LTR_DATA_UTILITY_PARSERS_PARSE_YANDEX_H_

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "boost/spirit/include/classic_core.hpp"

#include "ltr/data/object.h"
#include "ltr/data/object_pair.h"
#include "ltr/data/object_list.h"

#include "ltr/data/utility/parsers/parser.h"

namespace ltr {
namespace io_utility {
class YandexParser : public Parser {
 private:
  void init(std::istream* in);

  static const int raw_query_id_idx_;

  static const int raw_relevance_idx_;

 public:
  void parseRawObject(string line, RawObject* result);

  void makeString(const Object& obj, std::string* result);

  PairwiseDataSet buildPairwiseDataSet(
    const std::vector<Object>& objects, const FeatureInfo& info);

  ListwiseDataSet buildListwiseDataSet(
    const std::vector<Object>& objects, const FeatureInfo& info);
};
};
};

#endif  // LTR_DATA_UTILITY_PARSERS_PARSE_YANDEX_H_
