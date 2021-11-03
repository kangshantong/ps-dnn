#include "./cross_entropy_loss.h"

void CrossEntropy::evaluate(const Matrix& pred, const Matrix& target) {
  debug << "CrossEntropy::evaluate Start" << endl;
  debug << "pred:\t" << pred << endl;
  debug << "target:\t" << target << endl;

  debug << "pred shape:\t" << pred.rows() << "\t" << pred.cols() << endl;
  debug << "target shape:\t" << target.rows() << "\t" << target.cols() << endl;

  int m = pred.rows();
  int n = pred.cols();
  Matrix ones = Matrix::Constant(m, n, 1);

  const float eps = 1e-8;
  // forward: L = \sum{ -y_i*log(p_i) - (1-y_i)log(1-p_i)} / n
  loss = - (target.array().cwiseProduct((pred.array() + eps).log()) + 
      (ones-target).array().cwiseProduct(((ones-pred).array() + eps).log())).sum();
  loss /= n;
  // backward: d(L)/d(p_i) = ((1-y_i)/(1-p_i)-y_i/p_i)/n
  grad_bottom = ((ones-target).array().cwiseQuotient((ones-pred).array() + eps) - 
      target.array().cwiseQuotient(pred.array() + eps)) / n;
  matrix_clip(grad_bottom);
  debug << "grad_bottom:\t" << grad_bottom << endl;
  debug << "CrossEntropy::evaluate End" << endl;
}
