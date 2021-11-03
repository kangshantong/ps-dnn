#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include "../third_party/eigen/Eigen/Core"
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <fstream>

using namespace std;

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> Matrix;
typedef vector<vector<vector<float>>>  Matrix3D;
typedef Eigen::Matrix<float, Eigen::Dynamic, 1> Vector;
typedef Eigen::Array<float, 1, Eigen::Dynamic> RowVector;
typedef unordered_map<string, Vector*> ParamMap;

//using namespace Eigen;

float lr = 0.01;
float decay_v0 =5e-4;
float decay_v1 =5e-3;
float momentum = 0.9;
bool nesterov = true;
float beta1 = 0.9;
float beta2=0.999;
float eps = 1e-8;
std::unordered_map<const float*, Vector> v_map;  // velocity
std::unordered_map<const float*, Vector> s_map; 
std::unordered_map<const float*, int> counts; 

string ex_file = "./dist_ex_file"; 

//class Singleton
class Postoffice {
  public:
    /**
     *    * \brief return the singleton object
     *       */
    static Postoffice* Get() {
      static Postoffice e; return &e;
    }
    std::thread::id pid = std::this_thread::get_id();
};

void write_exfile(string file_name)
{
    ofstream outfile(file_name.c_str());
    outfile << "done" << endl;
    outfile.close();
}

bool read_exfile(string file_name)
{
    ifstream is(file_name.c_str());
    if(!is)
    {
      cout << file_name << " doesn't exists!!!" << endl;
      return false;
    }
    else
    {
     cout << file_name << " is ready!!!" << endl;
    }
    return true;
}

void run_worker(int i)
{
  sleep(i);
  cout << "worker:" << i << " pid:" << std::this_thread::get_id() << " singleon:" << Postoffice::Get()->pid << endl;
}

void sgd_update(Vector::AlignedMapType& w,
                 Vector::ConstAlignedMapType& dw) {
  // refer to SGD in PyTorch:
  // https://github.com/pytorch/pytorch/blob/master/torch/optim/sgd.py
  // If v is zero, initialize it
  Vector& v = v_map[w.data()];
  if (v.size() == 0) {
    v.resize(w.size());
    v.setZero();
  }

  Vector l_grad = dw + decay_v0 * w;
  cout << "w:" << w << " dw:" << dw << " decay_v0:" << decay_v0 << " l_grad:" << l_grad << endl;
  // update v
  v = momentum * v + l_grad;
  cout << "momentum:" << momentum << " v:" << v << endl;

  // update w
  if (nesterov)
    w -= lr * (momentum * v + l_grad);
  else
    w -= lr * v;
  cout << "nesterov:" << nesterov << " lr:" << lr << " new w:" << w << endl;
}

void adam_update(Vector::AlignedMapType& w,
                 Vector::ConstAlignedMapType& dw) {
  // refer:https://github.com/pytorch/pytorch/blob/master/torch/optim/adam.py
  // If v/s is zero, initialize it
  Vector& v = v_map[w.data()];
  if (v.size() == 0) {
    v.resize(w.size());
    v.setZero();
  }
  Vector& s = s_map[w.data()];
  if (s.size() == 0) {
    s.resize(w.size());
    s.setZero();
  }
  
  if (counts.find(w.data()) == counts.end())
    counts[w.data()] = 2;

  int count = counts[w.data()];
  Vector l_grad = dw + decay_v1 * w;

  cout << "w:" << w << endl; 
  cout << "dw:" << dw << endl; 
  cout << "l_grad:" << l_grad << endl;
  float bias_correction1 = 1 - pow(beta1, count);
  float bias_correction2 = 1 - pow(beta2, count);

  v = beta1 * v + (1 - beta1) * l_grad;
  s = beta2 * s + (1 - beta2) * (l_grad.cwiseProduct(l_grad));
  //Vector denom = s.cwiseSqrt() / (sqrt(bias_correction2) + eps);
  float step_size = lr * sqrt(bias_correction1) / bias_correction2;

  cout << "v:\t" << v << endl;
  cout << "s:\t" << s << endl;
  //cout << "denom:\t" << denom << endl;

  //float step_size = lr / bias_correction1;
  cout << "step_size:\t" << step_size << endl;

  //w -= step_size * (v.cwiseQuotient(denom + Vector::Constant(w.size(), eps)));
  w -= step_size * (v.cwiseQuotient(s.cwiseSqrt() + Vector::Constant(w.size(), eps)));
  cout << "w:" << w << endl; 

  counts[w.data()] += 1;
}
void update(Vector::AlignedMapType& w,
                 Vector::ConstAlignedMapType& dw) {
  Vector& v = v_map[dw.data()];
  if (v.size() == 0) {
    v.resize(dw.size());
    v.setZero();
  }
  // update v
  v = momentum * v + (dw + decay_v1 * w);
  // update w
  if (nesterov)
    w -= lr * (momentum * v + (dw + decay_v1 * w));
  else
    w -= lr * v;
}

