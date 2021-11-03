#include "./tanh.h"

void Tanh::forward() {
  // a = 2 / (1 + exp(-2*z)) - 1
  debug << "Tanh::forward Start" << endl;
  debug << "layer_index:" << layer_index << endl;
  top.array() = 2.0 / (1.0 + (-2*(*bottom)).array().exp()) - 1.0;
  debug << "bottom:" << *bottom << endl;
  debug << "top:" << top << endl;
  debug << "Tanh::forward End" << endl;
}

void Tanh::backward() {
  // d(L)/d(z_i) = d(L)/d(a_i) * d(a_i)/d(z_i)
  // d(a_i)/d(z_i) = (a_i + 1) * (1 - a_i)
  debug << "Tanh::backward Start" << endl;
  debug << "layer_index:" << layer_index << endl;
  Matrix da_dz = (top.array() + 1.0).cwiseProduct(1.0 - top.array());
  grad_bottom = (*grad_top).cwiseProduct(da_dz);
  debug << "top:" << top << endl;
  debug << "da_dz:" << da_dz << endl;
  debug << "grad_top:" << *grad_top << endl;
  debug << "grad_bottom:" << grad_bottom << endl;
  debug << "Tanh::backward End" << endl;
}
