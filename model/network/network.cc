#include "./network.h"
#include <fstream>

void Network::forward(const Matrix& dense_user_input, const Matrix& dense_ad_input, const Matrix& dense_user_ad_input,
    const Matrix3D& sparse_user_input, const Matrix3D& sparse_ad_input, const Matrix3D& sparse_user_ad_input)
{
  if (net_layers.empty())
    return;

  if (dense_layer_user) dense_layer_user->forward(dense_user_input);
  if (dense_layer_ad) dense_layer_ad->forward(dense_ad_input);
  if (dense_layer_user_ad) dense_layer_user_ad->forward(dense_user_ad_input);
  if (sparse_layer_user) sparse_layer_user->forward(sparse_user_input);
  if (sparse_layer_ad) sparse_layer_ad->forward(sparse_ad_input);
  if (sparse_layer_user_ad) sparse_layer_user_ad->forward(sparse_user_ad_input);

  for (int i = 0; i < net_layers.size(); i++) 
  {
    net_layers[i]->gather_inputs();
    net_layers[i]->forward();
  }
}

void Network::backward(const Matrix& dense_user_input, const Matrix& dense_ad_input, const Matrix& dense_user_ad_input,
    const Matrix3D& sparse_user_input, const Matrix3D& sparse_ad_input, const Matrix3D& sparse_user_ad_input,
    const Matrix& target) 
{
  int n_layer = net_layers.size();
  // 0 layer
  if (n_layer <= 0)
    return;
  // >= 1 layer
  loss->evaluate(net_layers[n_layer-1]->output(), target);
  net_layers[n_layer-1]->set_grad_top(&(loss->back_gradient()));

  for (int i = n_layer-1; i >= 0; i--)
  {
    net_layers[i]->backward();
    net_layers[i]->split_grad_bottom();
  }
  if (sparse_layer_user) sparse_layer_user->backward(sparse_user_input);
  if (sparse_layer_ad) sparse_layer_ad->backward(sparse_ad_input);
  if (sparse_layer_user_ad) sparse_layer_user_ad->backward(sparse_user_ad_input);
}

void Network::update() {
  if (sparse_layer_user) sparse_layer_user->update(opt);
  if (sparse_layer_ad) sparse_layer_ad->update(opt);
  if (sparse_layer_user_ad) sparse_layer_user_ad->update(opt);
  for (int i = 0; i < net_layers.size(); i++) {
    net_layers[i]->update(opt);
  }
}

ParamInfo Network::get_fc_parameters() const {
  ParamInfo res;
  const int n_layer = net_layers.size();
  for (int i = 0; i < n_layer; i++) {
    ParamInfo layer_param = net_layers[i]->get_parameters();
    res.insert(layer_param.begin(), layer_param.end());
  }
  return res;
}

ParamInfo Network::get_sparse_parameters(const Matrix3D& sparse_user_input, const Matrix3D& sparse_ad_input, 
    const Matrix3D& sparse_user_ad_input) const {
  ParamInfo res;
  if(sparse_layer_user) 
  {
    ParamInfo input_param = sparse_layer_user->get_parameters(sparse_user_input);
    res.insert(input_param.begin(), input_param.end());
  }
  if(sparse_layer_ad) 
  {
    ParamInfo input_param = sparse_layer_ad->get_parameters(sparse_ad_input);
    res.insert(input_param.begin(), input_param.end());
  }
  if(sparse_layer_user_ad) 
  {
    ParamInfo input_param = sparse_layer_user_ad->get_parameters(sparse_user_ad_input);
    res.insert(input_param.begin(), input_param.end());
  }
  return res;
}

ParamMap Network::get_fc_gradient() {
  ParamMap res;
  const int n_layer = net_layers.size();
  for (int i = 0; i < n_layer; i++) {
    ParamMap layer_gradient = net_layers[i]->get_gradient();
    res.insert(layer_gradient.begin(), layer_gradient.end());
  }
  return res;
}

ParamMap Network::get_sparse_gradient() {
  ParamMap res;
  if(sparse_layer_user) 
  {
    ParamMap input_grad = sparse_layer_user->get_gradient();
    add_parammap(res, input_grad);
  }
  if(sparse_layer_ad) 
  {
    ParamMap input_grad = sparse_layer_ad->get_gradient();
    add_parammap(res, input_grad);
  }
  if(sparse_layer_user_ad) 
  {
    ParamMap input_grad = sparse_layer_user_ad->get_gradient();
    add_parammap(res, input_grad);
  }
  return res;
}

ParamMap Network::get_gradient() {
  ParamMap res;
  ParamMap sparse_gradient = get_sparse_gradient();
  res.insert(sparse_gradient.begin(), sparse_gradient.end());

  ParamMap fc_gradient = get_fc_gradient();
  res.insert(fc_gradient.begin(), fc_gradient.end());

  return res;
}

void  Network::print_gradient() const {
  cout << "print_gradient:Start" << endl;
  //sparse_layer->print_gradient();
  for (int i = 0; i < net_layers.size(); i++) 
  {
    net_layers[i]->print_gradient();
  }
  cout << "print_gradient:End" << endl;
}

