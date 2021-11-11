#include "./emb_dict.h"

void EmbDict::start(int sparse_emb_dim) {
  dim_sparse_emb = sparse_emb_dim;
  // The 0-th embedding is always all zero
  Vector zero_emb = Vector::Zero(dim_sparse_emb);
  emb_dict["0"] = zero_emb;
}

ParamMap EmbDict::get_parameters_and_values() 
{
  return emb_dict;
}

void EmbDict::set_parameters_and_values(const ParamMap& param) {
  emb_dict.clear();
  debug << "set emb_dict : start" << endl;
  for (auto iter=param.begin();iter!=param.end(); iter++)
  {
    string param = iter -> first;
    string prefix = extract_field(param, "_", 0);
    if (prefix != "l")
    {
      debug << iter -> first << ":"  << iter->second << endl;
      emb_dict[iter -> first] = iter->second;
    }
  }
  debug << "set emb_dict : end" << endl;
}

void EmbDict::print_parameters_and_values() {

  cout << "emb dict:" << endl;
  for (auto iter=emb_dict.begin();iter!=emb_dict.end(); iter++)
  {
    cout << iter -> first << ":" << iter -> second << endl;
  }
  cout << endl;
}

Vector EmbDict::get_emb(string param)
{
  if (emb_dict.find(param) == emb_dict.end())
  {
    char *dmlc_role = getenv("DMLC_ROLE");
    if (dmlc_role != nullptr)
    {
      cerr << "DMLC_ROLE is " << dmlc_role << endl; 
      cerr << "All emb should be inited in ps server" << endl;
      exit(1);
    }
    debug << "Gen emb for " << param << endl;
    Vector emb(dim_sparse_emb); 
    set_normal_random(emb.data(), emb.size(), 0, 0.05);
    emb_dict[param] = emb;
  }

  debug << param << "-emb:" << emb_dict[param] << endl;
  return emb_dict[param];
}

void EmbDict::update_emb(const string param, const Vector& grad, Optimizer* opt)
{
  //Never update 0-th embedding
  if (param == "0")
      return;
  debug << "before update: " << emb_dict[param] << endl;
  debug << "grad: " << grad << endl;
  Vector::AlignedMapType emb_vec(emb_dict[param].data(), emb_dict[param].size());
  Vector::ConstAlignedMapType grad_emb_vec(grad.data(), grad.size());
  opt->update(emb_vec, grad_emb_vec);
  debug << "after update: " << emb_dict[param] << endl;
}
