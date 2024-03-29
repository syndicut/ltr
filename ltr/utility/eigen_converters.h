// Copyright 2012 Yandex
#ifndef LTR_UTILITY_EIGEN_CONVERTERS_H_
#define LTR_UTILITY_EIGEN_CONVERTERS_H_

#include <Eigen/Dense>

#include <vector>

#include "ltr/data/object.h";

using std::vector;

using Eigen::VectorXd;
using Eigen::MatrixXd;

namespace ltr {
namespace utility {
/**
 * Function converts std::vector to Eigen::VectorXd
 */
VectorXd StdVectorToEigenVector(const vector<double>& std_vector);

/**
 * Function inits every element of Eigen::MatrixXd equal to zero
 */
void InitEigenMatrix(MatrixXd* eigen_matrix);

/**
 * Function inits every element of Eigen::VectorXd equal to zero
 */
void InitEigenVector(VectorXd* eigen_vector);
};
};

#endif  // LTR_UTILITY_EIGEN_CONVERTERS_H_
