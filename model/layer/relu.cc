#include "./relu.h"

void ReLU::forward() {
  // a = z*(z>0)
  debug << "ReLU::forward Start" << endl;
  debug << "layer_index:" << layer_index << endl;
  top = (*bottom).cwiseMax(0.0);
  debug << "bottom:" << *bottom << endl;
  debug << "top:" << top << endl;
  debug << "ReLU::forward End" << endl;
}

void ReLU::backward() {
  // d(L)/d(z_i) = d(L)/d(a_i) * d(a_i)/d(z_i)
  //             = d(L)/d(a_i) * 1*(z_i>0)
  debug << "ReLU::backward Start" << endl;
  debug << "layer_index:" << layer_index << endl;
  Matrix positive = ((*bottom).array() > 0.0).cast<float>();
  grad_bottom = (*grad_top).cwiseProduct(positive);
  debug << "grad_top:" << *grad_top << endl;
  debug << "grad_bottom:" << grad_bottom << endl;
  debug << "ReLU::backward End" << endl;
}
