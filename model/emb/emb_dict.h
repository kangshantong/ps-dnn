/*
 * EmbDict.h
 */

#ifndef SRC_LAYER_EMBDICT_H_
#define SRC_LAYER_EMBDICT_H_

#include <vector>
#include<stdlib.h>
#include <map>
#include <unordered_map>
#include "../utils/utils.h"
#include "../optimizer/optimizer.h"

class EmbDict {
 private:
  int dim_sparse_emb;

  ParamMap emb_dict;

  EmbDict() {dim_sparse_emb=0;}

  ~EmbDict() {}

 public:
  static EmbDict* get() {
    static EmbDict e; return &e;
  }
  void start(int sparse_emb_dim);

  ParamMap get_parameters_and_values();
  void set_parameters_and_values(const ParamMap& param);
  void print_parameters_and_values();

  Vector get_emb(string param);
  void update_emb(const string param, const Vector& grad, Optimizer* opt);

};

#endif  // SRC_LAYER_EMBDICT_H_
