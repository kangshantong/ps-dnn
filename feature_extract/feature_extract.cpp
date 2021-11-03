#include "feature_extract.h"

float FeatureExtractor::LogOperator(const string fea_name)
{
    check_fea(fea_name);

    string orig_fea_value = sample_fields[fea_name];
    if (orig_fea_value.empty())
    {
        return 0;
    }
    float res = log(stof(orig_fea_value) + 1.0);
    return res;
}

float FeatureExtractor::SqrtOperator(const string fea_name)
{
    check_fea(fea_name);

    string orig_fea_value = sample_fields[fea_name];
    if (orig_fea_value.empty())
    {
        return 0;
    }
    float res = sqrt(stof(orig_fea_value) + 1.0);
    return res;
}

string FeatureExtractor::DirectOperator(const string fea_name)
{
    check_fea(fea_name);

    string orig_fea_value = sample_fields[fea_name];

    return orig_fea_value;
}

string FeatureExtractor::BucketOperator(const string fea_name, string splits)
{
    check_fea(fea_name);

    string orig_fea_value = sample_fields[fea_name];
    if (orig_fea_value.empty())
    {
        return "-1";
    }
    float orig_value = stof(orig_fea_value);
    vector<string> breaklines = split(splits, input_conf_inner_delimiter);
    if (breaklines.size() <= 0)
    {
        cout << fea_name << ": bucket parameter is empty" << endl; 
        exit(1);
    }
    int output = breaklines.size();
    for (int i = 0; i < breaklines.size(); ++i)
    {
        float breakline = stof(breaklines[i]);
        if (orig_value > breakline)
        {
            continue;
        }
        else
        {
            output = i;
            break;
        }

    }

    return to_string(output);
}

string FeatureExtractor::CombineOperator(const string sub_fea_names, string sub_operators, string sub_args)
{
    vector<string> orig_sub_fea_names = split(sub_fea_names, input_conf_inner_delimiter);
    vector<string> orig_sub_operators = split(sub_operators, input_conf_inner_delimiter);
    vector<string> orig_sub_args = split(sub_args, input_conf_args_delimiter);

    if (orig_sub_fea_names.size() != orig_sub_operators.size() || orig_sub_fea_names.size() != orig_sub_args.size())
    {
        cerr << "Error:The size of sub_fea_names does't match sub_operators or sub_args:" << sub_fea_names << endl;
        cerr << sub_operators << endl;
        cerr << sub_args << endl;
        exit(1);
    }

    vector<string> sub_fea_values;
    for (int i = 0; i < orig_sub_fea_names.size(); ++i)
    {
        string sub_fea_name = orig_sub_fea_names[i];
        string sub_operator = orig_sub_operators[i];
        string sub_conf = orig_sub_args[i];

        if (sub_operator == "direct")
        {
            sub_fea_values.push_back(DirectOperator(sub_fea_name));
        }
        else if (sub_operator == "bucket")
        {
            sub_fea_values.push_back(BucketOperator(sub_fea_name,sub_conf));
        }
        else
        {
            cerr << "Error:Combine only support direct and bucket" << endl;
            cerr << sub_fea_names << ":" << sub_operators << endl;
            exit(1);
        }
    }

    return vector2str(sub_fea_values, output_fea_inner_delimiter);
}

string FeatureExtractor::GroupOperator(const string fea_name)
{
    check_fea(fea_name);
    string orig_fea_value = sample_fields[fea_name];
    if (orig_fea_value.empty())
    {
        return "";
    }

    string res = replace_all_distinct(orig_fea_value, input_sample_inner_delimiter, output_fea_intermediate_delimiter);
    
    return res;
}

string FeatureExtractor::HitOperator(const string sub_fea_names)
{
    vector<string> orig_sub_fea_names = split(sub_fea_names, input_conf_inner_delimiter);
    if (orig_sub_fea_names.size() != 2)
    {
        cerr << "Error:Hit Operator needs two sub feas:" << sub_fea_names << endl;
        exit(1);
    }
    string first_fea_name = orig_sub_fea_names[0];
    string second_fea_name = orig_sub_fea_names[1];
    check_fea(first_fea_name);
    check_fea(second_fea_name);

    string first_fea_value = sample_fields[first_fea_name];
    vector<string> second_fea_values = split(sample_fields[second_fea_name], input_sample_inner_delimiter);

    string res = "1";
    if (find(second_fea_values.begin(), second_fea_values.end(), first_fea_value) == second_fea_values.end())
      res = "0";
    
    return res;
}