void update_emb(Matrix &w,
                 Vector& dw, int index, int dim) {
  int rows = w.rows();
  Vector& v = v_map[w.data()];
  if (v.size() == 0) {
    v.resize(w.size());
    v.setZero();
  }
  // update v
  for (int i=0;i<dim;i++)
  {
    int v_index = i * rows + index;
    v[v_index] = momentum * v[v_index] + (dw(i) + decay_v1 * w(index,i));
    // update w
    if (nesterov)
      w(index,i) -= lr * (momentum * v[v_index] + (dw(i) + decay_v1 * w(index,i)));
    else
      w(index,i) -= lr * v[v_index];
  }
}

bool compare_head(const Vector& lhs, const Vector& rhs)
{
      return lhs(0) < rhs(0);
}

bool compare_head_reverse(const Vector& lhs, const Vector& rhs)
{
      return lhs(0) > rhs(0);
}

int sorted_rows_by_head(Matrix& A)
{
  std::vector<Vector> vec;
  for (int64_t i = 0; i < A.rows(); ++i)
    vec.push_back(A.row(i));

  std::sort(vec.begin(), vec.end(), &compare_head);

  for (int64_t i = 0; i < A.rows(); ++i)
    A.row(i) = vec[i];

  return 0;
}

int sorted_cols_by_head(Matrix& A, bool reverse = false)
{
  std::vector<Vector> vec;
  for (int64_t i = 0; i < A.cols(); ++i)
    vec.push_back(A.col(i));

  if (reverse)
    std::sort(vec.begin(), vec.end(), &compare_head_reverse);
  else
    std::sort(vec.begin(), vec.end(), &compare_head);

  for (int64_t i = 0; i < A.cols(); ++i)
    A.col(i) = vec[i];

  return 0;
}

float compute_auc(Matrix res)
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

      for(int x=j-1; x>0; x--)
        if(x>0 && res(0,x) == res(0,j))
          left = x;
        else
          break;

      for(int y=j+1; y<n; y++)
        if(y<n && res(0,y) == res(0,j))
          right = y;
        else
          break;


      pos_ranks += ((left + right) / 2.0 + 1.0);
      pos_labels += 1;
    }
  }

  int neg_labels = n - pos_labels;
  float auc = (pos_ranks - pos_labels*(pos_labels+1.0)/2)/(pos_labels * neg_labels);

  cout << "pos_labels:" << pos_labels << endl;
  cout << "neg_labels:" << neg_labels << endl;
  cout << "pos_ranks:" << pos_ranks << endl;

  return auc;
}

double fast_exp_double(double y) {
  double d;
  *(reinterpret_cast<int*>(&d) + 0) = 0;
  *(reinterpret_cast<int*>(&d) + 1) = static_cast<int>(1512775 * y + 1072632447);
  return d;
}

float fast_exp_float(float y) {
  float d;
  *(reinterpret_cast<short*>(&d) + 0) = 0;
  *(reinterpret_cast<short*>(&d) + 1) = static_cast<short>(184 * y + (16256-7));
  return d;
}

