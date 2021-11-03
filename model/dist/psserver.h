#ifndef __PSLITE_SERVER_H__
#define __PSLITE_SERVER_H__

//#include <functional>
//#include <typeinfo>
#include <unordered_map>
#include "ps/ps.h"
#include "../utils/utils.h"
#include "../../utils/ini.h"
#include "../utils/conf.h"

class PSServer
{
public:
    PSServer(int app_id, string dnn_conf_file, int server_id, string key2value_file, string last_key2value_file):
         _app_id(app_id), _server_id(server_id), _key2value_file(key2value_file), _last_key2value_file(last_key2value_file)
    {
        DnnIni.load(dnn_conf_file);
        _emb_dim = stoi(DnnIni["sparse_dict"]["emb_dim"].as<string>());
        string opt_conf = DnnIni["optimizer"]["optimizer"].as<string>();
        cerr << "opt_conf:\t" << opt_conf << endl;
        float learning_rate = stof(DnnIni["optimizer"]["learning_rate"].as<string>());
        _opt = gen_opt(learning_rate, opt_conf);

        cerr << "Start to init PSServer" << endl;
        if (!last_key2value_file.empty())
        {
          cerr << "psserver:Load model from "<< last_key2value_file << endl;
          load_key2value(last_key2value_file);
        }

        server = new ps::KVServer<float>(app_id);
        using namespace placeholders;
        server->set_request_handle(bind(&PSServer::req_handler, this, _1, _2, _3));

        //auto onExit = [this->server](){delete this->server;cerr << "Delete PSServer" << endl;};
        //ps::RegisterExitCallback(onExit);

        //_store[0] = Vector::Zero(_emb_dim);
        //cerr << "_store.size:" << _store.size() << endl;
        cerr << "Init PSServer Done" << endl;
    }

    ~PSServer() { cout << "destroy PSServer" << endl; save_key2value(_key2value_file);}

    void load_key2value(string file_name);
    void save_key2value(string file_name);

private:
    IniFile DnnIni;
    string _key2value_file;
    string _last_key2value_file;
    int _emb_dim;
    int _app_id;
    int _server_id;
    Optimizer* _opt;
    ps::KVServer<float>* server;
    unordered_map<ps::Key, Vector> _store;
    void req_handler(ps::KVMeta const& meta, ps::KVPairs<float> const& data, ps::KVServer<float>* server);
};

#endif
