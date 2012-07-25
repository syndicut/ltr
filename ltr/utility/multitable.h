#include <string>
#include <vector>
#include <exception>
#include <iostream>
#include "boost/multi_array.hpp"
#include "ltr/interfaces/printable.h"

using std::logic_error;
using std::string;
using std::stringstream;
using std::vector;
using std::cout;
using ltr::Printable;

namespace ltr{
namespace utility {
struct DimensionSize {
  int index;
  size_t size;
};

bool operator<(const DimensionSize& left, const DimensionSize& right) {
  return left.size < right.size;
}

template <typename T, size_t N>
class MultiTable : public Printable {
 public:
  class Iterator;
  MultiTable();
  MultiTable(const vector<size_t>& table_size);
  const T& operator[] (const vector<size_t>& multiIndex) const;
  T& operator[] (const vector<size_t>& multiIndex);
  void setAxisLabel(const size_t axis_index, const string& label);
  void setTickLabel(const size_t axis_index, const size_t tick_index, const string& label);
  virtual string toString() const;
  void resize(const vector<size_t>& multi_size);
  void clear();
  Iterator begin();
  Iterator end();
 private:
  struct DimensionMetaInfo;
  boost::multi_array<T, N> table_contents_;
  vector<DimensionMetaInfo> table_meta_info_;
  void printMultiTableInner(const vector<DimensionSize>& multi_size,
                            vector<size_t>* multi_index,
                            const int depth,
                            stringstream* const outstream) const ;
  void print2DTable(const size_t first_axis_index,
                    const size_t second_axis_index,
                    vector<size_t>* const multi_index,
                    stringstream* const out_stream) const;
};

template<typename T, size_t N>
class MultiTable<T, N>::Iterator {
 public:
  Iterator(MultiTable<T, N>* _instance);
  const T& operator*() const;
  T& operator*();
  T* operator->();
  Iterator operator++();
  const vector<size_t>& getMultiIndex() const;
  bool operator==(const Iterator& right);
  bool operator!=(const Iterator& right);
 private:
  vector<size_t> multi_index_;
  MultiTable<T, N>* instance;

