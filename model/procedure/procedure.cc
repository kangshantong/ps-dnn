#include "procedure.h"

void Procedure::init()
{
  // Feas Conf
  FeaIni.load(fea_conf_file);
  int dim_dense_user = FeaIni["dense_features_user"].size();
  int dim_dense_ad = FeaIni["dense_features_ad"].size();
  int dim_dense_user_ad = FeaIni["dense_features_user_ad"].size();
  int dim_sparse_user = FeaIni["sparse_features_user"].size();
  int dim_sparse_ad = FeaIni["sparse_features_ad"].size();
  int dim_sparse_user_ad = FeaIni["sparse_features_user_ad"].size();

  cout << "dim_dense_user:\t" << dim_dense_user << endl;
  cout << "dim_dense_ad:\t" << dim_dense_ad << endl;
  cout << "dim_dense_user_ad:\t" << dim_dense_user_ad << endl;
  cout << "dim_sparse_user:\t" << dim_sparse_user << endl;
  cout << "dim_sparse_ad:\t" << dim_sparse_ad << endl;
  cout << "dim_sparse_user_ad:\t" << dim_sparse_user_ad << endl;


  //data
  datasets.set_fea_dim(dim_dense_user,dim_dense_ad,dim_dense_user_ad,dim_sparse_user,dim_sparse_ad,dim_sparse_user_ad);
  datasets.set_train_file(train_file);
  datasets.set_test_file(test_file);

  // dnn
  DnnIni.load(dnn_conf_file);
  int emb_dim = stoi(DnnIni["sparse_dict"]["emb_dim"].as<string>()); 
  EmbDict::get()->start(emb_dim);
  vector<string> net_layers = split(DnnIni["net"]["layers"].as<string>(),",");
  string model = DnnIni["net"]["model"].as<string>();
  dnn.build_network(dim_dense_user,dim_dense_ad,dim_dense_user_ad,dim_sparse_user,dim_sparse_ad,dim_sparse_user_ad,emb_dim,net_layers,model);

  // loss
  string loss_conf = DnnIni["loss"]["loss"].as<string>();
  cout << "loss_conf:\t" << loss_conf << endl;
  Loss* loss = gen_loss(loss_conf);
  dnn.add_loss(loss);

  // optimizer;
  string opt_conf = DnnIni["optimizer"]["optimizer"].as<string>();
  cout << "opt_conf:\t" << opt_conf << endl;
  float learning_rate = stof(DnnIni["optimizer"]["learning_rate"].as<string>());
  Optimizer* opt = gen_opt(learning_rate, opt_conf);
  dnn.add_opt(opt);

  // debug
  debug_interval = stoi(DnnIni["debug"]["interval"].as<string>());

  // load base model for local mode
  if (!last_checkpoint_file.empty() && _ps == nullptr)
  {
    dnn.load_model(last_checkpoint_file);
  }

  // ps 模式下对网络参数和0th稀疏特征embedding进行初始化
  if (_ps != nullptr)
  {
    cout << "Init Fc Params" << endl;
    ParamInfo fc_params = dnn.get_fc_parameters();
    cout << "Fc params Count:" << fc_params.size() << endl;
    int keys_count = fc_params.size();
    int total_dim = 0;
    map<ps::Key, int> fc_keys_counts; // 使用map协助进行排序
    for (auto& iter : fc_params)
    {
      string param = iter.first;
      int dim = iter.second;

      ps::Key key = hash_fn(param);
      param2key[param] = key;
      key2param[key] = param;
      key2dim[key] = dim;
      fc_keys_counts[key] = 1;
      debug << param << ":" << key << ":" << dim << endl;

      total_dim +=  dim;
    }

    fc_keys.resize(keys_count, 0);
    fc_vals.resize(total_dim, 0);
    fc_lens.resize(keys_count, 0);

    cout << "fc params count: " << keys_count << endl;
    cout << "fc params total dim: " << total_dim << endl;

    int index = 0;
    for (auto &t : fc_keys_counts)
    {
      ps::Key key = t.first;
      fc_keys[index] = key;
      fc_lens[index] = key2dim[key];
      index += 1;
    }

    cout << "fc params: " << fc_keys << endl;

    // 网络参数在Push阶段初始化
    if (_customer_id == 0)
    {
      cout << "Push Fc Params to ps server: start" << endl;
      _ps->Wait(_ps->Push(fc_keys, fc_vals, fc_lens));
      cout << "Push Fc Params to ps server: end" << endl;
    }

    cout << "Init barrier start" << endl;
    ps::Postoffice::Get()->Barrier(_customer_id, ps::kWorkerGroup);
    cout << "Init barrier end" << endl;

    cout << "Init 0-th sparse fea value hash" << endl;
    // The 0-th sparse_fea's hash is zero
    ps::Key key = 0;
    string param = "0"; 
    param2key[param] = key;
    key2param[key] = param;
    key2dim[key] = emb_dim; 
  } 
}

