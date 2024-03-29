// Copyright 2011 Yandex

#ifndef LTR_DATA_UTILITY_IO_UTILITY_H_
#define LTR_DATA_UTILITY_IO_UTILITY_H_

#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "ltr/data/data_set.h"
#include "ltr/data/utility/parsers/parser.h"
#include "ltr/scorers/scorer.h"
#include "ltr/utility/numerical.h"

using std::ifstream;
using std::map;
using std::ofstream;
using std::string;
using std::vector;

using ltr::utility::DOUBLE_PRECISION;

namespace ltr {
namespace io_utility {
/**
 * Function to load data set from file.
 * @param filename - path to file
 * @param format - data format (svmlite, yandex, arff)
 * @code
 * DataSet<Object> = loadDataSet<Object>("dataset.txt", "svmlite");
 * @endcode
 */
template<class TElement>
DataSet<TElement> loadDataSet(const string& filename,
                              const string& format);
/**
 * Function to save data set into file.
 * @param data - data set to save
 * @param filename - path to file
 * @param format - data format (svmlite, yandex, arff) 
 * @code
 * saveDataSet(data, "dataset.txt", "svmlite");
 * @endcode
 */
template<class TElement>
void saveDataSet(const DataSet<TElement>& data,
                 const string& filename,
                 const string& format);
/**
 * Function to build data set of given objects
 * @param parser - parser, which will build data set
 * @param objects - objects to build data set from
 * @param info - information about features in objects
 * @code
 * buildDataSet<TElement>(parser, objects, handler->featureInfo());
 * @endcode
 */
template<class TElement>
DataSet<TElement> buildDataSet(Parser::Ptr parser,
                               const vector<Object>& objects,
                               const FeatureInfo& info);
/**
 * Function to save predicted labeles for given data set
 * @param data - data set for prediction
 * @param scorer - scorer for prediction
 * @param filename - path to file
 * @code
 * savePredictions(data, scorer, "predictions.txt");
 * @endcode
 */
template<class TElement>
void savePredictions(const DataSet<TElement>& data,
                     Scorer::Ptr scorer,
                     const string& filename);

/**
 * Function to save predicted labeles for given data set
 * @param data - data set with marked predicted labeles
 * @param filename - path to file
 * @code
 * savePredictions(data, "predictions.txt");
 * @endcode
 */
template<class TElement>
void savePredictions(const DataSet<TElement>& data,
                     const string& filename);
/**
 * Function groups objects in lists, each having same
 * meta-parameter
 * @param objects - objects to group
 * @param group_parameter - meta-parameter name
 * @param result - result lists
 */
void groupByMeta(const vector<Object>& objects,
                 const string& group_parameter,
                 map<string, vector<Object> >* result);
/**
 * Function groups objects in lists, each having same
 * integer meta-parameter
 * @param objects - objects to group
 * @param group_parameter - meta-parameter name
 * @param result - result lists
 */
void groupByIntMeta(const vector<Object>& objects,
                    const string& group_parameter,
                    map<int, vector<Object> >* result);
/**
 * Function groups objects in lists, each having same
 * float meta-parameter
 * @param objects - objects to group
 * @param group_parameter - meta-parameter name
 * @param result - result lists
 */
void groupByFloatMeta(const vector<Object>& objects,
                      const string& group_parameter,
                      map<float, vector<Object> >* result);
};
};

// Template realization
namespace ltr {
namespace io_utility {

template<class TElement>
DataSet<TElement> loadDataSet(const string& filename,
                              const string& format) {
  ifstream file(filename.c_str());
  if (file.fail()) {
    throw std::logic_error("File " + filename + " not found");
  }
  Parser::Ptr parser = getParser(format);
  parser->startParsing(&file);

  vector<Object> objects;
  Object current_object;

  while (parser->parseNextObject(&current_object)) {
    objects.push_back(current_object);
  }
  file.close();
  int feature_count = parser->featureInfo().feature_count();
  for (int object_index = 0;
       object_index < (int)objects.size();
       ++object_index) {
    for (int count = objects[object_index].features().size();
         count < feature_count;
         ++count) {
      objects[object_index] << ltr::utility::NaN;
    }
  }
  DataSet<TElement> result =
    buildDataSet<TElement>(parser, objects, parser->featureInfo());
  result.set_alias(filename);
  return result;
}


template<class TElement>
void saveDataSet(const DataSet<TElement>& data,
                 const string& filename,
                 const string& format) {
  ofstream file(filename.c_str());
  if (file.fail()) {
    throw std::logic_error("can't open " + filename + " for writing");
  }
  Parser::Ptr parser = getParser(format);
  for (int element_index = 0;
       element_index < (int)data.size();
       ++element_index) {
    PerObjectAccessor<const TElement> per_object_accessor(&data[element_index]);
    for (int object_index = 0;
         object_index < per_object_accessor.object_count();
         ++object_index) {
      string str;
      parser->makeString(per_object_accessor.object(object_index), &str);
      file << str << std::endl;
    }
  }
  file.close();
}

template<class TElement>
void savePredictions(const DataSet<TElement>& data,
                     Scorer::Ptr scorer,
                     const string& filename) {
  ofstream file(filename.c_str());

  if (file.fail()) {
    throw std::logic_error("can't open " + filename + " for writing");
  }

  file.precision(DOUBLE_PRECISION);
  for (int element_index = 0;
       element_index < (int)data.size();
       ++element_index) {
    PerObjectAccessor<const TElement> per_object_accessor(&data[element_index]);
    for (int object_index = 0;
         object_index < per_object_accessor.object_count();
         ++object_index) {
      file << (*scorer)(per_object_accessor.object(object_index)) << std::endl;
    }
  }
  file.close();
}

template<class TElement>
void savePredictions(const DataSet<TElement>& data,
                     const string& filename) {
  ofstream file(filename.c_str());

  if (file.fail()) {
    throw std::logic_error("can't open " + filename + " for writing");
  }

  file.precision(utility::DOUBLE_PRECISION);
  for (int element_index = 0;
       element_index < (int)data.size();
       ++element_index) {
    PerObjectAccessor<const TElement> per_object_accessor(&data[element_index]);
    for (int object_index = 0;
         object_index < per_object_accessor.object_count();
         ++object_index) {
      double label = per_object_accessor.object(object_index).predicted_label();
      file << label << std::endl;
    }
  }
  file.close();
}
};
};
#endif  // LTR_DATA_UTILITY_IO_UTILITY_H_
