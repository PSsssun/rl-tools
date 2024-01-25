////#define RL_TOOLS_DISABLE_EVALUATION
//#include "training.h"
//int main(int argc, char** argv){
//    TI seed = 0;
//    if(argc > 1){
//        seed = std::atoi(argv[1]);
//    }
//    auto return_code = run(seed);
//    return return_code < 4 ? 0 : return_code;
//}

#include <rl_tools/operations/cpu_mux.h>
#include <rl_tools/nn/operations_cpu_mux.h>
#include <rl_tools/rl/environments/pendulum/operations_cpu.h>
#include <rl_tools/nn_models/sequential/operations_generic.h>
#include <rl_tools/nn_models/mlp/operations_generic.h>


#include <rl_tools/rl/algorithms/td3/loop/core/config.h>
#include <rl_tools/rl/loop/steps/evaluation/config.h>
#include <rl_tools/rl/loop/steps/timing/config.h>
#include <rl_tools/rl/algorithms/td3/loop/core/operations.h>
#include <rl_tools/rl/loop/steps/evaluation/operations.h>
#include <rl_tools/rl/loop/steps/timing/operations.h>

namespace rlt = rl_tools;

using DEVICE = rlt::devices::DEVICE_FACTORY<>;
using T = float;
using TI = typename DEVICE::index_t;

using PENDULUM_SPEC = rlt::rl::environments::pendulum::Specification<T, TI, rlt::rl::environments::pendulum::DefaultParameters<T>>;
using ENVIRONMENT = rlt::rl::environments::Pendulum<PENDULUM_SPEC>;
struct LOOP_CORE_PARAMETERS: rlt::rl::algorithms::td3::loop::core::DefaultParameters<T, TI, ENVIRONMENT>{
    static constexpr TI STEP_LIMIT = 10000;
    static constexpr TI ACTOR_NUM_LAYERS = 3;
    static constexpr TI ACTOR_HIDDEN_DIM = 64;
    static constexpr TI CRITIC_NUM_LAYERS = 3;
    static constexpr TI CRITIC_HIDDEN_DIM = 64;
};
#ifdef BENCHMARK
using LOOP_CORE_CONFIG = rlt::rl::algorithms::sac::loop::core::DefaultConfig<DEVICE, T, ENVIRONMENT, LOOP_CORE_PARAMETERS>;
using LOOP_TIMING_CONFIG = rlt::rl::loop::steps::timing::DefaultConfig<LOOP_CORE_CONFIG>;
using LOOP_CONFIG = LOOP_TIMING_CONFIG;
#else
using LOOP_CORE_CONFIG = rlt::rl::algorithms::td3::loop::core::DefaultConfig<DEVICE, T, ENVIRONMENT, LOOP_CORE_PARAMETERS, rlt::rl::algorithms::td3::loop::core::DefaultConfigApproximatorsMLP>;
using LOOP_EVAL_CONFIG = rlt::rl::loop::steps::evaluation::DefaultConfig<LOOP_CORE_CONFIG>;
using LOOP_TIMING_CONFIG = rlt::rl::loop::steps::timing::DefaultConfig<LOOP_EVAL_CONFIG>;
using LOOP_CONFIG = LOOP_TIMING_CONFIG;
#endif

using LOOP_STATE = LOOP_CONFIG::State<LOOP_CONFIG>;

int main(){
    LOOP_STATE ts;
    rlt::init(ts, 0);
    while(!rlt::step(ts)){
#ifndef BENCHMARK
        if(ts.step == 5000){
            std::cout << "steppin yourself > callbacks 'n' hooks: " << ts.step << std::endl;
        }
#endif
    }
    rlt::destroy(ts);
}
