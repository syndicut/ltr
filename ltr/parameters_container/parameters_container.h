// Copyright 2011 Yandex

#ifndef LTR_PARAMETERS_CONTAINER_PARAMETERS_CONTAINER_H_
#define LTR_PARAMETERS_CONTAINER_PARAMETERS_CONTAINER_H_

#pragma warning(disable: 4290)

#include <list>
#include <stdexcept>
#include <string>

#include <boost/any.hpp> //NOLINT
#include <boost/unordered_map.hpp> //NOLINT

#include "ltr/utility/shared_ptr.h"

#include "ltr/interfaces/printable.h"


using std::list;
using std::string;

namespace ltr {
/**
* \brief This class is a storage for parameters of different types.
* Is able to store POD-type parameters and user-defined-type parameters.
* This class is needed for unified initialization in ltr-client.
*
* \sa Parameterized
*/
class ParametersContainer: public Printable {
 public:
  typedef boost::unordered_map<string, boost::any> StringAnyHash;
  typedef ltr::utility::shared_ptr<ParametersContainer> Ptr;

  ParametersContainer();
  ParametersContainer(const ParametersContainer& other);
  ParametersContainer& operator=(const ParametersContainer& other);
  /**
   * Erases all parameters from container
   */
  void Clear();
  /**
   * Gets iterator to the beginning of container
   */
  StringAnyHash::const_iterator begin() const {
    return name_value_hash_.begin();
  }
  /**
   * Gets iterator to the end of container
   */
  StringAnyHash::const_iterator end() const {
    return name_value_hash_.end();
  }
  /**
   * Returns true if the containter is empty,
   * otherwise returns false
   */
  bool empty() const {
    return name_value_hash_.empty();
  }

  template <class T>
  struct NameValue {
    NameValue(const string& parameter_name, const T& parameter_value)
             : name(parameter_name), value(parameter_value) { }
    string name;
    T value;
  };
  /**
   * Gets all parameters of given type
   * @result Returns list of parameters with given type
   */
  template <class T>
  list<NameValue<T> > getValuesByType() const {
    list<NameValue<T> > result;
    for (StringAnyHash::const_iterator iterator = name_value_hash_.begin();
         iterator != name_value_hash_.end();
         ++iterator) {
      T* value = boost::any_cast<T>(&iterator->second);
      if (!value) {
        continue;
      }
      result.push_back(NameValue<T>(iterator->first, *value));
    }
    return result;
  }
  /**
   * Checks the existance of parameter with given name in container
  */
  bool Contains(const string& name) const;
  /**
   * Checks the coincidence of given type and type of parameter with
   * given name
  */
  template <class T>
  bool TypeCoincides(const string& name) const {
    const StringAnyHash::const_iterator iterator = name_value_hash_.find(name);
    if (iterator == name_value_hash_.end()) {
      throw std::logic_error("No such parameter name: " + name);
    }
    return iterator->second.type() == typeid(T);
  }
  /**
   * Returns value of parameter with given name
   * If type of parameter and template type are not
   * the same throws
   */
  template<class T>
  T Get(const string& name) const
      throw(std::logic_error, std::bad_cast) {
    const StringAnyHash::const_iterator iterator = name_value_hash_.find(name);
    if (iterator == name_value_hash_.end()) {
      throw std::logic_error("No such parameter name: " + name);
    }
    try {
      return boost::any_cast<T>(iterator->second);
    } catch(const boost::bad_any_cast &exc) {
      throw std::logic_error(string(exc.what()) +
                             "\nParameter name: " + name +
                             "\nRequested type: " + typeid(T).name() +
                             "\nActual type: " +
                             iterator->second.type().name());
    }
  }
  /**
   * Returns value of parameter with given name
   * and casts it to DesiredType.
   */
  template<class StoredType, class DesiredType>
  DesiredType Get(const string& name) const
      throw(std::logic_error, std::bad_cast) {
    const StringAnyHash::const_iterator iterator = name_value_hash_.find(name);
    if (iterator == name_value_hash_.end())
      throw std::logic_error("No such parameter name: " + name);

    try {
      const StoredType &value = boost::any_cast<StoredType>(iterator->second);
      DesiredType desired_type_value = dynamic_cast<DesiredType>(value); //NOLINT
      return desired_type_value;
    } catch(const boost::bad_any_cast &exc) {
      throw std::logic_error(string(exc.what()) +
                             "\nParameter name: " + name +
                             "\nRequested type: " +
                             typeid(StoredType).name() +
                             "\nactual type: " +
                             iterator->second.type().name());
    } catch(const std::bad_cast &exc) {
      throw std::logic_error(string(exc.what()) +
                             "\nParameter name: " + name);
    }
  }
  /**
   * Returns const reference to value of parameter with given name
   */
  template<class T>
  const T& GetRef(const string& name) const
      throw(std::logic_error, std::bad_cast) {
    const StringAnyHash::const_iterator iterator = name_value_hash_.find(name);
    if (iterator == name_value_hash_.end()) {
      throw std::logic_error("No such parameter name: " + name);
    }

    try {
      return *boost::any_cast<T>(&iterator->second);
    }
    catch(const boost::bad_any_cast &exc) {
      throw std::logic_error(string(exc.what()) +
                             "\nParameter name: " + name +
                             "\nRequested type: " + typeid(T).name());
    }
  }
  /**
   * Returns reference to value of parameter with given name
   */
  template<class T>
  T& GetRef(const string &name)
      throw(std::logic_error, std::bad_cast) {
    const StringAnyHash::iterator iterator = name_value_hash_.find(name);
    if (iterator == name_value_hash_.end()) {
      throw std::logic_error("No such parameter name: " + name);
    }

    try {
      return *boost::any_cast<T>(&iterator->second);
    }
    catch(const boost::bad_any_cast &exc) {
      throw std::logic_error(string(exc.what()) +
                             "\nParameter name: " + name +
                             "\nRequested type: " + typeid(T).name());
    }
  }


  /**
   * Sets given value for parameter with given name.
   * Adds new parameter if there is no such parameter name.
   * @param name - name of parameter to set/add
   * @param value - value to set/add
   */
  template<class T>
  void Set(const string &name, const T &value) {
    name_value_hash_[name] = value;
  }

  /**
   * Sets given value for parameter with given name.
   * Throws if there is no such parameter name.
   * @param name - name of parameter to set
   * @param value - value to set
   */
  template<class T>
  void SetExisting(const string& name, const T& value) {
    const StringAnyHash::iterator iterator = name_value_hash_.find(name);
    if (iterator == name_value_hash_.end()) {
      throw std::logic_error("No such parameter name: " + name);
    }
    iterator->second = value;
  }

  /**
   * Adds new parameter with given name and value.
   * Throws if container already has such parameter name.
   * @param name - name of parameter to add
   * @param value - value to add
   */
  template<class T>
  void AddNew(const string& name, const T& value) {
    const StringAnyHash::const_iterator iterator = name_value_hash_.find(name);
    if (iterator != name_value_hash_.end()) {
      throw std::logic_error("There is already exist such parameter: " + name);
    }
    name_value_hash_[name] = value;
  }
  /**
   * Copies parameters from other container.
   * Throws if other container contains new parameters
   * or there is conflict of types.
   */
  void Copy(const ParametersContainer& parameters);
  /**
   * Print names and values of all parameters
   */
  virtual string toString() const;

 private:
  /**
   * Map from parameter name to parameter value
   */
  StringAnyHash name_value_hash_;
};
}


#endif  // LTR_PARAMETERS_CONTAINER_PARAMETERS_CONTAINER_H_
