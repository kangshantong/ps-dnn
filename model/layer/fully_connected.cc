#include "./fully_connected.h"

void FullyConnected::init() {
  weight.resize(dim_in, dim_out);
  bias.resize(dim_out);
  grad_weight.resize(dim_in, dim_out);
  grad_bias.resize(dim_out);
  set_normal_random(weight.data(), weight.size(), 0, 0.05);
  set_normal_random(bias.data(), bias.size(), 0, 0.05);

}

void FullyConnected::forward() {
  // z = w' * x + b
  const int n_sample = bottom->cols();
  top.resize(dim_out, n_sample);
  debug << "FullyConnected::forward Start" << endl;
  debug << "layer_index:" << layer_index << endl;
  debug << "weight.size:\t" << weight.cols() << "\t" << weight.rows() << endl;
  debug << "bottom.size:\t" << bottom->cols() << "\t" << bottom->rows() << endl;
  top = weight.transpose() * (*bottom);
  top.colwise() += bias;
  debug << "FC bottom:\t" << *bottom << endl;
  debug << "FC weight:\t" << weight << endl;
  debug << "FC bias:\t" << bias << endl;
  debug << "FC top:\t" << top << endl;
  debug << "FullyConnected::forward End" << endl;
}

void FullyConnected::backward() {
  debug << "FC::backward Start" << endl;
  debug << "layer_index:" << layer_index << endl;
  const int n_sample = bottom->cols();
  // d(L)/d(w') = d(L)/d(z) * x'
  // d(L)/d(b) = \sum{ d(L)/d(z_i) }
  grad_weight = (*bottom) * (grad_top->transpose());
  grad_bias = grad_top->rowwise().sum();
  // d(L)/d(x) = w * d(L)/d(z)
  grad_bottom.resize(dim_in, n_sample);
  grad_bottom = weight * (*grad_top);
  matrix_clip(grad_bottom);
  debug << "bottom:\t" << *bottom << endl;
  debug << "grad_weight:\t" << grad_weight << endl;
  debug << "grad_bias:\t" << grad_bias << endl;
  debug << "grad_top:\t" << *grad_top << endl;
  debug << "weight:\t" << weight << endl;
  debug << "grad_bottom:\t" << grad_bottom << endl;
  debug << "FC::backward End" << endl;
}

void FullyConnected::update(Optimizer* opt) {
  debug << "FC::update Start" << endl;
  debug << "layer_index:" << layer_index << endl;
  Vector::AlignedMapType weight_vec(weight.data(), weight.size());
  Vector::AlignedMapType bias_vec(bias.data(), bias.size());
  Vector::ConstAlignedMapType grad_weight_vec(grad_weight.data(),
                                              grad_weight.size());
  Vector::ConstAlignedMapType grad_bias_vec(grad_bias.data(), grad_bias.size());

  debug << "before update:" << endl;
  debug << "weight:" << weight << endl;
  debug << "grad_weight:" << grad_weight << endl;
  debug << "bias:" << bias << endl;
  debug << "grad_bias:" << grad_bias << endl;
  opt->update(weight_vec, grad_weight_vec);
  opt->update(bias_vec, grad_bias_vec);
  debug << "after update:" << endl;
  debug << "weight:" << weight << endl;
  debug << "bias:" << bias << endl;
  debug << "FC::update END" << endl;
}

ParamInfo FullyConnected::get_parameters() {
  ParamInfo res;
  for(int i=0;i<weight.rows();i++)
  {
     string w_param = gen_fc_param("w",i); 
     res[w_param] = dim_out;
  }

  string b_param = gen_fc_param("b");
  res[b_param] = dim_out;
  
  debug << "FullyConnected::get_parameters layer " << layer_index << " param size:" << res.size() << " dim:" << dim_out << endl; 
  return res;
}

ParamMap FullyConnected::get_gradient() {
  ParamMap res;
  for(int i=0;i<grad_weight.rows();i++)
  {
      string param = gen_fc_param("w",i); 
      res[param] = grad_weight.row(i);
  }

  string param = gen_fc_param("b");
  res[param] = grad_bias;

  return res;
}

void FullyConnected::print_gradient() {
  cout << "layer_index:" << layer_index << endl;
  cout << "layer_type:" << layer_type << endl;
  cout << "grad_weight:\t" << grad_weight << endl;
  cout << "grad_bias:\t" << grad_bias << endl;
  cout << endl;
}

ParamMap FullyConnected::get_parameters_and_values() {
  ParamMap res;
  for(int i=0;i<weight.rows();i++)
  {
      string param = gen_fc_param("w",i); 
      res[param] = weight.row(i);
  }

  string param = gen_fc_param("b");
  res[param] = bias;

  return res;
}

void FullyConnected::set_parameters_and_values(const ParamMap& param_and_values) {
  debug << "Load w param for layer " << layer_index << endl;
  for(int i=0;i<weight.rows();i++)
  {
      string param = gen_fc_param("w",i); 
      for (int j=0; j<weight.cols(); j++)
        weight(i,j) = param_and_values.at(param)[j];
  }
  debug << "weight: " << weight << endl;

  debug << "Load b param for layer " << layer_index << endl;
  string param = gen_fc_param("b");
  for (int j=0; j<bias.cols(); j++)
    bias(j) = param_and_values.at(param)[j];
  debug << "bias: " << bias << endl;
}

void FullyConnected::print_parameters_and_values() {
  cout << "layer_index:" << layer_index << endl;
  cout << "layer_type:" << layer_type << endl;
  cout << "weight:\t" << weight << endl;
  cout << "bias:\t" << bias << endl;
  cout << endl;
}

string FullyConnected::gen_fc_param(string mode, int index)
{
  if (mode == w_param)
  {
    string param = prefix + delimiter + to_string(layer_index) + delimiter +
      w_param + delimiter + to_string(index);
    return param;
  }
  else
  {
    string param = prefix + delimiter + to_string(layer_index) + delimiter + b_param;
    return param;
  }
}
