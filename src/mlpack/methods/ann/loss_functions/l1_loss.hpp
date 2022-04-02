/**
 * @file methods/ann/loss_functions/l1_loss.hpp
 * @author Himanshu Pathak
 *
 * Definition of the L1 Loss function.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_LOSS_FUNCTION_L1_LOSS_HPP
#define MLPACK_METHODS_ANN_LOSS_FUNCTION_L1_LOSS_HPP

#include <mlpack/prereqs.hpp>

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

/**
 * The L1 loss is a loss function that measures the mean absolute error (MAE) 
 * between each element in the input x and target y 
 *
 * @tparam MatType Matrix representation to accept as input and use for
 *    computation.
 */
template<typename MatType = arma::mat>
class L1LossType
{
 public:
  /**
   * Create the L1LossType object.
   *
   * @param mean Reduction type. If true, it returns the mean of 
   * the loss. Else, it returns the sum.
   */
  L1LossType(const bool mean = true);

  /**
   * Computes the L1 Loss function.
   *
   * @param prediction Predictions used for evaluating the specified loss
   *     function.
   * @param target The target vector.
   */
  typename MatType::elem_type Forward(const MatType& prediction,
                                      const MatType& target);

  /**
   * Ordinary feed backward pass of a neural network.
   *
   * @param prediction Predictions used for evaluating the specified loss
   *     function.
   * @param target The target vector.
   * @param loss The calculated error.
   */
  void Backward(const MatType& prediction,
                const MatType& target,
                MatType& loss);

  //! Get the value of reduction type.
  bool Mean() const { return mean; }
  //! Set the value of reduction type.
  bool& Mean() { return mean; }

  /**
   * Serialize the layer.
   */
  template<typename Archive>
  void serialize(Archive& ar, const uint32_t /* version */);

 private:
  //! Reduction type. If true, performs mean of loss else sum.
  bool mean;
}; // class L1LossType

// Default typedef for typical `arma::mat` usage.
typedef L1LossType<arma::mat> L1Loss;

} // namespace ann
} // namespace mlpack

// Include implementation.
#include "l1_loss_impl.hpp"

#endif
