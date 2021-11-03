/*
 * dataset.h
 *
 * Created on: 13 Feb 2021
 *     Author: RandomKang
 */

#ifndef SRC_DATASET_H_
#define SRC_DATASET_H_

#include <fstream>
#include <iostream>
#include <string>
#include "../utils/utils.h"

class Dataset {
 private:
  std::string train_file_;
  std::string test_file_;
  std::ifstream train_stream_;
  std::ifstream test_stream_;

  int dim_dense_user_;
  int dim_dense_ad_;
  int dim_dense_user_ad_;
  int dim_sparse_user_;
  int dim_sparse_ad_;
  int dim_sparse_user_ad_;

  string output_fea_inner_delimiter_ = "#";
  string output_fea_intermediate_delimiter_ = "|";
  string output_fea_outer_delimiter_ = ",";
  string output_fea_group_delimiter_ = "\t";

 public:
  int read_dense_data(string& sample_dense, Matrix& dense_data, int dim_dense, int index);
  void resize_sparse_data(Matrix3D& sparse_data, int dim_sparse, int needs);
  int read_sparse_data(string& sample_sparse, Matrix3D& sparse_data, int dim_sparse, int index);
  int read_dataset_data(std::ifstream& file, Matrix& dense_user, Matrix& dense_ad, Matrix& dense_user_ad,
      Matrix3D& sparse_user, Matrix3D& sparse_ad, Matrix3D& sparse_user_ad, 
      Matrix& labels, int needs);

  int read_train_data(Matrix& dense_user, Matrix& dense_ad, Matrix& dense_user_ad,
      Matrix3D& sparse_user, Matrix3D& sparse_ad, Matrix3D& sparse_user_ad, 
      Matrix& labels, int needs);
  int read_test_data(Matrix& dense_user, Matrix& dense_ad, Matrix& dense_user_ad,
      Matrix3D& sparse_user, Matrix3D& sparse_ad, Matrix3D& sparse_user_ad, 
      Matrix& labels, int needs);

  void set_train_file(const std::string& train_file);
  void set_test_file(const std::string& test_file);
  void set_fea_dim(int dense_dim_user, int dense_dim_ad, int dense_dim_user_ad,
      int sparse_dim_user, int sparse_dim_ad, int sparse_dim_user_ad);

  explicit Dataset() {} 
};

#endif  // SRC_DATASET_H_
