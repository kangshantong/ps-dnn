#include "./sigmoid.h"

void Sigmoid::forward() {
  // a = 1 / (1 + exp(-z))
  debug << "Sigmoid::forward Start" << endl;
  debug << "layer_index:" << layer_index << endl;
  top.array() = 1.0 / (1.0 + (-(*bottom)).array().exp());
  debug << "bottom:" << *bottom << endl;
  debug << "top:" << top << endl;
  debug << "Sigmoid::forward End" << endl;
}

void Sigmoid::backward() {
  // d(L)/d(z_i) = d(L)/d(a_i) * d(a_i)/d(z_i)
  // d(a_i)/d(z_i) = a_i * (1-a_i)
  debug << "Sigmoid::backward Start" << endl;
  debug << "layer_index:" << layer_index << endl;
  Matrix da_dz = top.array().cwiseProduct(1.0 - top.array());
  grad_bottom = (*grad_top).cwiseProduct(da_dz);
  debug << "top:" << top << endl;
  debug << "da_dz:" << da_dz << endl;
  debug << "grad_top:" << *grad_top << endl;
  debug << "grad_bottom:" << grad_bottom << endl;
  debug << "Sigmoid::backward End" << endl;
}
