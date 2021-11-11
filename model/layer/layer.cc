#include "layer.h"

void Layer::gather_inputs() {
  debug << "gather_inputs:Start" << endl;
  debug << "layer_type: " << layer_type << endl;
  debug << "layer_index: " << layer_index << endl;
  debug << "input_layers.size(): " << input_layers.size() << endl;
  if (input_layers.size() == 1)
  {
    bottom = &(input_layers[0]->output());
  }
  else
  {
    int input_dims = 0;
    int batch_size = input_layers[0]->output_batch_size();
    for (int i=0; i<input_layers.size(); i++)
    {
      input_dims += input_layers[i]->output_dim();
    }
    if (merge_bottom.rows() != input_dims || merge_bottom.cols() != batch_size)
      merge_bottom.resize(input_dims, batch_size);

    int start_rows = 0;
    for (int i=0; i<input_layers.size(); i++)
    {
      debug << "merge_bottom:" << merge_bottom << endl;
      debug << "input_layers " << i << " output():" << input_layers[i]->output() << endl;
      int input_dim = input_layers[i]->output_dim();
      debug << "input_dim:" << input_dim << " batch_size:" << input_layers[i]->output_batch_size() << endl;
      debug << "start_rows:" << start_rows << endl;
      for (int j=0; j<input_dim; j++)
        merge_bottom.row(start_rows+j) = (input_layers[i]->output()).row(j);
      start_rows += input_dim;
    }
    bottom = &merge_bottom;
    debug << "merge_bottom:" << merge_bottom << endl;
  }
  debug << "gather_inputs:End" << endl;
}

void Layer::split_grad_bottom() {
  debug << "split_grad_bottom:Start" << endl;
  debug << "input_layers.size():" << input_layers.size() << endl;
  debug << "grad_bottom.size():" << grad_bottom.rows() << "*" << grad_bottom.cols() << endl;
  debug << "grad_bottom:" << grad_bottom << endl;
  int n_layer = input_layers.size();
  if (input_layers.size() == 1)
    input_layers[0]->set_grad_top(&grad_bottom);
  else
  {
    if (sub_grad_bottom.size() <= 0)
      sub_grad_bottom.resize(n_layer);
    int n_sample = grad_bottom.cols();
    int start_row = 0;
    for (int i=0; i<input_layers.size(); i++)
    {
      int input_dim = input_layers[i]->output_dim();
      Matrix& sub_grad = sub_grad_bottom[i];
      if (sub_grad.rows() != input_dim || sub_grad.cols() != n_sample)
        sub_grad.resize(input_dim, n_sample);
      for (int j=0; j<input_dim; j++)
        sub_grad.row(j)= grad_bottom.row(start_row+j);
      input_layers[i]->set_grad_top(&sub_grad_bottom[i]);
      start_row += input_dim;
    }
    for (int i=0; i<input_layers.size(); i++)
    {
      debug << "input_layer: " << i << " grad_top:" <<  *(input_layers[i]->input_gradient()) << endl;
    }
  }
  debug << "split_grad_bottom:END" << endl;
}