int main()
{
    int rows = 1;
    int cols = 8;
    Matrix m(rows,cols);
    Matrix n(rows,cols);
    for (int i=0;i<rows;i++)
        for(int j=0;j<cols;j++)
        {
            m(i,j) = j/10.0;
            n(i,j) = j/10.0;
        }
    //m<< 1,2,3,4,
    //    5,6,7,8,
    //    9,8,11,12,
    //    13,14,15,16;
    //cout<<"Block in the middle"<<endl;
    //cout<<m.block<2,2>(1,1)<<endl<<endl;
    //for(int i = 1;i <= 3;++i)
    //{
    //    cout<<"Block of size "<<i<<"x"<<i<<endl;
    //    cout<<m.block(0,0,i,i)<<endl<<endl;
    //}
    //MatrixXf n(50000,4);
    //n<< 81,82,83,84,
    //    85,86,87,88,
    //    89,18,111,112,
    //    113,114,115,116;
    cout<<"befor update"<<endl;
    cout<<m(0,7)<<endl;
    cout<<n(0,7)<<endl;
    cout<<m.block(0,0,1,8)<<endl;
    cout<<n.block(0,0,1,8)<<endl;
    
    Matrix mm(1,1);
    Matrix nn(1,1);
    mm(0,0) = -0.0676251;
    nn(0,0) = 0.473591;
    Vector::AlignedMapType sparse_emb_vec(mm.data(), 1);
    Vector::ConstAlignedMapType grad_sparse_emb_vec(nn.data(),1);

    cout<< "sparse_emb_vec:" << sparse_emb_vec<<endl;
    cout<< "grad_sparse_emb_vec:" << grad_sparse_emb_vec<<endl;
    cout<<"mm:" << mm <<endl;
    cout<<"nn:" << nn <<endl;

    sgd_update(sparse_emb_vec, grad_sparse_emb_vec);
    cout<< "sparse_emb_vec:" <<sparse_emb_vec<<endl;
    cout<<"mm:" << mm <<endl;
    cout<<"after update"<<endl;

    cout<<m.block(0,0,1,8)<<endl;
    cout<<n.block(0,0,1,8)<<endl;

    cout<<"after pruned"<<endl;
    cout<<m.block(0,0,1,8)<<endl;
    cout<<n.block(0,0,1,8)<<endl;

    Vector x(cols);
    for(int j=0;j<cols;j++)
    {
        x(j) = 20;
    }
    //update_emb(m,x,1,cols);
    //adam_update(sparse_emb_vec,grad_sparse_emb_vec);
    //sgd_update(sparse_emb_vec,grad_sparse_emb_vec);
   // cout<<"after update"<<endl;
    //cout<< "sparse_emb_vec:" <<sparse_emb_vec<<endl;
    //cout<<"m:" << m.block(0,0,1,8)<<endl;

    float min_value = 1e-6;
    float test = -0.1;
    if (abs(test) < min_value)
    {
        cout << abs(test) << " < " << min_value << endl;
    }
    else
    {
        cout << abs(test) << " > " << min_value << endl;
    }

    //string temp = "";
    //cout << "temp:" << stof(temp) << endl;
    std::unordered_map<uint64_t, Matrix> _store;
    _store[1] = m;
    _store[2] = n;
    cout << "_store size: " << _store.size() << endl;
    
    int rows2 = 1;
    //int cols2 = 7;
    int cols2 = 4;
    Matrix m2(rows2,cols2);
    Matrix n2(rows2,cols2);
    Matrix m3(0,cols2);
    cout << "m3 cols: " << m3.cols() << endl;
    
    //m2 << 0.8,0.7,0.5,0.5,0.5,0.5,0.3;
    //n2 << 1,1,0,0,1,1,0;
    m2 << 0.1, 0.4, 0.35, 0.8;
    n2 << 0, 0, 1, 1;
    //for (int i=0;i<rows2;i++)
    //    for(int j=0;j<cols2;j++)
    //    {
    //        m2(i,j) = j/10.0;
    //        if (j < 5)
    //          n2(i,j) = 0;
    //        else
    //          n2(i,j) = 1;
    //    }
    
    cout<<"before sort"<<endl;
    cout<<m2.block(0,0,rows2,cols2)<<endl;
    cout<<n2.block(0,0,rows2,cols2)<<endl;

    Matrix mn(2*rows2, cols2);
    mn << m2,
       n2;
    cout<<mn.block(0,0,2*rows2,cols2)<<endl;
    sorted_cols_by_head(mn);
    cout<<"after sort"<<endl;
    cout<<mn.block(0,0,2*rows2,cols2)<<endl;
    
    float auc = compute_auc(mn);
    cout << "auc:" << auc << endl;

    int x_ = 3;
    int y_ = 2;
    int z_ = 2;
    Matrix3D xyz;
    xyz.resize(x_);
    for(int i=0; i<x_; i++)
    {
      xyz[i].resize(y_);
      for(int j=0;j<y_; j++)
      {
        xyz[i][j].resize(z_);
        for(int k=0; k<z_; k++)
          xyz[i][j][k] = i+j+k;
      }
    }

    cout << "Matrix3D xyz inited:" << endl;
    for(int i=0; i<x_; i++)
    {
      for(int j=0;j<y_; j++)
      {
        for(int k=0; k<z_; k++)
          cout << i << " " << j << " " << k << ":" << xyz[i][j][k] << endl;;
      }
    }

    for(int i=0; i<x_; i++)
      xyz[i].resize(1);
    cout << "Matrix3D xyz resize:" << endl;
    for(int i=0; i<x_; i++)
    {
      for(int j=0;j<1; j++)
      {
        for(int k=0; k<z_; k++)
          cout << i << " " << j << " " << k << ":" << xyz[i][j][k] << endl;;
      }
    }
    Matrix u;
    cout << "u rows:"  << u.rows() << endl;
    cout << "u cols:"  << u.cols() << endl;
    u.resize(2,2);
    cout << "u:" << u << endl;
    u.block(0,0,1,2) = Matrix::Zero(1,2);
    cout << "u:" << u << endl;
    cout << "u row2:" << u.block(0,0,2,2) << endl;
    u.bottomRows(1) = Matrix::Constant(1,2,10.0);
    cout << "u:" << u << endl;
    cout << "u.log:" << u.array().log() << endl;
    //Vector v(2);
    //v << 0.1,0.2;
    //cout << "v:" << v << endl;
    //v << 0.2;
    //cout << "v:" << v << endl;
    //cout<< xyz.block(0,0,0,x_,y_,z_) <<endl;


    ParamMap emb_dict;
    vector<Vector> embs(5);
    int dim_sparse_emb = 3;
    cout << "init emb_dict:" << endl;
    for (int i=0;i<5;i++)
    {
      Vector& emb = embs[i];
      emb.resize(dim_sparse_emb); 
      for (int j=0; j<dim_sparse_emb;j++)
        emb(j) = i;
      string key = to_string(i);
      //embs.push_back(emb);
      cout << "key:" << key << " value: " << emb << endl;
      cout << "embs[" << i << "]: " << embs[i] << endl;
      //emb_dict[key] = &emb;
      emb_dict[key] = &(embs[i]);
    }

    cout << "orig emb_dict:" << endl;
    for (auto iter=emb_dict.begin(); iter!=emb_dict.end();iter++)
    {
      cout << iter->first << ":" << *(iter->second) << endl;
    }

    string key = "1";
    Vector* emb = emb_dict.at(key);
    cout << "key:" << key << " value:" << *emb << endl;
    (*emb)(0) = 100;

    cout << "new emb_dict:" << endl;
    for (auto iter=emb_dict.begin(); iter!=emb_dict.end();iter++)
    {
      cout << iter->first << ":" << *(iter->second) << endl;
    }

    for (int i=1; i<10;i++)
    {
      float x = i*3.1415926;
      double e1 = exp(x);
      double e2 = fast_exp_double(x);
      double e3 = fast_exp_float(x);
      cout << x << "\t" << e1 << "\t" << e2 << "\t" << e3 << "\t" << endl;
    }

    unordered_map<int, Vector> um_test;
    if (um_test.find(1) == um_test.cend())
    {
      um_test[1] = Vector::Constant(1,10);
    }
    for (auto& t: um_test)
      cout << t.first << ":" << t.second<< endl;

    for (int i=2; i<10; i++)
    {
      Vector v = Vector::Constant(i, i*10);
      um_test[i] = v;
    }
    for (auto& t: um_test)
      cout << t.first << ":" << t.second<< endl;
    read_exfile(ex_file);
    write_exfile(ex_file);
    read_exfile(ex_file);

    vector<thread> threadList;
    int workers = 10;
    for(int i = 0; i < workers; i++) {
      threadList.push_back(thread(run_worker, i));
    }
    cout<<"Wait for all the worker thread to finish"<<endl;
    for_each(threadList.begin(), threadList.end(), mem_fn(&thread::join));
    cout<<"All worker thread done, send finish signal to the server"<<endl;
    cout<<"Exiting from Main Thread"<<endl;

    char* user = getenv("USER");
    string user_str = user;
    cout << "USER:" << user_str << user_str.size() << endl;
    char *role = getenv("DMLC_ROLE");
    if (role == nullptr)
      cout << "role is nullptr" << endl; 
    string role_str =role;
    cout << "DMLC_ROLE:" << role_str << role_str.size() << endl;
}