  friend class MultiTable;
};

template <typename T, size_t N>
struct MultiTable<T, N>::DimensionMetaInfo {
  string axis_label;
  vector<string> axis_ticks_labels;
};

template <typename T, size_t N>
void MultiTable<T, N>::resize(const vector<size_t>& multi_size) {
  table_contents_.resize(multi_size);
  table_meta_info_.resize(multi_size.size());
  for (size_t index = 0; index < multi_size.size(); ++index) {
    table_meta_info_[index].axis_ticks_labels.resize(multi_size[index]);
  }
}

template <typename T, size_t N>
void MultiTable<T, N>::clear() {
  vector<size_t> empty;
  for (size_t i = 0; i < N; ++i) {
    empty.push_back(0);
  }
  this->resize(empty);
}

template<typename T, size_t N>
void MultiTable<T, N>::print2DTable(const size_t first_axis_index,
                                    const size_t second_axis_index,
                                    vector<size_t>* const multi_index,
                                    stringstream* const out_stream) const {
  size_t first_size = table_contents_.shape()[first_axis_index];
  size_t second_size = table_contents_.shape()[second_axis_index];

  *out_stream << "\n";
  for (int axis_index = 0; axis_index < multi_index->size(); ++axis_index) {
    size_t tick_index = (*multi_index)[axis_index];
    if (axis_index != first_axis_index && axis_index != second_axis_index) {
      *out_stream << table_meta_info_[axis_index].axis_label << ":"
                  << table_meta_info_[axis_index].
                     axis_ticks_labels[tick_index]
                  << "\t";
    }
  }
  *out_stream << "\n";
  *out_stream << "Table ROW: "
       << table_meta_info_[first_axis_index].axis_label
       << " COLUMN: "
       << table_meta_info_[second_axis_index].axis_label
       << "\n";
  *out_stream << "\t";
  for (int second_index = 0; second_index < second_size; ++second_index) {
    *out_stream
        << table_meta_info_
           [second_axis_index].axis_ticks_labels[second_index]
         << "\t";
  }
  *out_stream << "\n";
  for (size_t first_index = 0; first_index < first_size; ++first_index) {
    (*multi_index)[first_axis_index] = first_index;
    *out_stream << table_meta_info_
                   [first_axis_index].axis_ticks_labels[first_index]
                << "\t";
    for (size_t second_index = 0; second_index < second_size; ++second_index) {
      (*multi_index)[second_axis_index] = second_index;
      *out_stream << table_contents_(*multi_index) << "\t";
    }
    *out_stream << "\n";
  }
}

template <typename T, size_t N>
void MultiTable<T, N>::printMultiTableInner(
    const vector<DimensionSize>& axis_print_order,
    vector<size_t>* multi_index,
    const int depth,
    stringstream* const out_stream) const {
  if (depth + 2 == axis_print_order.size()) {
    print2DTable(axis_print_order[depth].index,
                 axis_print_order[depth + 1].index,
                 multi_index,
                 out_stream);
  } else {
    int index_number_to_increase = axis_print_order[depth].index;
    int ticks_count = axis_print_order[depth].size;
    for (int tick_index = 0; tick_index < ticks_count; ++tick_index) {
      (*multi_index)[index_number_to_increase] = tick_index;
      printMultiTableInner(axis_print_order,
                           multi_index, depth + 1, out_stream);
    }
  }
}

template<typename T, size_t N>
MultiTable<T, N>::MultiTable(const vector<size_t>& table_size)
  : table_contents_()
  , table_meta_info_() {
  if (table_size.size() != N) {
    throw logic_error("Bad number of input sizes in multitable constructor\n");
  }
  table_contents_.resize(table_size);
  table_meta_info_.resize(table_size.size());
  for (size_t index = 0; index < table_size.size(); ++index) {
    table_meta_info_[index].axis_ticks_labels.resize(table_size[index]);
  }
}

template<typename T, size_t N>
MultiTable<T, N>::MultiTable()
  : table_contents_()
  , table_meta_info_() {}

template<typename T, size_t N>
const T& MultiTable<T, N>::operator[] (const vector<size_t>& multi_index) const {
  if (multi_index.size() != N) {
    throw logic_error("Bad number of input sizes in multitable [] operator\n");
  }
  return table_contents_(multi_index);
}

template<typename T, size_t N>
T& MultiTable<T, N>::operator[] (const vector<size_t>& multi_index) {
  if (multi_index.size() != N) {
    throw logic_error("Bad number of input sizes in multitable [] operator\n");
  }
  return table_contents_(multi_index);
}

template<typename T, size_t N>
void MultiTable<T, N>::setAxisLabel(const size_t dim_index, const string& label) {
  table_meta_info_[dim_index].axis_label = label;
}

template<typename T, size_t N>
void MultiTable<T, N>::setTickLabel(
    const size_t dim_index, const size_t axis_index, const string& label) {
  table_meta_info_[dim_index].axis_ticks_labels[axis_index] = label;
}

template<typename T, size_t N>
string MultiTable<T, N>::toString() const {
  const int dimension_count = table_contents_.num_dimensions();
  vector<DimensionSize> multi_size;
  const size_t* dims_size = table_contents_.shape();
  for (int i = 0; i < dimension_count; ++i) {
    DimensionSize to_push = {i, dims_size[i]};
    multi_size.push_back(to_push);
  }
  sort(multi_size.begin(), multi_size.end());
  vector<size_t> multi_index(multi_size.size(), 0);
  stringstream out_stream;
  printMultiTableInner(multi_size, &multi_index, 0, &out_stream);
  return out_stream.str();
}

template<typename T, size_t N>
typename MultiTable<T, N>::Iterator MultiTable<T, N>::begin() {
  return Iterator(this);
}

template<typename T, size_t N>
typename MultiTable<T, N>::Iterator MultiTable<T, N>::end() {
  Iterator end_iter(this);
  for (size_t i = 0; i < N; ++i) {
    end_iter.multi_index_[i] = table_contents_.shape()[i];
  }
  return end_iter;
}

template<typename T, size_t N>
MultiTable<T, N>::Iterator::Iterator(MultiTable<T, N>* _instance)
  : multi_index_(vector<size_t>(N))
  , instance(_instance) {}

template<typename T, size_t N>
const T& MultiTable<T, N>::Iterator::operator*() const {
  return instance->table_contents_(multi_index_);
}

template<typename T, size_t N>
T& MultiTable<T, N>::Iterator::operator*() {
  return instance->table_contents_(multi_index_);
}

template<typename T, size_t N>
T* MultiTable<T, N>::Iterator::operator->() {
  return &(instance->table_contents_(multi_index_));
}

template<typename T, size_t N>
typename MultiTable<T, N>::Iterator MultiTable<T, N>::Iterator::operator++() {
  vector<size_t> multi_size(N);
  for (int i = 0; i < N; ++i) {
    multi_size[i] = instance->table_contents_.shape()[i];
  }
  size_t index_number = 0;
  do {
    ++multi_index_[index_number];
    if (multi_index_[index_number] != multi_size[index_number]) {
      break;
    }
    multi_index_[index_number] = 0;
    ++index_number;
    if (index_number == multi_size.size()) {
      multi_index_ = multi_size;
      break;
    }
  } while(true);
  return *this;
}

template<typename T, size_t N>
bool MultiTable<T, N>::Iterator::operator==(const Iterator& right) {
  return (this->multi_index_ == right.multi_index_);
}

template<typename T, size_t N>
bool MultiTable<T, N>::Iterator::operator!=(const Iterator& right) {
    return !(*this == right);
}

template<typename T, size_t N>
const vector<size_t>& MultiTable<T, N>::Iterator::getMultiIndex() const {
  return this->multi_index_;
}

}
}