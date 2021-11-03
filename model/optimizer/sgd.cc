#include "./sgd.h"

void SGD::update(Vector::AlignedMapType& w,
                 Vector::ConstAlignedMapType& dw) {
  // refer to SGD in PyTorch:
  // https://github.com/pytorch/pytorch/blob/master/torch/optim/sgd.py
  // If v is zero, initialize it
  Vector& v = v_map[w.data()];
  if (v.size() == 0) {
    v.resize(w.size());
    v.setZero();
  }

  Vector l_grad = dw + decay * w;
  debug << "w:" << w << " dw:" << dw << " decay:" << decay << " l_grad:" << l_grad << endl;
  // update v
  v = momentum * v + l_grad;
  debug << "momentum:" << momentum << " v:" << v << endl;
  // update w
  if (nesterov)
    w -= lr * (momentum * v + l_grad);
  else
    w -= lr * v;
  debug << "nesterov:" << nesterov << " lr:" << lr << " new w:" << w << endl;
}
