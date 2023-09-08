#include "../../version.h"
#if (defined(BACKPROP_TOOLS_DISABLE_INCLUDEGUARDS) || !defined(BACKPROP_TOOLS_NN_PARAMETERS_PERSIST_H)) && (BACKPROP_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define BACKPROP_TOOLS_NN_PARAMETERS_PERSIST_H

#include "../../nn/parameters/parameters.h"

#include <highfive/H5Group.hpp>
BACKPROP_TOOLS_NAMESPACE_WRAPPER_START
namespace backprop_tools{
    template<typename DEVICE, typename CONTAINER>
    void save(DEVICE& device, nn::parameters::Plain::instance<CONTAINER>& parameter, HighFive::Group group) {
        save(device, parameter.parameters, group, "parameters");
    }
    template<typename DEVICE, typename CONTAINER>
    void save(DEVICE& device, nn::parameters::Gradient::instance<CONTAINER>& parameter, HighFive::Group group) {
        save(device, (nn::parameters::Plain::instance<CONTAINER>&)parameter, group);
        save(device, parameter.gradient, group, "gradient");
    }
    template<typename DEVICE, typename CONTAINER>
    void load(DEVICE& device, nn::parameters::Plain::instance<CONTAINER>& parameter, HighFive::Group group) {
        load(device, parameter.parameters, group, "parameters");
    }
    template<typename DEVICE, typename CONTAINER>
    void load(DEVICE& device, nn::parameters::Gradient::instance<CONTAINER>& parameter, HighFive::Group group) {
        load(device, (nn::parameters::Plain::instance<CONTAINER>&)parameter, group);
        load(device, parameter.gradient, group, "gradient");
    }
}
BACKPROP_TOOLS_NAMESPACE_WRAPPER_END
#endif
