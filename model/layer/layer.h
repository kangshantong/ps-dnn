#ifndef SRC_LAYER_H_
#define SRC_LAYER_H_

#include <Eigen/Core>
#include <vector>
#include <map>
#include "../utils/utils.h"
#include "../optimizer/optimizer.h"

enum LayerType {
    DENSEINPUT=0, 
    SPARSEINPUT, 
    FULLYCONNECTED, 
    SOFTMAX,
    SIGMOID,
    TANH,
    RELU,
    DOT
};

class Layer {
 protected:

  const Matrix* bottom;//如果单个输入的情况,bottom直接指向上一层的输出，节省一次copy；否则，现将输入收集到merge_bottom中，然后将bottom指向merge_bottom；
  Matrix merge_bottom;//用于合并多个输入
  Matrix top;  // layer output

  Matrix grad_bottom;  //本层整体需要回传的梯度,如果是单个输入，上层的grad_top直接指向grad_bottom；否则，先将grad_bottom分割为sub_grad_bottom，然后在分别为各个输入层指定grad_top
  vector<Matrix> sub_grad_bottom;  // 用于多个输入的情况，将grad_bottom进行分割
  const Matrix* grad_top;//如果连接到了一个输出层，grad_top直接指向下一层的grad_bottom或者对应的sub_grad_bottom中的元素，节省一次copy；
  //Matrix merge_grad_top;//用户合并多个输出的梯度,暂时只支持每个层只能连接到一个输出层

  LayerType layer_type;
  int layer_index;
  vector<Layer*> input_layers;
  Layer* output_layer;

  string prefix = "l";
  string delimiter = "_";

 public:
  virtual ~Layer() {}

  virtual void add_input(Layer* input) {input_layers.push_back(input);}
  virtual void set_output(Layer* output) {this->output_layer=output;}

  virtual void set_grad_top(const Matrix* grad_top) {this->grad_top = grad_top;}
  //virtual void set_merge_grad_top(const Matrix merge_grad_top) {this->merge_grad_top=merge_grad_top; grad_top=&(this->merge_grad_top);}

  virtual void gather_inputs();

  virtual void split_grad_bottom();

  virtual void forward() {cerr << "This function shouldn't be called!!!" << endl; exit(1);}

  virtual void backward() {cerr << "This function shouldn't be called!!!" << endl; exit(1);}

  virtual void update(Optimizer* opt) {}

  virtual const Matrix& output() { return top; }

  virtual const Matrix& back_gradient() { return grad_bottom; }

  virtual const Matrix* input_gradient() { return grad_top; }

  virtual LayerType get_layer_type() { return layer_type; }

  virtual int get_layer_index() { return layer_index; }

  virtual int output_dim() { return top.rows(); }

  virtual int output_batch_size() { return top.cols(); }

  virtual ParamInfo get_parameters() { return ParamInfo(); }

  virtual ParamMap get_gradient() { return ParamMap();}

  virtual void print_gradient() {}

  virtual void print_parameters_and_values() {}

  virtual ParamMap get_parameters_and_values() { return ParamMap(); }

  virtual void set_parameters_and_values(const ParamMap& param_and_values) {}
};

#endif  // SRC_LAYER_H_
