#include "./dataset.h"
#include<cmath>

int Dataset::read_dense_data(string& sample_dense, Matrix& dense_data, int dim_dense, int index)
{
  vector<string> dense_feas = split(sample_dense, output_fea_outer_delimiter_);
  if (dim_dense != dense_feas.size())
  {
    cerr << "dim_dense != dense_feas.size()" << endl;
    cerr << "dim_dense:" << dim_dense << " vs dense_feas.size():" << dense_feas.size() << endl;
    cerr << "sample_dense:" << sample_dense << endl;
    return -1;
  }
  for (int i=0; i<dim_dense; i++)
  {
    if (dense_feas[i].size() > 0)
      dense_data(i,index) = stof(dense_feas[i]);
    // dense feature 默认值为 0.0
    else
      dense_data(i,index) = 0.0;
  }
  return 0;
}

void Dataset::resize_sparse_data(Matrix3D& sparse_data, int dim_sparse, int needs)
{
  sparse_data.resize(dim_sparse);
  for (int i=0; i<dim_sparse; i++)
    sparse_data[i].resize(needs);
}

int Dataset::read_sparse_data(string& sample_sparse, Matrix3D& sparse_data, int dim_sparse, int index)
{
  vector<string> sparse_feas = split(sample_sparse, output_fea_outer_delimiter_);
  if (dim_sparse != sparse_feas.size())
  {
    cerr << "dim_sparse != sparse_feas.size()" << endl;
    cerr << "dim_sparse:" << dim_sparse << " vs sparse_feas size:" << sparse_feas.size() << endl;
    cerr << "sample_sparse:" << sample_sparse << endl;
    return -1;;
  }
  for (int i=0; i<dim_sparse; i++)
  {
    vector<string> sparse_fea_values = split(sparse_feas[i], output_fea_intermediate_delimiter_);
    sparse_data[i][index] = sparse_fea_values;
    //cout << "sparse_feas: " << i << " " << sparse_feas[i] << " vs ";
    //for (int k=0; k<sparse_data[i][count].size(); k++)
    //  cout << sparse_data[i][count][k] << "\t";
    //cout << endl;
  }
  return 0;
}

int Dataset::read_dataset_data(std::ifstream& file, Matrix& dense_user, Matrix& dense_ad, Matrix& dense_user_ad,
      Matrix3D& sparse_user, Matrix3D& sparse_ad, Matrix3D& sparse_user_ad, 
      Matrix& labels, int needs)
{
  dense_user.resize(dim_dense_user_, needs);
  dense_ad.resize(dim_dense_ad_, needs);
  dense_user_ad.resize(dim_dense_user_ad_, needs);

  resize_sparse_data(sparse_user, dim_sparse_user_, needs);
  resize_sparse_data(sparse_ad, dim_sparse_ad_, needs);
  resize_sparse_data(sparse_user_ad, dim_sparse_user_ad_, needs);

  labels.resize(1, needs);

  int count = 0;
  while(file.good() && !file.eof()){
    string line;
    getline(file,line);
    vector<string> fields = split(line, output_fea_group_delimiter_);
    // skip if line fields is less than 8
    if (fields.size() < 8)
      continue;

    string id = fields[0];
    float label = stof(fields[1]);
    string dense_features_user = fields[2];
    string dense_features_ad = fields[3];
    string dense_features_user_ad = fields[4];
    string sparse_features_user = fields[5];
    string sparse_features_ad = fields[6];
    string sparse_features_user_ad = fields[7];

    labels(0, count) = label;

    int res = 0; //用于记录dense或者sparse特征是否有读取失败
    res += read_dense_data(dense_features_user, dense_user, dim_dense_user_, count);
    res += read_dense_data(dense_features_ad, dense_ad, dim_dense_ad_, count);
    res += read_dense_data(dense_features_user_ad, dense_user_ad, dim_dense_user_ad_, count);

    res += read_sparse_data(sparse_features_user, sparse_user, dim_sparse_user_, count);
    res += read_sparse_data(sparse_features_ad, sparse_ad, dim_sparse_ad_, count);
    res += read_sparse_data(sparse_features_user_ad, sparse_user_ad, dim_sparse_user_ad_, count);

    if (res < 0)
    {
      cerr << "Error: read sample failed:" << line << endl;
      continue;
    }

    count +=1;
    if (count >= needs)
      break;
  }

  if (count < needs)
  {
      dense_user.conservativeResize(dim_dense_user_, count);
      dense_ad.conservativeResize(dim_dense_ad_, count);
      dense_user_ad.conservativeResize(dim_dense_user_ad_, count);
      for (int i=0; i<dim_sparse_user_; i++)
        sparse_user[i].resize(count);
      for (int i=0; i<dim_sparse_ad_; i++)
        sparse_ad[i].resize(count);
      for (int i=0; i<dim_sparse_user_ad_; i++)
        sparse_user_ad[i].resize(count);
      labels.conservativeResize(1, count);
  }

  if (file.eof())
  {
    file.close();
    file.clear();
  }

  return count;
}

void Dataset::set_train_file(const string& new_train_file) {
  train_stream_.open(new_train_file);
  train_file_ = new_train_file;
}

void Dataset::set_test_file(const string& new_test_file) {
  test_stream_.open(new_test_file);
  test_file_ = new_test_file;
}

int Dataset::read_train_data(Matrix& dense_user, Matrix& dense_ad, Matrix& dense_user_ad,
    Matrix3D& sparse_user, Matrix3D& sparse_ad, Matrix3D& sparse_user_ad, 
    Matrix& labels, int needs) {
  return read_dataset_data(train_stream_, dense_user, dense_ad, dense_user_ad, sparse_user, sparse_ad, sparse_user_ad, labels, needs);
}

int Dataset::read_test_data(Matrix& dense_user, Matrix& dense_ad, Matrix& dense_user_ad,
    Matrix3D& sparse_user, Matrix3D& sparse_ad, Matrix3D& sparse_user_ad, 
    Matrix& labels, int needs) {
  return read_dataset_data(test_stream_, dense_user, dense_ad, dense_user_ad, sparse_user, sparse_ad, sparse_user_ad, labels, needs);
}

void Dataset::set_fea_dim(int dense_dim_user, int dense_dim_ad,  int dense_dim_user_ad,
    int sparse_dim_user, int sparse_dim_ad, int sparse_dim_user_ad)
{
  dim_dense_user_ = dense_dim_user;
  dim_dense_ad_ = dense_dim_ad;
  dim_dense_user_ad_ = dense_dim_user_ad;
  dim_sparse_user_ = sparse_dim_user;
  dim_sparse_ad_ = sparse_dim_ad;
  dim_sparse_user_ad_ = sparse_dim_user_ad;
}
