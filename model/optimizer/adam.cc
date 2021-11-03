#include "./adam.h"

void ADAM::update(Vector::AlignedMapType& w,
                 Vector::ConstAlignedMapType& dw) {
  // refer:https://github.com/pytorch/pytorch/blob/master/torch/optim/adam.py
  // If v/s is zero, initialize it
  Vector& v = v_map[w.data()];
  if (v.size() == 0) {
    v.resize(w.size());
    v.setZero();
  }
  Vector& s = s_map[w.data()];
  if (s.size() == 0) {
    s.resize(w.size());
    s.setZero();
  }
  
  if (counts.find(w.data()) == counts.end())
    counts[w.data()] = 1;

  int count = counts[w.data()];
  Vector l_grad = dw + decay * w;

  v = beta1 * v + (1 - beta1) * l_grad;
  s = beta2 * s + (1 - beta2) * (l_grad.cwiseProduct(l_grad));

  float bias_correction1 = 1 - pow(beta1, count);
  float bias_correction2 = 1 - pow(beta2, count);
  float step_size = lr * sqrt(bias_correction2) / bias_correction1;

  w -= step_size * (v.cwiseQuotient(s.cwiseSqrt() + Vector::Constant(w.size(), eps)));

  counts[w.data()] += 1;
}
