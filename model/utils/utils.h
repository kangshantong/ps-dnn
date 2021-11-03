#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <Eigen/Core>
#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>

#if 0
 #define debug std::cout
#else
 #define debug 0 && std::cout
#endif // DEBUG

using namespace std;

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> Matrix;
typedef vector<vector<vector<string>>>  Matrix3D;//存放稀疏特征
typedef Eigen::Matrix<float, Eigen::Dynamic, 1> Vector;
typedef Eigen::Array<float, 1, Eigen::Dynamic> RowVector;
typedef unordered_map<string, Vector> ParamMap;//存放参数以及参数的值
typedef unordered_map<string, int> ParamInfo;//存放参数以及参数值的维度

template <typename T>
ostream & operator<<(ostream &out, vector<T> &data){
  for (int i=0; i<data.size(); i++)
    cout << data[i] << "\t";
  cout << endl;
  return out;
}

// 将B加在A上
void add_parammap(ParamMap& A, ParamMap& B);

static default_random_engine generator;

// Normal distribution: N(mu, sigma^2)
void set_normal_random(float* arr, int n, float mu, float sigma);

// Normal distribution: N(mu, sigma^2)
void set_normal_random_v2(Matrix &data, float mu, float sigma);

// shuffle cols of matrix
void shuffle_data(Matrix& data, Matrix& labels);

// encode discrete values to one-hot values
Matrix one_hot_encode(const Matrix& y, int n_value);

// classification accuracy
float compute_accuracy(const Matrix& preditions, const Matrix& labels);

// mse
float compute_mse(const Matrix& pred, const Matrix& target);

// check if row,col is illegal index for data
bool check_index(const Matrix& data, int row, int col);

// gradient clipped
void vector_clip(Vector& data, float min_value=1e-6, float max_value=1e6);
void matrix_clip(Matrix& data, float min_value=1e-6, float max_value=1e6);

vector<string> split(const string& str, const string& delimiter);

vector<float> split_tof(const string& str, const string& delimiter);
Vector split_tof_V(const string& str, const string& delimiter);

string extract_field(const string str, const string delimiter, int index);

string floatvector2str(const vector<float>& v, const string& delimiter);
string vector2str(const Vector& v, const string& delimiter);

int findPosVector(const vector<long unsigned int>& input , int number);

bool compare_head(const Vector& lhs, const Vector& rhs);

bool compare_head_reverse(const Vector& lhs, const Vector& rhs);

int sorted_rows_by_head(Matrix& A, bool reverse = false);

int sorted_cols_by_head(Matrix& A, bool reverse = false);

float do_compute_auc(const Matrix& res);

float compute_auc(const Matrix& pred, const Matrix& target);

#endif  // SRC_UTILS_H_
