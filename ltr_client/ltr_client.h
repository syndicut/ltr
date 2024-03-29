// Copyright 2012 Yandex

#ifndef LTR_CLIENT_LTR_CLIENT_H_
#define LTR_CLIENT_LTR_CLIENT_H_

#include <string>

#include "ltr/interfaces/parameterized.h"

#include "ltr_client/configurator.h"

#include "ltr_client/utility/parameterized_info.h"

using std::string;

using ltr::Parameterized;

class LtrClient {
 public:
  LtrClient();

  ~LtrClient();

  void initFrom(const string& file_name);

  void launch();

 private:
  template <class TElement>
  void launchTrain(boost::any parameterized,
                   const TrainLaunchInfo& train_info);

  template <class TElement>
  void launchCrossvalidation(
    const CrossvalidationLaunchInfo& crossvalidation_info);

  ParametrizedInfosList getLoadQueue() const;

  ConfigParser configurator_;
};

#endif  // LTR_CLIENT_LTR_CLIENT_H_
