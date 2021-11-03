#include "utils.h"

// 将B加在A上
void add_parammap(ParamMap& A, ParamMap& B)
{
  for(auto& iter : B)
  {
    string param = iter.first;
    Vector value = iter.second;
    if (A.find(param) == A.end())
    {
      A[param] = value;
    }
    else
    {
      A[param] = A[param] + value;
    }
  }
}


// Normal distribution: N(mu, sigma^2)
void set_normal_random(float* arr, int n, float mu, float sigma) {
// if matrix's mem addr space is not continuous, it maybe cause problem
  normal_distribution<float> distribution(mu, sigma);
  for (int i = 0; i < n; i ++) {
    arr[i] = distribution(generator);
  }
}

// Normal distribution: N(mu, sigma^2)
void set_normal_random_v2(Matrix &data, float mu, float sigma) {
// if matrix's mem addr space is not continuous, it maybe cause problem
  normal_distribution<float> distribution(mu, sigma);
  int cols = data.cols();
  int rows = data.rows();
  for (int i = 0; i < rows; i ++)
    for (int j = 0; j < cols; j ++) {
        data(i,j) = distribution(generator);
  }
}
// shuffle cols of matrix
void shuffle_data(Matrix& data, Matrix& labels) {
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> perm(data.cols());
  perm.setIdentity();
  random_shuffle(perm.indices().data(), perm.indices().data()
                      + perm.indices().size());
  data = data * perm;  // permute columns
  labels = labels * perm;
}

// encode discrete values to one-hot values
Matrix one_hot_encode(const Matrix& y, int n_value) {
  int n = y.cols();
  Matrix y_onehot = Matrix::Zero(n_value, n);
  for (int i = 0; i < n; i ++) {
    y_onehot(int(y(i)), i) = 1;
  }
  return y_onehot;
}

// classification accuracy
float compute_accuracy(const Matrix& preditions, const Matrix& labels) {
  int n = preditions.cols();
  float acc = 0;
  for (int i = 0; i < n; i ++) {
    Matrix::Index max_index;
    float max_value = preditions.col(i).maxCoeff(&max_index);
    acc += int(max_index) == labels(i);
  }
  return acc / n;
}

// mse
float compute_mse(const Matrix& pred, const Matrix& target) {
  int n = pred.cols();
  Matrix diff = pred - target;
  Matrix rmse_diff(1, n);
  Matrix exp_pred(1, n);
  Matrix exp_target(1, n);
  Matrix exp_diff(1, n);
  Matrix exp_rmse_diff(1, n);
  for (int i=0; i<n; i++)
  {
    rmse_diff(0, i) = diff(0, i) * diff(0, i);
    exp_pred(0, i) = exp(pred(0, i));
    exp_target(0, i) = exp(target(0, i));
    //exp_pred(0, i) = pred(0, i)*1000;
    //exp_target(0, i) = target(0, i)*1000;
    exp_diff(0, i) = exp_pred(0, i) - exp_target(0, i);
    exp_rmse_diff(0, i) = exp_diff(0, i) * exp_diff(0, i);
  }
 
  debug << endl;
  debug << "Total test:" << endl;
  debug << "pred" << "\t" << pred.mean() << "\t" << endl;
  debug << "target" << "\t" << target.mean() << "\t" << endl;
  debug << "diff" << "\t" << diff.mean() << "\t" << endl;
  debug << "rmse" << "\t" << sqrt(rmse_diff.mean()) << "\t" << endl;
  debug << "norm_pred" << "\t" << exp_pred.mean() << "\t" << endl;
  debug << "norm_target" << "\t" << exp_target.mean() << "\t" << endl;
  debug << "norm_diff" << "\t" << exp_diff.mean() << "\t" << endl;
  debug << "norm_rmse" << "\t" << sqrt(exp_rmse_diff.mean()) << endl;
  debug << endl;

  debug << "First 100 test case:" << endl;
  int out_num = min(100, n);
  debug << "pred" << "\t" << "target" << "\t" << "diff" << "\t" 
      << "norm_pred" << "\t" << "norm_target" << "\t" << "norm_diff" << endl;
  for (int i=0; i<out_num; i++)
  {
    debug << pred(0,i) << "\t" << target(0,i) << "\t" << diff(0,i) << "\t" 
        << exp(pred(0,i)) << "\t" << exp(target(0,i)) << "\t" << exp(pred(0,i)) - exp(target(0,i)) << endl;
        //<< 1000 * (pred(0,i)) << "\t" << 1000 * (target(0,i)) << "\t" << 1000 * (pred(0,i)) - 1000 * (target(0,i)) << endl;
  }
  
  //float loss = diff.cwiseProduct(diff).sum();
  //return loss /= n;
  return sqrt(exp_rmse_diff.mean());
}