ParamMap Procedure::proc_ps_response(vector<ps::Key>& keys, vector<float>& vals)
{
  ParamMap param_and_values;
  int start = 0;
  for (int i=0; i<keys.size(); ++i)
  {
    ps::Key key = keys[i];
    string param = key2param[key];
    int dim = key2dim[key];
    Vector values(dim);
    for (int j=0; j<dim; ++j)
      values(j) = vals[start+j];
    start += dim;
    param_and_values[param] = values;
  }
  return param_and_values;
}

void Procedure::get_values_from_ps_server(const Matrix3D& sparse_user_input, const Matrix3D& sparse_ad_input, const Matrix3D& sparse_user_ad_input)
{
  debug << "get_values_from_ps_server::Start" << endl;

  //0.获取稀疏特征值
  debug << "get_values_from_ps_server::0.获取稀疏特征值" << endl;
  ParamInfo sparse_param = dnn.get_sparse_parameters(sparse_user_input, sparse_ad_input, sparse_user_ad_input);
  debug << "sparse_param size:" << sparse_param.size() << endl;

  //1.对特征值进行hash
  debug << "get_values_from_ps_server::1.对特征值进行hash" << endl;
  map<ps::Key, int> sparse_keys_counts; // 使用map协助进行排序 
  int keys_count = sparse_param.size();
  int total_dim = 0;
  for (auto& iter : sparse_param)
  {
    string param = iter.first;;
    int dim = iter.second;
    ps::Key key;
    if (param2key.find(param) == param2key.end())
    {
      key = hash_fn(param);
      param2key[param] = key;
      key2param[key] = param;
      key2dim[key] = dim;
    }
    else
      key = param2key[param];
    debug << param << ":" << key << ":" << key2dim[key] << endl;
    total_dim += dim;
    sparse_keys_counts[key] = 1;
  }
  debug << "sparse_keys_counts's size:" << sparse_keys_counts.size() << endl;

  sparse_keys.resize(keys_count, 0);
  sparse_vals.resize(total_dim, 0);
  sparse_lens.resize(keys_count, 0);

  int index = 0;
  for (auto&t : sparse_keys_counts)
  {
    ps::Key key = t.first;
    sparse_keys[index] = key;
    sparse_lens[index] = key2dim[key];
    index += 1;
  }

  //2.从server获取稀疏特征的embedding
  debug << "get_values_from_ps_server::2.从server获取稀疏特征的embedding" << endl;
  _ps->Wait(_ps->Pull(sparse_keys, &sparse_vals));
  debug << "sparse_keys:" << sparse_keys << endl;
  debug << "sparse_vals:" << sparse_vals << endl;

  ParamMap sparse_param_and_values = proc_ps_response(sparse_keys, sparse_vals);

  //3.填充emb_dict
  debug << "get_values_from_ps_server::3.填充emb_dict" << endl;
  dnn.set_sparse_parameters_and_values(sparse_param_and_values);

  //4.获取网络参数
  debug << "get_values_from_ps_server::4.获取网络参数" << endl;
  _ps->Wait(_ps->Pull(fc_keys, &fc_vals));
  debug << "fc_keys:" << fc_keys << endl;
  debug << "fc_values:" << fc_vals << endl;

  ParamMap fc_param_and_values = proc_ps_response(fc_keys, fc_vals);

  //5.设置网络参数
  debug << "get_values_from_ps_server::5.设置网络参数" << endl;
  dnn.set_fc_parameters_and_values(fc_param_and_values);

  debug << "get_values_from_ps_server::End" << endl;
}

