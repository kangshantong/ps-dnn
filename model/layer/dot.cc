#include "./dot.h"
void Dot::gather_inputs() {
  if (input_layers.size() != 2)
  {
    cerr << "Error:Dot need 2 input layers!!!" << endl;
    exit(1);
  }
  left_bottom = &(input_layers[0]->output());
  right_bottom = &(input_layers[1]->output());
}

void Dot::forward() {
  // y = a.b
  debug << "Dot::forward Start" << endl;
  debug << "layer_index:" << layer_index << endl;
  int batch_size = (*left_bottom).cols();
  top.resize(1,batch_size);
  for (int i=0; i<batch_size; i++)
  {
    Vector left_vec = (*left_bottom).col(i);
    Vector right_vec = (*right_bottom).col(i);
    float product = left_vec.dot(right_vec);
    top(0,i) = product;
  }
  debug << "left bottom:" << *left_bottom << endl;
  debug << "right bottom:" << *right_bottom << endl;
  debug << "top:" << top << endl;
  debug << "Dot::forward End" << endl;
}

void Dot::backward() {
  // d(L)/d(a_i) = d(L)/d(z_i) * d(b_i)
  // d(L)/d(b_i) = d(L)/d(z_i) * d(a_i)
  debug << "Dot::backward Start" << endl;
  debug << "layer_index:" << layer_index << endl;
  int batch_size = (*left_bottom).cols();
  int vec_dim = (*left_bottom).rows();

  left_grad_bottom.resize(vec_dim, batch_size);
  right_grad_bottom.resize(vec_dim, batch_size);

  for (int i=0; i<batch_size; i++)
  {
    left_grad_bottom.col(i) = (*right_bottom).col(i) * (*grad_top)(0, i);
    right_grad_bottom.col(i) = (*left_bottom).col(i) * (*grad_top)(0, i);
  }
  debug << "grad_top:" << *grad_top << endl;
  debug << "left_grad_bottom:" << left_grad_bottom << endl;
  debug << "right_grad_bottom:" << right_grad_bottom << endl;
  debug << "Dot::backward End" << endl;
}

void Dot::split_grad_bottom() {
  debug << "split_grad_bottom:Start" << endl;
  debug << "input_layers.size():" << input_layers.size() << endl;
  debug << "grad_bottom.size():" << grad_bottom.rows() << "*" << grad_bottom.cols() << endl;
  debug << "grad_bottom:" << grad_bottom << endl;
  input_layers[0]->set_grad_top(&left_grad_bottom);
  input_layers[1]->set_grad_top(&right_grad_bottom);
  debug << "split_grad_bottom:END" << endl;
}
