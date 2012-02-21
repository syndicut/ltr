// Copyright 2011 Yandex

#ifndef LTR_INTERFACES_PARAMETERIZED_H_
#define LTR_INTERFACES_PARAMETERIZED_H_

#include <string>
#include <vector>

#include "ltr/parameters_container/parameters_container.h"

using std::string;
using std::vector;

#define CHECK_PARAMETER(type, name, condition) \
  { \
    type X = this->get_##type_##parameter(name); \
    if (!(condition)) \
      throw std::logic_error("Error in parameter '" \
        name "' check: " #condition); \
  }

#define CHECK_INT_PARAMETER(name, condition) \
  { \
    int X = this->getIntParameter(name); \
    if (!(condition)) \
      throw std::logic_error("Error in parameter '" \
        name "' check: " #condition); \
  }
#define CHECK_DOUBLE_PARAMETER(name, condition) \
  { \
    double X = this->getDoubleParameter(name); \
    if (!(condition)) \
      throw std::logic_error("Error in parameter '" \
        name "' check: " #condition); \
  }
#define CHECK_BOOL_PARAMETER(name, condition) \
  { \
    bool X = this->getBoolParameter(name); \
    if (!(condition)) \
      throw std::logic_error("Error in parameter '" \
        name "' check: " #condition); \
  }
#define CHECK_LIST_PARAMETER(name, condition) \
  { \
    vector<int> X = this->getListParameter(name); \
    if (!(condition)) \
      throw std::logic_error("Error in parameter '" \
        name "' check: " #condition); \
  }

#define CHECK_HAS_PARAMETER(type, name) \
  if (!parameters_.has<type>(name)) \
    throw std::logic_error("Class must has " #type " parameter");

#define CHECK_HAS_NO_PARAMETER(type, name) \
  if (parameters_.has<type>(name)) \
    throw std::logic_error("Class mustn't has " #type " parameter");

namespace ltr {
/**
 * Parameterized is a class that provides derived subclasses hold their
 * different-type parameters (int, double and bool) in convenient
 * way. E.g. a derived subclass knows it's default parameters
 */
class Parameterized {
  protected:
  /** Container, which contain all the parameters of this object.
   */
  ParametersContainer parameters_;
  Parameterized() {}

  public:
  /**
   * By default simply clears all parameters
   */
  virtual void setDefaultParameters();
  /**
   * checks, if current parameters are valid.
   * If invalid parameters found - logic_error is thrown.
   */
  virtual void checkParameters() const;
  /** Sets integer parameter and calls checkParameters()
   */
  void setIntParameter(const string& name, int value) {
    CHECK_HAS_PARAMETER(int, name);
    parameters_.setInt(name, value);
    checkParameters();
  }
  /** Sets double parameter and calls checkParameters()
   */
  void setDoubleParameter(const string& name, double value) {
    CHECK_HAS_PARAMETER(double, name);
    parameters_.setDouble(name, value);
    checkParameters();
  }
  /** Sets bool parameter and calls checkParameters()
   */
  void setBoolParameter(const string& name, bool value) {
    CHECK_HAS_PARAMETER(bool, name);
    parameters_.setBool(name, value);
    checkParameters();
  }
  /** Sets list(array of integers) parameter and calls checkParameters()
   */
  void setListParameter(const string& name, vector<int> value) {
    CHECK_HAS_PARAMETER(vector<int>, name);
    parameters_.setList(name, value);
    checkParameters();
  }

  /** Sets integer parameter, but don't calls checkParameters()
   */
  void addIntParameter(const string& name, int value) {
    CHECK_HAS_NO_PARAMETER(int, name);
    parameters_.setInt(name, value);
  }
  /** Sets double parameter, but don't calls checkParameters()
   */
  void addDoubleParameter(const string& name, double value) {
    CHECK_HAS_NO_PARAMETER(double, name);
    parameters_.setDouble(name, value);
  }
  /** Sets bool parameter, but don't calls checkParameters()
   */
  void addBoolParameter(const string& name, bool value) {
    CHECK_HAS_NO_PARAMETER(bool, name);
    parameters_.setBool(name, value);
  }
  /** Sets list(array of integers) parameter, but don't calls checkParameters()
   */
  void addListParameter(const string& name, vector<int> value) {
    CHECK_HAS_NO_PARAMETER(vector<int>, name);
    parameters_.setList(name, value);
  }

  /** Copies parameters from another parameter container
   * @param params - container to copy parameters from.
   */
  void copyParameters(const ParametersContainer& params) {
    parameters_.copyParameters(params);
    checkParameters();
  }
  /**
   * Removes all the parameters.
   */
  void clearParameters() {
    parameters_.clear();
  }

  /** Returns the value of parameter with name 'name'
   */
  template<class T>
  T getParameter(const string& name) const {
    return parameters_.get<T>(name);
  }
  /** Returns the value of integer parameter with name 'name'
   */
  int getIntParameter(const string& name) const {
    return parameters_.getInt(name);
  }
  /** Returns the value of double parameter with name 'name'
   */
  double getDoubleParameter(const string& name) const {
    return parameters_.getDouble(name);
  }
  /** Returns the value of bool parameter with name 'name'
   */
  bool getBoolParameter(const string& name) const {
    return parameters_.getBool(name);
  }
  /** Returns the value of list parameter with name 'name'
   */
  vector<int> getListParameter(const string& name) const {
    return parameters_.getList(name);
  }
  virtual ~Parameterized() {}
};
};

#endif  // LTR_INTERFACES_PARAMETERIZED_H_
