#include "conf.h"

Loss* gen_loss(string loss_conf)
{
  if (loss_conf == "cross_entropy")
    return new CrossEntropy;
  else if (loss_conf == "mse")
    return new MSE;
  else
  {
    cerr << "Error: unknown loss " << loss_conf << endl;
    exit(1);
  }
  return nullptr;
}

Optimizer* gen_opt(float learning_rate, string opt_conf)
{
  if (opt_conf == "sgd")
    return new SGD(learning_rate, 1e-5, 0.0, false);
  else if (opt_conf == "momentum") 
    return new SGD(learning_rate, 5e-4, 0.9, false);
  else if (opt_conf == "nesterov") 
    return new SGD(learning_rate, 5e-4, 0.9, true);
  else
    return new ADAM(learning_rate, 5e-5);
}