// check if row,col is illegal index for data
bool check_index(const Matrix& data, int row, int col) {
  int rows = data.rows();
  int cols = data.cols();
  if (row>=0 && row<rows && col>=0 && col<cols)
    return true;
  cerr << "data.size:\t" << data.rows() << "\t" << data.cols() << endl;
  cerr << "row:" << row << " col:" << col << endl;
  return false;
}

// gradient clipped
void vector_clip(Vector& data, float min_value, float max_value) {
  for (int i=0; i<data.size(); i++)
  {
    float orig_value = data(i);
    if (orig_value == 0)
      continue;

    float sign = (orig_value > 0.0) ? 1.0 : -1.0;
    float abs_orig_value = orig_value * sign;
    if (abs_orig_value < min_value)
    {
      data(i) = sign * min_value;
    }
    if (abs_orig_value > max_value)
    {
      data(i) = sign * max_value;
    }
  }
}

// gradient clipped
void matrix_clip(Matrix& data, float min_value, float max_value) {
  int rows = data.rows();
  int cols = data.cols();
  for (int i=0; i<rows; i++)
    for (int j=0; j<cols; j++)
    {
      float orig_value = data(i,j);
      if (orig_value == 0)
        continue;

      float sign = (orig_value > 0.0) ? 1.0 : -1.0;
      float abs_orig_value = orig_value * sign;
      if (abs_orig_value < min_value)
      {
        data(i,j) = sign * min_value;
      }
      if (abs_orig_value > max_value)
      {
        data(i,j) = sign * max_value;
      }
    }
}

vector<string> split(const string& str, const string& delimiter)
{
    vector<string> res;
    if(str == "") 
        return res;
    //在字符串末尾也加入分隔符，方便截取最后一段
    string strs = str + delimiter;
    size_t pos = strs.find(delimiter);

    while(pos != strs.npos)
    {   
        string temp = strs.substr(0, pos);
        res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos+1, strs.size());
        pos = strs.find(delimiter);
    }   

    return res;
}

vector<float> split_tof(const string& str, const string& delimiter)
{
    vector<float> res;
    if(str == "") 
        return res;
    //在字符串末尾也加入分隔符，方便截取最后一段
    string strs = str + delimiter;
    size_t pos = strs.find(delimiter);

    while(pos != strs.npos)
    {   
        string temp = strs.substr(0, pos);
        res.push_back(stof(temp));
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos+1, strs.size());
        pos = strs.find(delimiter);
    }   

    return res;
}

Vector split_tof_V(const string& str, const string& delimiter)
{
    Vector res;
    vector<float> temp = split_tof(str, delimiter);
    int l = temp.size();
    res.resize(l);
    for (int i=0;i<l;i++)
      res[l] = temp[l];

    return res;
}

string extract_field(const string str, const string delimiter, int index)
{
    vector<string> res;
    if(str == "")
        return "";
    //在字符串末尾也加入分隔符，方便截取最后一段
    string strs = str + delimiter;
    size_t pos = strs.find(delimiter);

    while(pos != strs.npos)
    {
        string temp = strs.substr(0, pos);
        res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos+1, strs.size());
        pos = strs.find(delimiter);
    }

    if (index < 0 || index >= res.size())
    {
        cerr << "str: " << str << endl;
        cerr << "delimiter: " << delimiter << endl;
        cerr << "illegal index: " << index << endl;
        cerr << "string fields: " << res.size() << endl;
        return "";
    }

    return res[index];
}

