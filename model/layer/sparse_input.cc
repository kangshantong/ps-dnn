#include "./sparse_input.h"

void SparseInput::forward(const Matrix3D& sparse_bottom) {
  // z= x_sparse
  debug << "SparseInput::forward Start" << endl;
  const int n_sample = sparse_bottom[0].size();
  if (top.cols() != n_sample)
    top.resize(dim_sparse*dim_sparse_emb, n_sample);

  debug << "n_sample:" << n_sample << endl;
  debug << "dim_sparse:" << dim_sparse << endl;
  debug << "sparse_bottom cols:" << sparse_bottom.size() << endl;
  for (int i=0; i<n_sample; i++)
    for (int j=0; j<dim_sparse; j++)
    {
        debug << "start process:" << i << " " << j << endl;
        int sparse_value_count = sparse_bottom[j][i].size();
        //if (i==0 && j==21)
        {
          debug << "i-" << i << " j-" << j << endl;
          debug << "sparse_value_count: " << sparse_value_count << endl;
        }
        Vector emb = Vector::Zero(dim_sparse_emb);
        for (int k=0; k<sparse_value_count; k++)
        {
          string sparse_fea_index = sparse_bottom[j][i][k];
          if (i==0 && j==21)
          {
            debug <<  "k-" << k << endl;
            debug << "sparse_fea_index: " << sparse_fea_index << endl;
          }
          Vector fea_emb = EmbDict::get()->get_emb(sparse_fea_index);
          emb += fea_emb;
          if (emb.size() != fea_emb.size())
          {
            cerr << "Error::emb.size() != fea_emb.size()" << endl;
            cerr << emb.size() << " vs " << fea_emb.size() << endl; 
          }
          //if (i==0 && j == 21)
          {
            debug <<  "sparse fea emb: " << fea_emb << endl;
            debug <<  "new emb: " <<  emb << endl;
          }
        }
        for (int l=0; l<dim_sparse_emb; l++)
          // 进行平均
          top(j*dim_sparse_emb+l, i) = emb(l)/sparse_value_count;
          //top(j*dim_sparse_emb+l, i) = emb(l);

        debug << "end process:" << i << " " << j << endl;
    }
  debug << "SparseInput top: \t" << top << endl;
  debug << "SparseInput:: forward END" << endl;
}

void SparseInput::backward(const Matrix3D& sparse_bottom) {
  debug << "SparseInput::backward Start" << endl;
  debug << "grad_top size:" << grad_top->rows() << "*" << grad_top->cols() << endl;
  debug << "grad_top:" << *grad_top << endl;
  const int n_sample = sparse_bottom[0].size();
  // d(L)/d(w') = d(L)/d(z) * x'
  // d(L)/d(b) = \sum{ d(L)/d(z_i) }
  // !!!! grad_sparse_emb should be cleard
  grad_sparse_emb.clear();
  for (int i=0; i<n_sample;i++)
    for (int j = 0; j<dim_sparse;j++)
    {
        int sparse_value_count = sparse_bottom[j][i].size();
        if (i==0 && j == 21)
        {
          debug << "i-" << i << " j-" << j << endl;
          debug << "sparse_value_count: " << sparse_value_count << endl;
        }
        for (int k=0; k<sparse_value_count; k++)
        {
          string sparse_fea_index = sparse_bottom[j][i][k];
          if (i==0 && j == 21)
          {
            debug <<  "k-" << k << endl;
            debug << "sparse_fea_index: " << sparse_fea_index << endl;
          }
          Vector& grad_sparse_emb_ele = grad_sparse_emb[sparse_fea_index];
          if (i==0 && j == 21)
          {
            debug << "before grad_sparse_emb_ele: " << grad_sparse_emb_ele << endl;
          }
          if (grad_sparse_emb_ele.size() == 0) {
            grad_sparse_emb_ele.resize(dim_sparse_emb);
            grad_sparse_emb_ele.setZero();
          }
          for (int l=0;l<dim_sparse_emb;l++)
          {
            if (i==0 && j == 21)
            {
              debug << "(*grad_top)(j*dim_sparse_emb+l,i):" << (*grad_top)(j*dim_sparse_emb+l,i) << endl;
            }
            grad_sparse_emb_ele(l) += ((*grad_top)(j*dim_sparse_emb+l,i) / sparse_value_count); //稀疏特征的向量，为多个稀疏特征值对应embedding的平均值,所以在计算梯度时，要处以稀疏特征值的个数
            //grad_sparse_emb_ele(l) += ((*grad_top)(j*dim_sparse_emb+l,i)); //不除以稀疏特征值的个数
          }
          if (i==0 && j == 21)
          {
            debug << "after grad_sparse_emb_ele: " << grad_sparse_emb_ele << endl;
          }
        }
    }
  //grad_bottom.resize(dim_dense+dim_sparse*dim_sparse_emb, n_sample);
  //grad_bottom = grad_top;

  debug << "grad_sparse_emb:\t" << endl;
  for (auto iter=grad_sparse_emb.begin();iter!=grad_sparse_emb.end(); iter++)
  {
    debug << iter -> first << ":" << iter -> second << endl;
    //debug << "orig:\t" << iter -> second << endl;
    // string key = iter -> first;
    // Vector& grad_sparse_emb_ele = grad_sparse_emb[key];
    // embedding梯度过小，此处进行放大
    // grad_sparse_emb_ele = grad_sparse_emb_ele * n_sample;
    // debug << "new:\t" << iter -> second << endl;
  }
  debug << "SparseInput::backward End" << endl;
}

void SparseInput::update(Optimizer* opt) {
  debug << "SparseInput::update Start" << endl;
  for (auto& iter : grad_sparse_emb)
  {
    string param = iter.first;
    //!!! The params of update need to be continuous
    EmbDict::get()->update_emb(param, iter.second, opt);
  }
  debug << "SparseInput::update End" << endl;
}

ParamInfo SparseInput::get_parameters(const Matrix3D& sparse_bottom) {
  //收集本次batch参与的稀疏特征
  ParamInfo res;
  const int n_sample = sparse_bottom[0].size();
  for (int i=0; i<n_sample; i++)
    for (int j=0; j<dim_sparse;j++)
    {
      int sparse_value_count = sparse_bottom[j][i].size();
      for (int k=0; k<sparse_value_count; k++)
      {
        string sparse_fea_index = sparse_bottom[j][i][k];
        res[sparse_fea_index] = dim_sparse_emb;

      }
    }
  
  return res;
}

ParamMap SparseInput::get_gradient() {
  return grad_sparse_emb;
}

void SparseInput::print_gradient() {
  cout << "layer_index:" << layer_index << endl;
  cout << "layer_type:" << layer_type << endl;

  cout << "emb gradient" << endl;
  for (auto& iter : grad_sparse_emb)
  {
    cout << iter.first << ":" << iter.second << endl;
  }
  cout << endl;
}