void Procedure::prepare_ps_req(ParamMap& param_gradient, vector<ps::Key>& keys, vector<float>& vals)
{
  int start = 0;
  for (int i=0; i<keys.size();i++)
  {
    ps::Key key = keys[i];
    string param = key2param[key];
    Vector gradient = param_gradient[param];
    int dim = gradient.size();
    for (int j=0; j<dim; j++)
      vals[start+j] = gradient(j);
    start += dim;
  }
}

void Procedure::update_params_on_psserver()
{
  debug << "update_params_on_psserver::Start" << endl;

  //0.获取稀疏特征值梯度
  debug << "update_params_on_psserver::0.获取稀疏特征值梯度" << endl;
  ParamMap sparse_gradient = dnn.get_sparse_gradient();

  //1.组装push参数
  debug << "update_params_on_psserver::1.组装push参数" << endl;
  prepare_ps_req(sparse_gradient, sparse_keys, sparse_vals);
  debug << "sparse_keys:" << sparse_keys << endl;
  debug << "sparse_vals:" << sparse_vals << endl;

  //2.update稀疏特征
  debug << "update_params_on_psserver::2.update稀疏特征" << endl;
  _ps->Wait(_ps->Push(sparse_keys, sparse_vals, sparse_lens));

  //3.获取网络参数梯度
  debug << "update_params_on_psserver::3.获取网络参数梯度" << endl;
  ParamMap fc_gradient = dnn.get_fc_gradient();

  //4.组装push参数
  debug << "update_params_on_psserver::4.组装push参数" << endl;
  prepare_ps_req(fc_gradient, fc_keys, fc_vals);
  debug << "fc_keys:" << fc_keys << endl;
  debug << "fc_vals:" << fc_vals << endl;

  //5.update网络参数
  debug << "update_params_on_psserver::5.update网络参数" << endl;
  _ps->Wait(_ps->Push(fc_keys, fc_vals, fc_lens));

  debug << "update_params_on_psserver::End" << endl;
}

int Procedure::do_forward_backward(int batch_size, float& loss, float& indicator, string& loss_conf, bool train_flag)
{
  Matrix dense_user_input;
  Matrix dense_ad_input;
  Matrix dense_user_ad_input;
  Matrix3D sparse_user_input;
  Matrix3D sparse_ad_input;
  Matrix3D sparse_user_ad_input;
  Matrix label;

  int n_sample = 0;
  debug << "start to read sample" << endl;
  if (train_flag)
    n_sample = datasets.read_train_data(dense_user_input, dense_ad_input, 
        dense_user_ad_input, sparse_user_input, sparse_ad_input, sparse_user_ad_input,
        label, batch_size);
  else
    n_sample = datasets.read_test_data(dense_user_input, dense_ad_input, 
        dense_user_ad_input, sparse_user_input, sparse_ad_input, sparse_user_ad_input,
        label, batch_size);
  debug << "sample number: " << n_sample << endl;
  if (n_sample < batch_size)
  {
    cout << "sample number: " << n_sample << endl;
    cout << "reached the end of the file"  << endl;
    return -1;
  }

  if (_ps != nullptr)
  {
    //ps模式中，首先把稀疏特征的embedding和网络参数的值从pserver取回来
    get_values_from_ps_server(sparse_user_input, sparse_ad_input, sparse_user_ad_input);
  }

  dnn.forward(dense_user_input, dense_ad_input, dense_user_ad_input,
      sparse_user_input, sparse_ad_input, sparse_user_ad_input);

  // 只有在train阶段才进行backward和梯度更新
  if (train_flag)
  {
    dnn.backward(dense_user_input, dense_ad_input, dense_user_ad_input,
        sparse_user_input, sparse_ad_input, sparse_user_ad_input, label);

    // optimize
    if (_ps != nullptr)
    {
      // ps模式下，更新参数在pserver上
      update_params_on_psserver();
    }
    else
    {
      // 本地更新参数
      dnn.update();
    }
  }

  loss = dnn.get_loss();
  if (loss_conf == "cross_entropy")
  {
    indicator = compute_auc(dnn.output(), label);
  }
  else
  {
    indicator = compute_mse(dnn.output(), label);
  }

  return 0;
}

