// Copyright 2012 Yandex

#include <gtest/gtest.h>

#include <map>
#include <string>

#include "ltr/data/object.h"
#include "ltr/data/feature_info.h"
#include "ltr/utility/indices.h"
#include "ltr/utility/numerical.h"

#include "ltr/feature_converters/feature_converter.h"
#include "ltr/feature_converters/fake_feature_converter.h"
#include "ltr/feature_converters/feature_sampler.h"
#include "ltr/feature_converters/per_feature_linear_converter.h"
#include "ltr/feature_converters/nan_to_zero_converter.h"
#include "ltr/feature_converters/nominal_to_bool_converter.h"
#include "ltr/feature_converters/remove_nominal_converter.h"

using ltr::Object;
using ltr::FeatureInfo;
using ltr::NOMINAL;
using ltr::NUMERIC;
using ltr::BOOLEAN;
using ltr::utility::DoubleEqual;
using ltr::utility::Indices;

using ltr::FeatureConverter;
using ltr::FakeFeatureConverter;
using ltr::FeatureSampler;
using ltr::PerFeatureLinearConverter;
using ltr::NanToZeroConverter;
using ltr::NominalToBoolConverter;
using ltr::RemoveNominalConverter;

const int features_count = 11;

class FeatureConvertersTest : public ::testing::Test {
 public:
   FeatureConvertersTest() {}
};

TEST_F(FeatureConvertersTest, FakeFeatureConverterTest) {
  Object object, converted_object;
  for (int feature_index = 0; feature_index < features_count; ++feature_index) {
    object << feature_index;
  }

  FakeFeatureConverter::Ptr fake_feature_converter
    (new FakeFeatureConverter(object.feature_info()));

  fake_feature_converter->apply(object, &converted_object);

  EXPECT_EQ(object.features(), converted_object.features());
}

TEST_F(FeatureConvertersTest, FeatureSamplerTest) {
  Object object, converted_object;
  for (int feature_index = 0; feature_index < features_count; ++feature_index) {
    object << feature_index;
  }

  Indices indices;
  indices.push_back(3);
  indices.push_back(7);
  indices.push_back(4);

  FeatureSampler::Ptr converter(new FeatureSampler(indices));
  converter->set_input_feature_info(object.feature_info());

  EXPECT_EQ(indices, converter->indices());

  converter->apply(object, &converted_object);

  EXPECT_EQ(indices.size(), converted_object.feature_count());
  for (int index = 0; index < indices.size(); ++index) {
    EXPECT_EQ(indices[index], converted_object.features()[index]);
  }

  indices.push_back(1);
  converter->set_indices(indices);
  converter->apply(object, &converted_object);

  EXPECT_EQ(indices.size(), converted_object.feature_count());
  for (int index = 0; index < indices.size(); ++index) {
    EXPECT_EQ(indices[index], converted_object.features()[index]);
  }

  indices.push_back(103);
  EXPECT_ANY_THROW(converter->set_indices(indices));
}

TEST_F(FeatureConvertersTest, PerFeatureLinearConverterTest) {
  Object object, converted_object;
  for (int feature_index = 0; feature_index < features_count; ++feature_index) {
    object << feature_index;
  }

  PerFeatureLinearConverter::Ptr per_feature_converter
    (new PerFeatureLinearConverter(object.feature_count()));

  per_feature_converter->set_input_feature_info(object.feature_info());

  per_feature_converter->set_shift(0, 1.0);
  per_feature_converter->set_shift(1, 0.0);
  per_feature_converter->set_shift(2, -1.0);
  per_feature_converter->set_shift(3, 0.0);

  per_feature_converter->set_factor(0, 1.0);
  per_feature_converter->set_factor(1, -0.5);
  per_feature_converter->set_factor(2, 2.0);
  per_feature_converter->set_factor(3, 0.0);
  
  per_feature_converter->apply(object, &converted_object);

  EXPECT_TRUE(DoubleEqual(converted_object.features()[0], 1.0));
  EXPECT_TRUE(DoubleEqual(converted_object.features()[1], -0.5));
  EXPECT_TRUE(DoubleEqual(converted_object.features()[2], 3.0));
  EXPECT_TRUE(DoubleEqual(converted_object.features()[3], 0.0));
}

