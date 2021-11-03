#include "psserver.h"

void PSServer::req_handler(ps::KVMeta const& meta, ps::KVPairs<float> const& data, ps::KVServer<float>* server)
{
    debug << "Start to req_handler" << endl;
    debug << "_emb_dim: " << _emb_dim << endl;
    debug << "req key size: " << data.keys.size() << endl;
    debug << "req server keys: " << data.keys << endl;
    debug << "req server vals: " << data.vals << endl;
    debug << "req server lens: " << data.lens << endl;
    ps::KVPairs<float> res;
    if (meta.pull){
        debug << "pull" << endl;
        vector<float> vals;
        for (auto i = 0; i < data.keys.size(); ++i){
          auto k = data.keys[i];
          debug << "proc k: " << k << endl;
          debug << "_store size: " << _store.size() << endl;

          // !!!!!!!!!!!!embedding 在pull阶段初始化
          if ((_store.find(k)) == _store.cend()){
            debug << "init k: " << k << endl;
            if (k == 0)
            {
              Vector v = Vector::Zero(_emb_dim);
              _store[k] = v;
            }
            else
            {
              Vector v(_emb_dim);
              set_normal_random(v.data(), v.size(), 0, 0.05);
              _store[k] = v;
            }
          }

          debug << "k weight: " << _store[k] << endl;

          auto p = _store[k].data();
          auto n = _store[k].rows();
          res.keys.push_back(k);
          res.lens.push_back(n);
          copy(p, p+n, back_inserter(vals));
        }
        res.vals.CopyFrom(vals.cbegin(), vals.cend());
    }else{
        debug << "push" << endl;
        auto offset = 0;
        for (auto i = 0; i < data.keys.size(); ++i){
          auto k = data.keys[i];
          auto n = data.lens[i];

          debug << "proc k: " << k << endl;

          // !!!!!!!!非embedding参数在push截断初始化
          if (_store.find(k) == _store.cend()){
              debug << "init k: " << k << endl;
              Vector v(n);
              set_normal_random(v.data(), v.size(), 0, 0.05);
              debug << "v: " << v << endl;
              _store[k] = v;
          }
          // 梯度更新
          else
          {

            debug << "extract grad for k : " << k << endl;
            debug << "k weight: " << _store[k] << endl;
            debug << "offset: " << offset << endl;
            debug << "n: " << n << endl;
            debug << "val size: " << data.vals.size() << endl;

            Vector v = Vector::Zero(n);
            Eigen::Index j = 0;
            for (auto pos = offset; pos < offset + n && j < n; ++pos, ++j){
              v(j) = data.vals[pos];
            }

            debug << "grad v : " << v << endl;

            if (k != 0)
            {
              //_store[k] = _store[k] + _lr*v; //update
              //debug << "before update:\t" << _store[k] << endl;  
              //debug << "grad:\t" << v << endl;  
              Vector::AlignedMapType _store_vec(_store[k].data(), _store[k].size());
              Vector::ConstAlignedMapType grad_vec(v.data(), v.size());
              _opt->update(_store_vec, grad_vec);
              debug << "_store_vec.data(): " << _store_vec.data() << endl;
              //debug << "after update:\t" << _store[k] << endl;  
            }
            debug << "k new weight : " << _store[k] << endl;
          }
          offset += n;
        }
    }
    debug << "res server keys: " << res.keys << endl;
    debug << "res server vals: " << res.vals << endl;
    debug << "res server lens: " << res.lens << endl;
    server->Response(meta, res);
    debug << "End to req_handler" << endl;
}


void PSServer::save_key2value(string file_name)
{
    string full_file = file_name + "_" + to_string(_server_id);
    ofstream outfile(full_file.c_str());

    cout << "Save key2value into " << full_file << endl;
    for (auto iter = _store.begin(); iter != _store.end(); iter++)
    {
        string value_str = vector2str(iter->second, ",");
        outfile << iter->first << "\t" << value_str << endl;
    }
    outfile.close();
}

void PSServer::load_key2value(string file_name)
{
    ifstream is(file_name.c_str());
    if(!is)
    {
        cerr << "key2value File " << file_name << " doesn't exist" << endl;
        exit(1);
    }

    cout << "Load key2value from " <<  file_name << endl;

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

        ps::Key key = stol(fields[0]);
        Vector value = split_tof_V(fields[1], ",");
        _store[key] = value;

    }
    
    cout << "key2value size: " <<  _store.size() << endl;
    cout << "Load Model Done " << endl;

}