void Procedure::train()
{
  int n_epoch = stoi(DnnIni["train"]["epoch"].as<string>());
  int batch_size = stoi(DnnIni["train"]["batch_size"].as<string>());
  string loss_conf = DnnIni["loss"]["loss"].as<string>();
  int ith_batch = 0;
  float loss = 0.0;
  float indicator = 0.0;

  cout << "n_epoch:\t" << n_epoch << endl;
  cout << "batch_size:\t" << batch_size << endl;
  cout << "loss_conf:\t" << loss_conf << endl;

  cout << "Start to Train " << endl;
  for (int epoch = 0; epoch < n_epoch; epoch ++) 
  {
    clock_t stime = clock();
    while(true)
    {
      if(do_forward_backward(batch_size, loss, indicator, loss_conf, true) < 0)
        break;

      ith_batch++;

      if (debug || ith_batch % debug_interval == 0) {
        cout << ith_batch << "-th batch, loss: " << loss << endl;
        if (loss_conf == "cross_entropy")
        {
          cout << "train auc: " << indicator << endl;
        }
        else
        {
          cout << "train mse: " << indicator << endl;
        }
        dnn.print_gradient();
        dnn.print_parameters_and_values();
        dnn.print_layers_out();
        //test();
      }
      //if (ith_batch > 20)
      //  exit(1);
    }

    datasets.set_train_file(train_file);

    clock_t etime1 = clock();
    cout << "Epoch:" << epoch << " Train Time:" << (etime1 - stime) * 1.0 / CLOCKS_PER_SEC << endl;


    //save model for local mode
    if (_ps == nullptr)
      dnn.save_model(checkpoint_file);
    else
      save_param2key(param2key_file);
  }

  test();
}

float Procedure::test()
{
  // test
  clock_t etime1 = clock();
  int batch_size = stoi(DnnIni["test"]["batch_size"].as<string>());
  int ith_batch = 0;
  string loss_conf = DnnIni["loss"]["loss"].as<string>();
  float loss = 0.0;
  float indicator = 0.0;
  float total_test_indicator = 0.0;

  cout << "Start to Test " << endl;
  while(true)
  {
    if(do_forward_backward(batch_size, loss, indicator, loss_conf, false) < 0)
      break;

    ith_batch++;

    total_test_indicator += indicator;

  }

  datasets.set_test_file(test_file);

  float avg_indicator = total_test_indicator / ith_batch;
  if (loss_conf == "cross_entropy")
  {
    clock_t etime2 = clock();
    cout << endl;
    cout << "Test Time:" << (etime2 - etime1) * 1.0 / CLOCKS_PER_SEC << endl;
    cout << "test auc: " << avg_indicator << endl;
    cout << endl;
    return avg_indicator;
  }
  else
  {
    clock_t etime2 = clock();
    cout << endl;
    cout << "Test Time:" << (etime2 - etime1) * 1.0 / CLOCKS_PER_SEC << endl;
    cout << "test mse: " << avg_indicator << endl;
    cout << endl;
    return avg_indicator;
  }
}

void Procedure::save_param2key(string file_name)
{
  string full_param2key_file = file_name + "_" + to_string(_customer_id);
  ofstream outfile(full_param2key_file.c_str());

  cout << "Save param2key into " <<  full_param2key_file << endl;
  for (auto iter = param2key.begin(); iter != param2key.end(); iter++)
  {
    outfile << iter->first << "\t" << to_string(iter->second) << endl;
  }
  outfile.close();
}