string floatvector2str(const vector<float>& v, const string& delimiter)
{
    string s;
    int length = v.size(), i = 0;
    for (const auto piece : v)  
    {   
        s += to_string(piece);
        if (i != length - 1)
            s += delimiter;
        i++;
    }   
    return s;
}

string vector2str(const Vector& v, const string& delimiter)
{
    string s;
    int length = v.size();
    for (int i=0; i<length; i++)  
    {   
        s += to_string(v(i));
        if (i != length - 1)
            s += delimiter;
    }   
    return s;
}

int findPosVector(const vector<long unsigned int>& input , int number)
{
  int index = -1;
  for(int i = 0; i < input.size(); ++i)
  {
    if (input[i] == number)
    {
      index = i;
      break;
    }
  }
  return index;
}

bool compare_head(const Vector& lhs, const Vector& rhs)
{
      return lhs(0) < rhs(0);
}

bool compare_head_reverse(const Vector& lhs, const Vector& rhs)
{
      return lhs(0) > rhs(0);
}

int sorted_rows_by_head(Matrix& A, bool reverse)
{
  vector<Vector> vec;
  for (int64_t i = 0; i < A.rows(); ++i)
    vec.push_back(A.row(i));

  if (reverse)
    sort(vec.begin(), vec.end(), &compare_head_reverse);
  else
    sort(vec.begin(), vec.end(), &compare_head);

  for (int64_t i = 0; i < A.rows(); ++i)
    A.row(i) = vec[i];

  return 0;
}

int sorted_cols_by_head(Matrix& A, bool reverse)
{
  vector<Vector> vec;
  for (int64_t i = 0; i < A.cols(); ++i)
    vec.push_back(A.col(i));

  if (reverse)
    sort(vec.begin(), vec.end(), &compare_head_reverse);
  else
    sort(vec.begin(), vec.end(), &compare_head);

  for (int64_t i = 0; i < A.cols(); ++i)
    A.col(i) = vec[i];

  return 0;
}

float do_compute_auc(const Matrix& res)
{
  int n = res.cols();
  float pos_ranks = 0.0;
  int pos_labels = 0;
  for (int j=0; j<n; j++)
  {
    if (res(1,j) == 1)
    {
      int left = j;
      int right = j;

      for(int x=j-1; x>=0; x--)
      {
        if(x>=0 && res(0,x) == res(0,j))
          left = x;
        else
          break;
      }

      for(int y=j+1; y<n; y++)
      {
        if(y<n && res(0,y) == res(0,j))
          right = y;
        else
          break;
      }


      pos_ranks += ((left + right) / 2.0 + 1.0);
      pos_labels += 1;
    }
  }

  int neg_labels = n - pos_labels;
  float auc = 0.0;
  if (pos_labels > 0)
    auc = (pos_ranks - pos_labels*(pos_labels+1.0)/2)/(float(pos_labels) * neg_labels);

  debug << "pos_labels:" << pos_labels << endl;
  debug << "neg_labels:" << neg_labels << endl;

  //debug << "pos_ranks:" << pos_ranks << endl;
  //debug << "pos_labels*(pos_labels+1.0)/2:" << pos_labels*(pos_labels+1.0)/2 << endl; 
  //debug << "pos_ranks - pos_labels*(pos_labels+1.0)/2:" << pos_ranks - pos_labels*(pos_labels+1.0)/2 << endl;;
  //debug << "pos_labels * neg_labels: " << float(pos_labels) * neg_labels << endl;
  //debug << "auc: " << auc << endl;
  return auc;
}

// auc
float compute_auc(const Matrix& pred, const Matrix& target)
{
  int n = pred.cols();
  float avg_pred = pred.mean();
  float avg_target = target.mean(); 
  float bias = avg_pred/avg_target - 1.0;
  debug << "bias: " << bias << endl;
  Matrix res(2, n);
  res << pred,
      target;
  sorted_cols_by_head(res);
  debug << "Train res: " << res.block(0, 0, 2, min(n, 128)) << endl;
  float auc = do_compute_auc(res);
  return auc;
}
