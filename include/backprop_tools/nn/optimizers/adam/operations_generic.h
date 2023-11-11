#include "../../../version.h"
#if (defined(BACKPROP_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(BACKPROP_TOOLS_NN_OPTIMIZERS_ADAM_OPERATIONS_GENERIC_H)) && (BACKPROP_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define BACKPROP_TOOLS_NN_OPTIMIZERS_ADAM_OPERATIONS_GENERIC_H

#include "adam.h"
#include "../../../nn/layers/dense/layer.h"
#include "../../../nn/parameters/operations_generic.h"
#include "../../../utils/polyak/operations_generic.h"

BACKPROP_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools{
    template <typename DEVICE, typename SPEC>
    void malloc(DEVICE& device, nn::parameters::Adam::instance<SPEC>& p){
        malloc(device, (nn::parameters::Gradient::instance<SPEC>&) p);
        malloc(device, p.gradient_first_order_moment);
        malloc(device, p.gradient_second_order_moment);
    }
    template <typename DEVICE, typename SPEC>
    void free(DEVICE& device, nn::parameters::Adam::instance<SPEC>& p){
        free(device, (nn::parameters::Gradient::instance<SPEC>&) p);
        free(device, p.gradient_first_order_moment);
        free(device, p.gradient_second_order_moment);
    }
    template<typename DEVICE, typename SPEC, typename PARAMETERS>
    void update(DEVICE& device, nn::parameters::Adam::instance<SPEC>& parameter, nn::optimizers::Adam<PARAMETERS>& optimizer) {
        utils::polyak::update(device, parameter.gradient, parameter.gradient_first_order_moment, PARAMETERS::BETA_1);
        utils::polyak::update_squared(device, parameter.gradient, parameter.gradient_second_order_moment, PARAMETERS::BETA_2);
        gradient_descent(device, parameter, optimizer);
    }

    template<typename DEVICE, typename SPEC, typename PARAMETERS>
    void gradient_descent(DEVICE& device, nn::parameters::Adam::instance<SPEC>& parameter, nn::optimizers::Adam<PARAMETERS>& optimizer){
        for(typename DEVICE::index_t row_i = 0; row_i < SPEC::CONTAINER::ROWS; row_i++) {
            for(typename DEVICE::index_t col_i = 0; col_i < SPEC::CONTAINER::COLS; col_i++) {
                typename SPEC::CONTAINER::T parameter_update = optimizer.alpha * optimizer.first_order_moment_bias_correction * get(parameter.gradient_first_order_moment, row_i, col_i) / (math::sqrt(device.math, get(parameter.gradient_second_order_moment, row_i, col_i) * optimizer.second_order_moment_bias_correction) + PARAMETERS::EPSILON);
                if constexpr(utils::typing::is_same_v<typename SPEC::CATEGORY_TAG, nn::parameters::categories::Biases> && PARAMETERS::BIAS_LR_FACTOR > 1){
                    parameter_update *= PARAMETERS::BIAS_LR_FACTOR;
                }
                if constexpr(utils::typing::is_same_v<typename SPEC::CATEGORY_TAG, nn::parameters::categories::Weights>){
                    if constexpr(utils::typing::is_same_v<typename SPEC::GROUP_TAG, nn::parameters::groups::Normal>){
                        parameter_update += get(parameter.parameters, row_i, col_i) * PARAMETERS::WEIGHT_DECAY / 2;
                    }
                    if constexpr(utils::typing::is_same_v<typename SPEC::GROUP_TAG, nn::parameters::groups::Input>  && PARAMETERS::WEIGHT_DECAY_INPUT > 0){
                        parameter_update += get(parameter.parameters, row_i, col_i) * PARAMETERS::WEIGHT_DECAY_INPUT / 2;
                    }
                    if constexpr(utils::typing::is_same_v<typename SPEC::GROUP_TAG, nn::parameters::groups::Output> && PARAMETERS::WEIGHT_DECAY_OUTPUT > 0){
                        parameter_update += get(parameter.parameters, row_i, col_i) * PARAMETERS::WEIGHT_DECAY_OUTPUT / 2;
                    }
                }
                increment(parameter.parameters, row_i, col_i, -parameter_update);
            }
        }
    }
    template<typename DEVICE, typename SPEC, typename PARAMETERS>
    void _reset_optimizer_state(DEVICE& device, nn::parameters::Adam::instance<SPEC>& parameter, nn::optimizers::Adam<PARAMETERS>& optimizer){
        set_all(device, parameter.gradient_first_order_moment, 0);
        set_all(device, parameter.gradient_second_order_moment, 0);
    }

    template<typename SOURCE_DEVICE, typename TARGET_DEVICE, typename SOURCE_SPEC, typename TARGET_SPEC>
    void copy(SOURCE_DEVICE& source_device, TARGET_DEVICE& target_device, const  nn::parameters::Adam::instance<SOURCE_SPEC>& source, nn::parameters::Adam::instance<TARGET_SPEC>& target){
        copy(source_device, target_device, (nn::parameters::Gradient::instance<SOURCE_SPEC>&) source, (nn::parameters::Gradient::instance<TARGET_SPEC>&) target);
        copy(source_device, target_device, source.gradient_first_order_moment , target.gradient_first_order_moment);
        copy(source_device, target_device, source.gradient_second_order_moment, target.gradient_second_order_moment);
    }
    template<typename DEVICE, typename SPEC_1, typename SPEC_2>
    typename SPEC_1::T abs_diff(DEVICE& device, const nn::parameters::Adam::instance<SPEC_1>& p1, const nn::parameters::Adam::instance<SPEC_2>& p2){
        typename SPEC_1::T acc = 0;
        acc += abs_diff(device, (nn::parameters::Gradient::instance<SPEC_1>&) p1, (nn::parameters::Gradient::instance<SPEC_2>&) p2);
        acc += abs_diff(device, p1.gradient_first_order_moment, p2.gradient_first_order_moment);
        acc += abs_diff(device, p1.gradient_second_order_moment, p2.gradient_second_order_moment);
        return acc;
    }
//    template<typename DEVICE, typename SPEC, typename OPTIMIZER>
//    void _reset_optimizer_state(DEVICE& device, nn::parameters::Adam::instance<SPEC>& p1, OPTIMIZER& optimizer) {
//        set_all(device, p1.gradient_first_order_moment, 0);
//        set_all(device, p1.gradient_second_order_moment, 0);
//    }
    template<typename DEVICE, typename PARAMETERS, typename MODEL>
    void reset_optimizer_state(DEVICE& device, nn::optimizers::Adam<PARAMETERS>& optimizer, MODEL& model) {
        optimizer.age = 1;
        _reset_optimizer_state(device, model, optimizer);
    }

    template<typename DEVICE, typename PARAMETERS, typename MODEL>
    void step(DEVICE& device, nn::optimizers::Adam<PARAMETERS>& optimizer, MODEL& model) {
        using T = typename PARAMETERS::T;
        optimizer.first_order_moment_bias_correction  = 1/(1 - math::pow(device.math, PARAMETERS::BETA_1, (T)optimizer.age));
        optimizer.second_order_moment_bias_correction = 1/(1 - math::pow(device.math, PARAMETERS::BETA_2, (T)optimizer.age));
        optimizer.age += 1;
        update(device, model, optimizer);
    }
    template<typename SOURCE_DEVICE, typename TARGET_DEVICE, typename SOURCE_SPEC, typename TARGET_SPEC>
    void copy(SOURCE_DEVICE& source_device, TARGET_DEVICE& target_device, const  nn::optimizers::Adam<SOURCE_SPEC>& source, nn::optimizers::Adam<TARGET_SPEC>& target){
        target.alpha = source.alpha;
        target.age = source.age;
    }
}
BACKPROP_TOOLS_NAMESPACE_WRAPPER_END
#endif