int FeatureExtractor::ParseSample(const string input)
{
    vector<string> eles = split(input, input_sample_outer_delimiter);
    vector<string> fields = split(conf_["raw_sample_schema"]["fields"].as<std::string>(),input_conf_outer_delimiter);
    if (eles.size() != fields.size())
    {
        cerr << "Error:The size of sample fields does't match the schema:" << input << endl;
        cerr << conf_["raw_sample_schema"]["fields"].as<std::string>() << endl;
        cerr << eles.size() << " vs " << fields.size() << endl;
        exit(1);
    }
    for (int i = 0; i < eles.size(); ++i)
    {
        string field_name = fields[i];
        string field_value = eles[i];
        sample_fields[field_name] = field_value;
        // cout << field_name << " : " << field_value << endl;
    }
    
    return 0;
}

string FeatureExtractor::ExtractSingleFeaure(string fea_name, string fea_conf)
{
    vector<string> fea_confs = split(fea_conf, input_conf_outer_delimiter);
    if (fea_confs.size() < 2)
    {
        cerr << "Error:Fea conf needs incude sub fea and operator:" << fea_name << "\t" << fea_conf << endl;
        exit(1);
    }

    string fea_prefix = fea_name;
    string orig_fea_names = fea_confs[0];
    string fea_operator = fea_confs[1];

    string fea_value;
    if (fea_operator == "log")
        fea_value = to_string(LogOperator(orig_fea_names));
    else if (fea_operator == "sqrt")
        fea_value = to_string(SqrtOperator(orig_fea_names));
    else if (fea_operator == "direct")
    {
        fea_prefix = orig_fea_names;
        fea_value = DirectOperator(orig_fea_names);
    }
    else if (fea_operator == "bucket")
    {
        if (fea_confs.size() < 3)
        {
            cerr << "Error:Bucket operator needs sub fea, operator and breaklines:" << fea_name << "\t" << fea_conf << endl; 
            exit(1);
        }
        fea_value = BucketOperator(orig_fea_names,fea_confs[2]);
    }
    else if (fea_operator == "combine")
    {
        if (fea_confs.size() < 4)
        {
            cerr << "Error:Combine operator needs sub feas,operator,sub operators and sub confs:" << fea_name << "\t" << fea_conf << endl; 
            exit(1);
        }
        fea_value = CombineOperator(orig_fea_names, fea_confs[2], fea_confs[3]);
    }
    else if (fea_operator == "group")
    {
        if (fea_confs.size() >= 3)
          fea_prefix = fea_confs[2];
        fea_value = GroupOperator(orig_fea_names);
    }
    else if (fea_operator == "hit")
        fea_value = HitOperator(orig_fea_names);
    else
    {
        cerr << "unkown operator:" << fea_operator << endl;
        exit(1);
    }

    //vector<string> orig_sub_fea_names = split(orig_fea_names, input_conf_inner_delimiter);
    ////子特征只有一个，则将特征值设置为子特征名和子特征值，以便进行embedding共享。譬如品牌A和历史访问的品牌A特征是一个。 
    //if (orig_sub_fea_names.size() == 1)
    //  fea_prefix = orig_sub_fea_names[0];
    
    //特征抽取为空
    if (fea_value.size() == 0)
      return fea_prefix + output_fea_inner_delimiter + fea_value;

    vector<string> sub_fea_values = split(fea_value, output_fea_intermediate_delimiter);
    vector<string> sub_fea_values_with_name;

    for (int i = 0; i < sub_fea_values.size(); ++i)
    {
        string sub_fea_value = sub_fea_values[i];
        string sub_fea_value_with_name = fea_prefix + output_fea_inner_delimiter + sub_fea_value;
        sub_fea_values_with_name.push_back(sub_fea_value_with_name);
    }
    string res =  vector2str(sub_fea_values_with_name, output_fea_intermediate_delimiter);
    return res;
}

string FeatureExtractor::ExtractGroupFeaure(string fea_group_name)
{
    vector<string> fea_values;
    std::map<std::string, IniField>::iterator iter;
    int fea_counts = conf_[fea_group_name].size();
    if (fea_counts <= 0)
    {
        //cout << fea_group_name << " has no features" << endl;
        return "";
    }

    for(iter = conf_[fea_group_name].begin(); iter != conf_[fea_group_name].end(); iter++) 
    {
        string fea_name = iter->first;
        string fea_conf = (iter->second).as<std::string>();
        fea_values.push_back(ExtractSingleFeaure(fea_name, fea_conf));
    }

    string res =  vector2str(fea_values, output_fea_outer_delimiter);
    vector<string> res_list = split(res, input_conf_outer_delimiter);
    if  (fea_counts != res_list.size())
    {
        cerr << "Error::Feas needed != Feas generated, maybe comma in sample "<< endl;
        cerr << res << endl;
        res = "";
        throw("Feas needed != Feas generated!");
    }
    return res;

}