TEST_F(FeatureConvertersTest, NanToZeroConverterTest) {
  Object nan_features_object, zero_features_object, converted_object;
  for (int feature_index = 0; feature_index < features_count; ++feature_index) {
    nan_features_object << numeric_limits<double>::quiet_NaN();
    zero_features_object << 0;
  }

  NanToZeroConverter::Ptr nan_to_zero_converter
    (new NanToZeroConverter(nan_features_object.feature_info()));

  nan_to_zero_converter->apply(nan_features_object, &converted_object);

  EXPECT_EQ(converted_object.features(), zero_features_object.features());
}

TEST_F(FeatureConvertersTest, NominalToBoolConverterTest) {
  FeatureInfo feature_info;
  map <size_t, std::string> nominal_feature_values1, nominal_feature_values2;
  nominal_feature_values1[1] = "red";
  nominal_feature_values2[2] = "green";
  feature_info.addFeature(NOMINAL,
                          ltr::NominalFeatureValues(nominal_feature_values1));
  feature_info.addFeature(NOMINAL,
                          ltr::NominalFeatureValues(nominal_feature_values2));

  NominalToBoolConverter::Ptr nominal_to_bool_converter
    (new NominalToBoolConverter(feature_info));

  Object object(feature_info), converted_object;
  object.features()[0] = 1;
  object.features()[1] = 1;

  nominal_to_bool_converter->apply(object, &converted_object);
  EXPECT_EQ(converted_object.features().size(), 2);
  EXPECT_EQ(converted_object.feature_info().getFeatureType(0), BOOLEAN);
  EXPECT_TRUE(converted_object.features()[0]);
  EXPECT_EQ(converted_object.feature_info().getFeatureType(1), BOOLEAN);
  EXPECT_FALSE(converted_object.features()[1]);

  feature_info.addFeature(NUMERIC);
  feature_info.addFeature(NOMINAL);
  object = Object(feature_info);
  object.features()[0] = 1;
  object.features()[1] = 1;
  object.features()[2] = 10;
  object.features()[3] = 1;

  NominalToBoolConverter::Ptr nominal_to_bool_converter2
    (new NominalToBoolConverter(feature_info));

  nominal_to_bool_converter2->apply(object, &converted_object);

  EXPECT_EQ(converted_object.features().size(), 4);
  EXPECT_EQ(converted_object.feature_info().getFeatureType(0), NUMERIC);
  EXPECT_EQ(converted_object.features()[0], 10);
  EXPECT_EQ(converted_object.feature_info().getFeatureType(1), BOOLEAN);
  EXPECT_TRUE(converted_object.features()[1]);
  EXPECT_EQ(converted_object.feature_info().getFeatureType(2), BOOLEAN);
  EXPECT_FALSE(converted_object.features()[2]);  
  EXPECT_EQ(converted_object.feature_info().getFeatureType(3), BOOLEAN);
}

TEST_F(FeatureConvertersTest, RemoveNominalConverterTest) {
  FeatureInfo feature_info;
  feature_info.addFeature(NOMINAL);
  
  RemoveNominalConverter::Ptr remove_nominal_converter
    (new RemoveNominalConverter(feature_info));

  Object converted_object;
  remove_nominal_converter->apply(Object(feature_info), &converted_object);

  EXPECT_TRUE(converted_object.features().empty());

  feature_info.addFeature(NUMERIC);
  feature_info.addFeature(NOMINAL);
  Object object(feature_info);
  object.features()[0] = 1;
  object.features()[1] = 2;
  object.features()[2] = 3;

  RemoveNominalConverter::Ptr remove_nominal_converter2
    (new RemoveNominalConverter(feature_info));

  remove_nominal_converter2->apply(object, &converted_object);

  EXPECT_EQ(converted_object.features().size(), 1);
  EXPECT_EQ(converted_object.features()[0], 2);
}
