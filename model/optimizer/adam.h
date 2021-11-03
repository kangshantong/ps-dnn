#ifndef SRC_OPTIMIZER_ADAM_H_
#define SRC_OPTIMIZER_ADAM_H_

#include <unordered_map>
#include "optimizer.h"

class ADAM : public Optimizer {
 private:
  float beta1;
  float beta2;
  float eps;
  std::unordered_map<const float*, Vector> v_map;  // velocity
  std::unordered_map<const float*, Vector> s_map; 
  std::unordered_map<const float*, int> counts; 

 public:
  explicit ADAM(float lr = 0.01, float decay = 0.0, float beta1 = 0.9, float beta2=0.999,
               float eps = 1e-8) : Optimizer(lr, decay),
               beta1(beta1), beta2(beta2),eps(eps) {}

  void update(Vector::AlignedMapType& w, Vector::ConstAlignedMapType& dw);
};

#endif  // SRC_OPTIMIZER_ADAM_H_
