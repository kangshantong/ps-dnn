#ifndef SRC_PROCEDURE_H_
#define SRC_PROCEDURE_H_

#include <stdlib.h>
#include <vector>
#include "../../utils/ini.h"
#include "../utils/conf.h"
#include "../dataset/dataset.h"
#include "../network/network.h"
#include "ps/ps.h"

class Procedure {
 private:
  string dnn_conf_file;;
  string fea_conf_file;;
  string train_file;;
  string test_file;;
  string checkpoint_file;
  string last_checkpoint_file;
  string param2key_file;
  string last_param2key_file;

  IniFile FeaIni;
  IniFile DnnIni;
  Dataset datasets;
  Network dnn;

  int _app_id;
  int _customer_id;
  int rank;
  shared_ptr<ps::KVWorker<float>> _ps;
  hash<string> hash_fn; //has函数
  unordered_map<string, ps::Key> param2key; // 缓存param->ps::Key
  unordered_map<ps::Key, string> key2param; // 缓存ps::Key->param
  unordered_map<ps::Key, int> key2dim; // 缓存ps::Key->dim

  vector<ps::Key> sparse_keys;
  vector<float> sparse_vals; 
  vector<int> sparse_lens;

  vector<ps::Key> fc_keys;
  vector<float> fc_vals; 
  vector<int> fc_lens;

  int debug_interval;

 public:
  // for local模式
  Procedure(string dnn_conf_file, string fea_conf_file, string train_file, 
            string test_file, string checkpoint_file, string last_checkpoint_file) : 
            dnn_conf_file(dnn_conf_file), fea_conf_file(fea_conf_file), train_file(train_file),
            test_file(test_file),checkpoint_file(checkpoint_file),
            last_checkpoint_file(last_checkpoint_file),_app_id(-1),
            _customer_id(-1),rank(-1), debug_interval(1000)
  {
    cout << "dnn_conf_file:\t" << dnn_conf_file << endl;
    cout << "fea_conf_file:\t" << fea_conf_file << endl;
    cout << "train_file:\t" << train_file << endl;
    cout << "test_file:\t" << test_file << endl;
    cout << "checkpoint_file:\t" << checkpoint_file << endl;
    cout << "last_checkpoint_file:\t" << last_checkpoint_file << endl;
    _ps = nullptr;
    init();
  }
  //for ps模式
  Procedure(string dnn_conf_file, string fea_conf_file, string train_file, 
            string test_file, string param2key_file, string last_param2key_file,
            int app_id, int customer_id) : 
            dnn_conf_file(dnn_conf_file), fea_conf_file(fea_conf_file), train_file(train_file),
            test_file(test_file),param2key_file(param2key_file),
            last_param2key_file(last_param2key_file),_app_id(app_id),
            _customer_id(customer_id), debug_interval(1000)
  {
    cout << "Start to init Worker " << customer_id << endl;
    cout << "dnn_conf_file:\t" << dnn_conf_file << endl;
    cout << "fea_conf_file:\t" << fea_conf_file << endl;
    cout << "train_file:\t" << train_file << endl;
    cout << "test_file:\t" << test_file << endl;
    cout << "param2key_file:\t" << param2key_file << endl;
    cout << "last_param2key_file:\t" << last_param2key_file << endl;
    cout << "customer_id:\t" << customer_id << endl;
    rank = ps::MyRank();
    cout << "rank: " << rank << endl;
    _ps = make_shared<ps::KVWorker<float>>(app_id, customer_id);
    init();
    cout << "Init Worker Done: rank: " << rank << endl;
  }
  ~Procedure() {}

  void init();
  int do_forward_backward(int batch_size, float& loss, float& indicator, string& loss_conf, bool train_flag);
  void train();
  float test();

  //以下函数在ps模式下使用
  void get_values_from_ps_server(const Matrix3D& sparse_user_input, const Matrix3D& sparse_ad_input, const Matrix3D& sparse_user_ad_input);
  ParamMap proc_ps_response(vector<ps::Key>& keys, vector<float>& vals);
  void prepare_ps_req(ParamMap& param_gradient, vector<ps::Key>& keys, vector<float>& vals);
  void update_params_on_psserver();
  void save_param2key(string file_name);
};

#endif  // SRC_PROCEDURE_H_