int FeatureExtractor::LoadSparseFeaDict(const string& spart_fea_dict_file)
{
    std::ifstream is(spart_fea_dict_file.c_str());
    if(!is)
    {
        std::cerr << "Error::spart_fea_dict_file File " << spart_fea_dict_file << " doesn't exist" << std::endl; 
        exit(1);
    }

    while(!is.eof() && !is.fail())
    {
        std::string line;
        std::getline(is, line, '\n');

        // skip if line is empty
        if(line.size() == 0)
            continue;

        vector<string> fields = split(line, input_sample_outer_delimiter);
        // skip if line fields is less than 2
        if (fields.size() < 2)
            continue;

        string fea_value = fields[0];
        string index = fields[1];
        sparse_fea_dict_[fea_value] = index;
    }

    return 0;
}

string FeatureExtractor::FeatureExtractStage1(const string& raw_sample)
{
    vector<string> intermediate_sample;

    ParseSample(raw_sample);
    string id = ExtractSingleFeaure("id", conf_["id"]["id"].as<std::string>());
    string label = ExtractSingleFeaure("label", conf_["label"]["label"].as<std::string>());

    intermediate_sample.push_back(id);
    intermediate_sample.push_back(label);

    try
    {
        intermediate_sample.push_back(ExtractGroupFeaure("dense_features_user"));
        intermediate_sample.push_back(ExtractGroupFeaure("dense_features_ad"));
        intermediate_sample.push_back(ExtractGroupFeaure("dense_features_user_ad"));

        intermediate_sample.push_back(ExtractGroupFeaure("sparse_features_user"));
        intermediate_sample.push_back(ExtractGroupFeaure("sparse_features_ad"));
        intermediate_sample.push_back(ExtractGroupFeaure("sparse_features_user_ad"));
    }
    catch(const char *msg)
    {
        return "";
    }

    return vector2str(intermediate_sample, output_fea_group_delimiter);
}

string FeatureExtractor::FeatureExtractStage2ForDense(const string& intermediate_sample_dense)
{
  vector<string> orig_dense_features_vec = split(intermediate_sample_dense, output_fea_outer_delimiter);
  vector<string> dense_value_vec;
  for (int i=0; i<orig_dense_features_vec.size();i++)
  {
    dense_value_vec.push_back(extract_field(orig_dense_features_vec[i], output_fea_inner_delimiter, 1));
  }
  return vector2str(dense_value_vec, output_fea_outer_delimiter);
}

string FeatureExtractor::FeatureExtractStage2ForSparse(const string& intermediate_sample_sparse)
{
  vector<string> raw_sparse_features = split(intermediate_sample_sparse, output_fea_outer_delimiter);
  vector<string> hash_sparse_features;
  if(sparse_fea_dict_.size() == 0)
  {
    cerr << "Error::sparse_fea_dict is empty!!!" << endl;
    exit(1);
  }
  for (int i = 0; i < raw_sparse_features.size(); ++i)
  {
    vector<string> raw_sparse_feature_vec = split(raw_sparse_features[i],output_fea_intermediate_delimiter);
    vector<string> hash_sparse_feature_vec;
    for (int j=0; j<raw_sparse_feature_vec.size();j++)
    {
      string raw_sparse_feature = raw_sparse_feature_vec[j]; 
      if (sparse_fea_dict_.find(raw_sparse_feature) != sparse_fea_dict_.end())
        hash_sparse_feature_vec.push_back(sparse_fea_dict_[raw_sparse_feature]);
      else
        //默认特征值index为0, skip
        //hash_sparse_feature_vec.push_back("0");
        continue;
    }
    // 如果sparse_fea_values中只有0，则只保留一个0
    if (hash_sparse_feature_vec.size() == 0)
      hash_sparse_feature_vec.push_back("0");

    hash_sparse_features.push_back(vector2str(hash_sparse_feature_vec, output_fea_intermediate_delimiter));
  }
  return vector2str(hash_sparse_features, output_fea_outer_delimiter);
}

