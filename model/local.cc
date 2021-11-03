/*
 * DNN demo
 * Author: Kang Shantong
 */
#include "procedure/procedure.h"

int main(int argc,char* argv[]) {
  if(argc < 6)
  {
      cerr << "Lack params" << endl;
      cerr << "For example: nework_conf_file features_conf_file train_file test_file checkpoint_file [last_checkpoint_file]" << endl;
      exit(1);
  }
  
  string dnn_conf_file = argv[1];
  string fea_conf_file = argv[2];
  string train_file = argv[3];
  string test_file = argv[4];
  string checkpoint_file = argv[5];
  string last_checkpoint_file = "";

  if (argc >= 7)
      last_checkpoint_file = argv[6];

  cout << "dnn_conf_file:\t" << dnn_conf_file << endl;
  cout << "fea_conf_file:\t" << fea_conf_file << endl;
  cout << "train_file:\t" << train_file << endl;
  cout << "test_file:\t" << test_file << endl;
  cout << "checkpoint_file:\t" << checkpoint_file << endl;
  cout << "last_checkpoint_file:\t" << last_checkpoint_file << endl;
  
  Procedure main_proc(dnn_conf_file, fea_conf_file, train_file, test_file, checkpoint_file, last_checkpoint_file);

  main_proc.train();  

  return 0;
}
