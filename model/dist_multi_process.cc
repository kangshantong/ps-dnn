#include <unistd.h>
#include "dist/psserver.h"
#include "procedure/procedure.h"

int run_worker(string dnn_conf_file, string fea_conf_file,
    string train_file, string test_file,
    string param2key_file, string last_param2key_file,
    int customer_id)
{
  //shared_ptr<Procedure> worker;
  //worker = make_shared<Procedure>(dnn_conf_file, fea_conf_file, train_file, test_file, param2key_file, last_param2key_file, 0, customer_id);
  cout << "run worker for customer_id " << customer_id << endl;
  ps::Start(customer_id);
  cout << "Gen Procedure for customer_id " << customer_id << endl;
  auto worker = new Procedure(dnn_conf_file, fea_conf_file, train_file, test_file, param2key_file, last_param2key_file, 0, customer_id);
  cout << "Train for customer_id " << customer_id << endl;
  worker->train();
  cout << "Finalize for customer_id " << customer_id << endl;
  ps::Finalize(customer_id, true);
  return 0;
}

int main(int argc, char* argv[])
{
  if(argc < 4)
  {
    cerr << "Lack params" << endl;
    cerr << "For example: role nework_conf_file features_conf_file" << endl;
    exit(1);
  }

  string role = argv[1];
  string dnn_conf_file = argv[2];
  string fea_conf_file = argv[3];

  bool isWorker = (strcmp(role.c_str(), "worker") == 0);
  if (!isWorker) {
    ps::Start(0);
    if (ps::IsServer())
    {
      int server_id =stoi(argv[4]);
      string key2value_file = argv[5];
      string last_key2value_file = "";
      if (argc >= 7)
        last_key2value_file = argv[6];

      auto server = new PSServer(0, dnn_conf_file, server_id, key2value_file, last_key2value_file);
      auto onExit = [server](){delete server;cerr << "Delete PSServer" << endl;};
      ps::RegisterExitCallback(onExit);
    }
    cout<<"Wait for the client thread to finish"<<endl;
    ps::Finalize(0, true);
    cout<<"Scheduler/Server Exiting from Main Thread"<<endl;
    return 0;
  }else{
    string train_dir = argv[4];
    string test_dir = argv[5];
    int customer_id =stoi(argv[6]);
    string param2key_file = argv[7];
    string last_param2key_file = "";
    if (argc >= 9)
      last_param2key_file = argv[8];

    string file_name = to_string(customer_id);
    if (customer_id < 10)
      file_name = "0" + to_string(customer_id);
    string train_file = train_dir + "/" + file_name; 
    string test_file = test_dir + "/" + file_name;  
    run_worker(dnn_conf_file, fea_conf_file, train_file, test_file, param2key_file, last_param2key_file, customer_id);

    cout<<"Worker " << customer_id << " is done"<<endl;
  }
  return 0;
}
