#ifndef FEATURE_EXTRACT_H
#define FEATURE_EXTRACT_H

#include <cstring>
#include <vector>
#include <math.h>
#include <algorithm>
#include <map>
#include <stdexcept>
#include "../utils/ini.h"
#include "str_func.h"

using namespace std;

class FeatureExtractor
{
private:
    IniFile conf_;
    map<string, string> sample_fields;
    map<string, string> sparse_fea_dict_;

    string input_conf_outer_delimiter = ",";
    string input_conf_inner_delimiter = "|";
    string input_conf_args_delimiter = "#";

    string input_sample_outer_delimiter = "\t";
    string input_sample_inner_delimiter = ",";
    string output_fea_inner_delimiter = "#";
    string output_fea_intermediate_delimiter = "|";
    string output_fea_outer_delimiter = ",";
    string output_fea_group_delimiter = "\t";

    float LogOperator(const string fea_name);
    float SqrtOperator(const string fea_name);
    string DirectOperator(const string fea_name);
    string BucketOperator(const string fea_name, string splits);
    string CombineOperator(const string sub_fea_names, string sub_operators, string sub_args);
    string GroupOperator(const string fea_name);
    string HitOperator(const string sub_fea_names);

    bool check_fea(const string fea_name)
    {
      if (sample_fields.find(fea_name) == sample_fields.end())
      {
        cerr << "wrong fea_name :\t" << fea_name <<  endl;
        exit(1);
      }
      return true;
    }
    /**********************************************************************
     * Feature extract
     *********************************************************************/
    int ParseSample(const string input);
    int LoadSparseFeaDict(const string& spart_fea_dict_file);

    string ExtractSingleFeaure(string fea_name, string fea_conf);
    string ExtractGroupFeaure(string fea_group_name);

public:
    FeatureExtractor(const IniFile &conf) : conf_(conf)
    {}

    FeatureExtractor(const string &ini_file) : conf_(ini_file)
    {
        cerr << "Init FeatureExtractor with ini file " << ini_file << endl;
    }

    FeatureExtractor(const string &ini_file, const string &spart_fea_dict_file) : conf_(ini_file)
    {
        cerr << "Init FeatureExtractor with ini file: " << ini_file << ", spart_fea_dict_file: " << spart_fea_dict_file << endl;
        if (!spart_fea_dict_file.empty())
            LoadSparseFeaDict(spart_fea_dict_file);
    }

    ~FeatureExtractor()
    {}

    string FeatureExtractStage1(const string& raw_sample);
    string FeatureExtractStage2ForDense(const string& intermediate_sample_dense);
    string FeatureExtractStage2ForSparse(const string& intermediate_sample_sparse);
    string FeatureExtractStage2(const string& intermediate_sample);
};

// 封装C接口
extern "C"{
int fea_extract_stage1(char c_ini_file[], char c_raw_sample[], char* buf,int *len);
int fea_extract_stage2(char c_ini_file[], char c_sparse_dict_file[], 
        char c_intermediate_sample[], char* buf,int *len);
}
#endif
