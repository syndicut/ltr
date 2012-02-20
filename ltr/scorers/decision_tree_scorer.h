// Copyright 2011 Yandex

#ifndef LTR_SCORERS_DECISION_TREE_SCORER_H_
#define LTR_SCORERS_DECISION_TREE_SCORER_H_

#include <string>
#include "ltr/scorers/scorer.h"
#include "ltr/decision_tree/decision_tree.h"

using std::string;
using ltr::decision_tree::Vertex;
using ltr::decision_tree::DecisionTree;

namespace ltr {
class DecisionTreeScorer : public Scorer {
  private:
    DecisionTree<double> tree_;

    double scoreImpl(const Object& obj) const {
      return tree_.value(obj);
    }

    string generateCppCodeImpl(const string& function_name) const {
      return tree_.generateCppCode(function_name);
    }
  public:
    typedef boost::shared_ptr< DecisionTreeScorer > Ptr;

    DecisionTreeScorer() : Scorer("Decision tree scorer") {}

    void setTree(const DecisionTree<double>& tree) {
      tree_ = tree;
    }

    void setTreeRoot(Vertex<double>::Ptr root) {
      tree_.setRoot(root);
    }

    string brief() const {
      return "Decision of the tree";
    }
};
}

#endif  // LTR_SCORERS_DECISION_TREE_SCORER_H_
