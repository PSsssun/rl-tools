#include "../version.h"
#if (defined(BACKPROP_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(BACKPROP_TOOLS_DEVICES_CPU_BLAS_H)) && (BACKPROP_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define BACKPROP_TOOLS_DEVICES_CPU_BLAS_H

#include "../utils/generic/typing.h"
#include "devices.h"

#include "cpu.h"

BACKPROP_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools::devices{
    template <typename T_SPEC>
    struct CPU_BLAS: CPU<T_SPEC>{
        static constexpr DeviceId DEVICE_ID = DeviceId::CPU_BLAS;
    };
    using DefaultCPU_BLAS = CPU_BLAS<DefaultCPUSpecification>;
}
BACKPROP_TOOLS_NAMESPACE_WRAPPER_END

#endif