void Network::print_layers_out() const {
  cout << "print_layers_out:Start" << endl;
  //cout << "sparse layer output:" << sparse_layer->output();
  for (int i = 0; i < net_layers.size(); i++) 
  {
    cout << "layer " << i << " output:" << net_layers[i]->output() << endl;
  }
  cout << "print_layers_out:End" << endl;
}

void  Network::print_parameters_and_values() const {
  cout << "print_parameters_and_values:Start" << endl;
  //sparse_layer->print_parameters_and_values();
  for (int i = 0; i < net_layers.size(); i++) 
  {
    net_layers[i]->print_parameters_and_values();
  }
  cout << "print_parameters_and_values:End" << endl;
}

ParamMap Network::get_parameters_and_values() {
  ParamMap res;
  ParamMap sparse_param = EmbDict::get()->get_parameters_and_values();
  res.insert(sparse_param.begin(), sparse_param.end());

  const int n_layer = net_layers.size();
  for (int i = 0; i < n_layer; i++) {
      ParamMap layer_param = net_layers[i]->get_parameters_and_values();
    res.insert(layer_param.begin(), layer_param.end());
  }
  return res;
}

void Network::set_fc_parameters_and_values(const ParamMap& param_and_values) {
  const int n_layer = net_layers.size();
  for (int i = 0; i < n_layer; i++) {
    {
        debug << "set_parameters_and_values layer " << i << endl;
        net_layers[i]->set_parameters_and_values(param_and_values);
    }
  }
}

void Network::set_sparse_parameters_and_values(const ParamMap& param_and_values) {
  EmbDict::get()->set_parameters_and_values(param_and_values);
}

void Network::set_parameters_and_values(const ParamMap& param_and_values) {
  set_fc_parameters_and_values(param_and_values);
  set_sparse_parameters_and_values(param_and_values);
}

void Network::save_model(string file_name)
{
    static int count = 0;
    string full_model_file = file_name + "_" + to_string(count);
    ParamMap param = get_parameters_and_values();
    ofstream outfile(full_model_file.c_str());

    cout << "Save model into " <<  full_model_file << endl;
    for (auto iter = param.begin(); iter != param.end(); iter++)
    {
        string value_str = vector2str(iter->second, ",");
        outfile << iter->first << "\t" << value_str << endl;
    }
    outfile.close();

    count += 1;

}

void Network::load_model(string file_name)
{
    ParamMap param;
    ifstream is(file_name.c_str());
    if(!is)
    {
        cerr << "Model File " << file_name << " doesn't exist" << endl;
        exit(1);
    }

    cout << "Load model from " <<  file_name << endl;

    while(!is.eof() && !is.fail())
    {
        string line;
        getline(is, line, '\n');

        //skip if line is empty
        if(line.size() == 0)
            continue;

        vector<string> fields = split(line, "\t");
        // skip if line fields is less than 2
        if (fields.size() < 2)
            continue;

        string key = fields[0];
        Vector value = split_tof_V(fields[1], ",");
        param[key] = value;

    }
    
    cout << "Model param size: " <<  param.size() << endl;

    set_parameters_and_values(param);
    
    cout << "Load Model Done " <<  param.size() << endl;

}

Layer* build_net_layer(string& layer_conf,int& last_output_dim, int layer_index)
{
  Layer* layer = nullptr;
  vector<string> layer_params = split(layer_conf,":");
  string layer_type = layer_params[0];
  cout << "gen layer " << layer_index << ":" << layer_type << endl;
  if (layer_type == "fc")
  {
    if (layer_params.size() != 2)
    {
      cerr << "illeagal fc layer params:\t" << layer_conf << endl;
      exit(1);
    }
    int hidden_num = stoi(layer_params[1]);
    layer = new FullyConnected(layer_index, last_output_dim, hidden_num);
    last_output_dim = hidden_num;
  }
  else if (layer_type == "relu")
  {
    layer = new ReLU(layer_index);
  }
  else if (layer_type == "sigmoid")
  {
    layer = new Sigmoid(layer_index);
  }
  else if (layer_type == "tanh")
  {
    layer = new Tanh(layer_index);
  }
  else if (layer_type == "softmax")
  {
    layer = new Softmax(layer_index);
  }
  else
  {
    cerr << "illeagal layer type:\t" << layer_conf << endl;
    exit(1);

  }
  return layer;
}

