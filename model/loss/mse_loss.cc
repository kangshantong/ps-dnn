#include "./mse_loss.h"

void MSE::evaluate(const Matrix& pred, const Matrix& target) {
  //std::cout << "MSE::evaluate Start" << std::endl;
  int n = pred.cols();
  // forward: L = sum{ (p-y).*(p-y) } / n
  Matrix diff = pred - target;
  loss = diff.cwiseProduct(diff).sum();
  loss /= n;
  // backward: d(L)/d(p) = (p-y)*2/n
  grad_bottom = diff * 2 / n;
  matrix_clip(grad_bottom);
  //std::cout << "pred:\t" << pred << std::endl;
  //std::cout << "target:\t" << target << std::endl;
  //std::cout << "grad_bottom:\t" << grad_bottom << std::endl;
  //std::cout << "MSE::evaluate End"<< std::endl;
}
