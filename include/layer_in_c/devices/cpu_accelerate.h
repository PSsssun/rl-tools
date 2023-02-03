#ifndef LAYER_IN_C_DEVICES_CPU_ACCELERATE_H
#define LAYER_IN_C_DEVICES_CPU_ACCELERATE_H

#include <layer_in_c/utils/generic/typing.h>
#include "devices.h"

#include "cpu_blas.h"

namespace layer_in_c::devices{
    template <typename T_SPEC>
    struct CPU_ACCELERATE: CPU_BLAS<T_SPEC>{
        explicit CPU_ACCELERATE(typename T_SPEC::LOGGING& logger) : CPU_BLAS<T_SPEC>(logger) {}
    };
    using DefaultCPU_ACCELERATE = CPU_ACCELERATE<DefaultCPUSpecification>;
}

#endif