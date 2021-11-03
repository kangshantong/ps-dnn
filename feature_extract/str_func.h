#ifndef STR_FUNC_H
#define STR_FUNC_H

#include <cstring>
#include <vector>

using namespace std;
vector<string> split(const string str, const string delimiter)
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

string vector2str(vector<string> v, string delimiter)
{
    std::string s;
    int length = v.size(), i = 0;
    for (const auto &piece : v) 
    {
        s += piece;
        if (i != length - 1)
            s += delimiter;
        i++;
    }
    return s;
}
//替换1 全部替换1次  12212 替换12为21------>21221
string& replace_all_distinct(string& str, const string& old_value, const string& new_value)
{
  string::size_type pos=0;
  while((pos=str.find(old_value,pos))!= string::npos)
  {
    str=str.replace(pos,old_value.length(),new_value);
    if(new_value.length()>0)
    {
      pos+=new_value.length();
    }
  }
  return str;

}

//替换2  循环替换，替换后的值也检查并替换 12212 替换12为21----->22211
string& replace_all(string& str, const string& old_value, const string& new_value)
{
  string::size_type pos=0;
  while((pos=str.find(old_value))!= string::npos)
  {
    str=str.replace(str.find(old_value),old_value.length(),new_value);
  }
  return str;
}
#endif
