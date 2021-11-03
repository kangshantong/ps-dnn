#ifndef SRC_CONF_H_
#define SRC_CONF_H_

#include "../loss/cross_entropy_loss.h"
#include "../loss/mse_loss.h"
#include "../optimizer/sgd.h"
#include "../optimizer/adam.h"

Loss* gen_loss(string loss_conf);

Optimizer* gen_opt(float learning_rate, string opt_conf);

#endif  // SRC_CONF_H_