string FeatureExtractor::FeatureExtractStage2(const string& intermediate_sample)
{
    vector<string> final_sample;

    vector<string> fields = split(intermediate_sample, output_fea_group_delimiter);
    string id = fields[0];
    string label = extract_field(fields[1],output_fea_inner_delimiter, 1);
    final_sample.push_back(id);
    final_sample.push_back(label);

    string intermediate_sample_dense_user = fields[2];
    string intermediate_sample_dense_ad = fields[3];
    string intermediate_sample_dense_user_ad = fields[4];
    string intermediate_sample_sparse_user = fields[5];
    string intermediate_sample_sparse_ad = fields[6];
    string intermediate_sample_sparse_user_ad= fields[7];

    try
    {
        final_sample.push_back(FeatureExtractStage2ForDense(intermediate_sample_dense_user));
        final_sample.push_back(FeatureExtractStage2ForDense(intermediate_sample_dense_ad));
        final_sample.push_back(FeatureExtractStage2ForDense(intermediate_sample_dense_user_ad));

        final_sample.push_back(FeatureExtractStage2ForSparse(intermediate_sample_sparse_user));
        final_sample.push_back(FeatureExtractStage2ForSparse(intermediate_sample_sparse_ad));
        final_sample.push_back(FeatureExtractStage2ForSparse(intermediate_sample_sparse_user_ad));
    }
    catch(const char *msg)
    {
        return "";
    }

    return vector2str(final_sample, output_fea_group_delimiter);
}

int fea_extract_stage1(char c_ini_file[], char c_raw_sample[], char* buf,int *len)
{
    string raw_sample(c_raw_sample);
    string ini_file(c_ini_file);

    static FeatureExtractor fe(ini_file);
    string fea_value_sample = fe.FeatureExtractStage1(raw_sample);
    if (fea_value_sample.length() > 81920)
    {
        cerr << "Error: fea_value_sample length is bigger than 80KB" << endl;
        cerr << fea_value_sample << endl;
        *len = 0;
        return *len;
    }
    memcpy(buf, fea_value_sample.c_str(), strlen(fea_value_sample.c_str()));
    *len = fea_value_sample.length();
    return 0;
}

int fea_extract_stage2(char c_ini_file[], char c_sparse_dict_file[], 
        char c_intermediate_sample[], char* buf,int *len)
{
    string intermediate_sample(c_intermediate_sample);
    string ini_file(c_ini_file);

    string sparse_dict_file(c_sparse_dict_file);

    static FeatureExtractor fe(ini_file,sparse_dict_file);
    string fea_value_sample = fe.FeatureExtractStage2(intermediate_sample);

    if (fea_value_sample.length() > 81920)
    {
        cerr << "Error:fea_value_sample length is bigger than 80KB" << endl;
        cerr << fea_value_sample << endl;
        *len = 0;
        return *len;
    }
    memcpy(buf, fea_value_sample.c_str(), strlen(fea_value_sample.c_str()));
    *len = fea_value_sample.length();
    return 0;
}

int main(int argc,char *argv[])
{
  for(int i=0; i<argc; i++)
  {
    cout << "argument[" << i << "] is: " << argv[i] << endl;
  }

  int phrase = atoi(argv[1]);
  string version = argv[2];
  string fea_conf = "../conf/features_" + version + ".ini";
  cout << "fea_conf:" << fea_conf << endl;
  if (phrase == 1)
  { 
    FeatureExtractor fe(fea_conf);

    ifstream inf;
    inf.open("./data/demo");

    ofstream outf;
    outf.open("./data/demo_out_stage1");

    string sample;
    while( getline(inf,sample ) ){    
      cout << "raw sample:\t" << sample  << endl;
      string res = fe.FeatureExtractStage1(sample);
      cout << "intermediate fea_extracted:\t" << res  << endl;           
      outf << res << endl;

    }

    inf.close();
    outf.close();
  }
  else if (phrase == 2)
  {
    FeatureExtractor fe(fea_conf, "./data/fea_dict_index");

    ifstream inf;
    inf.open("./data/demo_out_stage1");

    ofstream outf;
    outf.open("./data/demo_out_stage2");

    string sample;
    while( getline(inf,sample ) ){    
      cout << "intermediate sample:\t" << sample  << endl;
      string res = fe.FeatureExtractStage2(sample);
      cout << "final fea_extracted:\t" << res  << endl;           
      outf << res << endl;

    }

    inf.close();
    outf.close();
  }
  else
  {
    cerr << "unknown order" << endl;
  }
  return 0;
}