int Network::do_build_network_fnn(vector<string> net_layer_confs)
{
  int last_output_dim = 0;
  if (dense_layer_user != nullptr) last_output_dim += dense_layer_user->output_dim();
  if (dense_layer_ad != nullptr) last_output_dim += dense_layer_ad->output_dim();
  if (dense_layer_user_ad != nullptr) last_output_dim += dense_layer_user_ad->output_dim();
  if (sparse_layer_user != nullptr) last_output_dim += sparse_layer_user->output_dim();
  if (sparse_layer_ad != nullptr) last_output_dim += sparse_layer_ad->output_dim();
  if (sparse_layer_user_ad != nullptr) last_output_dim += sparse_layer_user_ad->output_dim();

  Layer* last_layer = nullptr;
  cout << "total layers: " << net_layer_confs.size() << endl;
  for (int i=0; i<net_layer_confs.size(); i++)
  {
      Layer* layer = build_net_layer(net_layer_confs[i], last_output_dim, get_layer_index());
      cout << "add layer " << net_layer_confs[i] << " to the net" << endl;

      cout << "connect layer " << net_layer_confs[i] << " to the net" << endl;
      if (i == 0)
      {
        if (dense_layer_user != nullptr) connect_layer(dense_layer_user, layer);
        if (dense_layer_ad != nullptr) connect_layer(dense_layer_ad, layer);
        if (dense_layer_user_ad != nullptr) connect_layer(dense_layer_user_ad, layer);
        if (sparse_layer_user != nullptr) connect_layer(sparse_layer_user, layer);
        if (sparse_layer_ad != nullptr) connect_layer(sparse_layer_ad, layer);
        if (sparse_layer_user_ad != nullptr) connect_layer(sparse_layer_user_ad, layer);
      }
      else
        connect_layer(last_layer, layer);

      cout << "add layer " << net_layer_confs[i] << " to the net" << endl;
      add_layer(layer);
      cout << "reset last_layer" << endl;
      last_layer = layer;
  }

  return 0;
}


int Network::do_build_network_dssm(vector<string> net_layer_confs)
{
  //去除user_ad交叉特征
  delete dense_layer_user_ad;
  delete sparse_layer_user_ad;
  dense_layer_user_ad = nullptr;
  sparse_layer_user_ad = nullptr;

  int user_last_output_dim = 0;
  if (dense_layer_user != nullptr) user_last_output_dim += dense_layer_user->output_dim();
  if (sparse_layer_user != nullptr) user_last_output_dim += sparse_layer_user->output_dim();

  int ad_last_output_dim = 0;
  if (dense_layer_ad != nullptr) ad_last_output_dim += dense_layer_ad->output_dim();
  if (sparse_layer_ad != nullptr) ad_last_output_dim += sparse_layer_ad->output_dim();

  Layer* user_last_layer;
  Layer* ad_last_layer;

  for (int i=0; i<net_layer_confs.size(); i++)
  {
      Layer* user_layer = build_net_layer(net_layer_confs[i], user_last_output_dim, get_layer_index());
      Layer* ad_layer = build_net_layer(net_layer_confs[i], ad_last_output_dim, get_layer_index());
      if (i == 0)
      {
        if (dense_layer_user != nullptr) connect_layer(dense_layer_user, user_layer);
        if (sparse_layer_user != nullptr) connect_layer(sparse_layer_user, user_layer);
        if (dense_layer_ad != nullptr) connect_layer(dense_layer_ad, ad_layer);
        if (sparse_layer_ad != nullptr) connect_layer(sparse_layer_ad, ad_layer);
      }
      else
      {
        connect_layer(user_last_layer, user_layer);
        connect_layer(ad_last_layer, ad_layer);
      }

      add_layer(user_layer);
      add_layer(ad_layer);

      user_last_layer = user_layer;
      ad_last_layer = ad_layer;
  }

  Layer * dot_layer = new Dot(get_layer_index());
  connect_layer(user_last_layer, dot_layer);
  connect_layer(ad_last_layer, dot_layer);
  add_layer(dot_layer);

  Layer * final_layer = new Sigmoid(get_layer_index());
  connect_layer(dot_layer, final_layer);
  add_layer(final_layer);
  return 0;
}

int Network::build_network(int dense_dim_user, int dense_dim_ad, int dense_dim_user_ad, 
    int sparse_dim_user, int sparse_dim_ad, int sparse_dim_user_ad,
    int emb_dim, vector<string> net_layer_confs, string model)
{
  if (dense_dim_user > 0)
    dense_layer_user = new DenseInput(get_layer_index(), dense_dim_user);
  if (dense_dim_ad > 0)
    dense_layer_ad = new DenseInput(get_layer_index(), dense_dim_ad);
  if (dense_dim_user_ad > 0)
    dense_layer_user_ad = new DenseInput(get_layer_index(), dense_dim_user_ad);
  if (sparse_dim_user > 0)
    sparse_layer_user = new SparseInput(get_layer_index(), sparse_dim_user, emb_dim);
  if (sparse_dim_ad > 0)
    sparse_layer_ad = new SparseInput(get_layer_index(), sparse_dim_ad, emb_dim);
  if (sparse_dim_user_ad > 0)
    sparse_layer_user_ad = new SparseInput(get_layer_index(), sparse_dim_user_ad, emb_dim);

  if (model == "fnn")
    do_build_network_fnn(net_layer_confs);
  else if (model == "dssm")
    do_build_network_dssm(net_layer_confs);
  else
    ;

  cout << "emb_dim:\t" << emb_dim << endl;
  cout << "net_layers:" << endl;
  for (int i=0; i<net_layers.size(); i++)
    cout << "layer_type: " << net_layers[i]->get_layer_type() << " layer_index:" << net_layers[i]->get_layer_index() << endl;
  return 0;
}
