// Copyright 2012 Yandex

#ifndef LTR_DATA_OBJECT_H_
#define LTR_DATA_OBJECT_H_

#include <map>
#include <ostream> //NOLINT
#include <stdexcept>
#include <string>
#include <vector>

#include "ltr/interfaces/printable.h"

#include "ltr/utility/shared_ptr.h"

using std::map;
using std::string;
using std::vector;

namespace ltr {
/** \typedef
 * Type for feature vector.
 */
typedef vector<double> Features;
/** \typedef
 * Type for pointer to feature vector.
 */
typedef ltr::utility::shared_ptr<Features> FeaturesPtr;
/** \typedef
 * Type for meta information.
 * Just map from info field name to info field value
 */
typedef map<string, string> MetaInfo;
/** \class Base class for storing information in a Dataset. An object consist of
 * feature vector and meta information;
 */
class Object : public Printable {
 public:
  /** \typedef Shared pointer to an object.
   */
  typedef shared_ptr<Object> Ptr;
  /** Default constructor, creates an object with empty feature vector.
   */
  Object();
  /** Default constructor, creates an deep copy of an object.
   */
  Object(const Object& object);
  /** Returns constant link to the feature vector of an object.
   */
  const Features& features() const;
  /** Returns link to the feature vector of an object.
   */
  Features& features();
  /** Gets a piece of meta information by name.
   */
  const string& getMetaInfo(const string& name) const;
  /** Sets a meta information with given name.
   */
  void setMetaInfo(const string& name, const string& value);
  /** Append a feature to the feature vector of the object.
   */
  Object& operator<<(double feature_value);

  // \TODO(sameg) Don't like the next 3 operators

  /** Returns constant link to feature at index i.
   */
  const double& operator[](int feature_index) const;
  /** Returns link to feature at index i.
   */
  double& operator[](int feature_index);
  /** Returns constant link to feature at index i.
   */
  const double& at(int feature_index) const;
  /** Returns link to feature at index i.
   */
  double& at(int feature_index);
  /** Easy weighted operator=. Makes the object to use the feature vector and
   * meta information of the other object. If it is needed, the resources of
   * the object (feature vector and meta information) are destroyed.
   */
  Object& operator=(const Object& other);
  /** Clear object features and FeaturesInfo
  */
  void clear();
  /** Returns the number of features in the object.
   */
  int feature_count() const;
  /** Returns actual (that means, the label was read from input file or any
   * other way is known) scoring label of the object.
   */
  double actual_label() const;
  /** Returns predicted (that means, the label set by a scoring algorithm)
   * label of the object.
   */
  double predicted_label() const;
  /** Sets actual (that means, the label was read from input file or any
   * other way is known) scoring label of the object.
   */
  void set_actual_label(double label);
  /** Sets predicted (that means, the label set by a scoring algorithm)
   * label of the object.
   */
  void set_predicted_label(double label) const;
  /** Creates a deep copy of the object, this copies the object resources and
   * creates a new object with these copied resources. The new object is
   * returned.
   */
  Object deepCopy() const;
  /** Friend operator, checks whether two objects are equal.
   */
  friend bool operator==(const Object& lhs, const Object& rhs);
  /**
   * Function for serialization of object.
   * Returns string of feature values in brackets. For example: [1, 5.6, 2.3]
   */
  virtual string toString() const;

 private:
  /** Shared pointer to feature vector.
   */
  FeaturesPtr features_;
  /** Shared pointer to meta information container.
   */
  ltr::utility::shared_ptr<MetaInfo> meta_info_;
  /** Actual label.
   */
  double actual_label_;
  /** Predicted label. Changing those isn't considered as changing the object,
   * thought it's mutable.
   */
  mutable double predicted_label_;

  template<class TElement>
  friend class DataSet;
};
/** Operator, checks whether two objects are equal.
 */
bool operator==(const Object& lhs, const Object& rhs);
/** Operator, checks whether two objects are not equal.
 */
bool operator!=(const Object& lhs, const Object& rhs);
};
#endif  // LTR_DATA_OBJECT_H_
