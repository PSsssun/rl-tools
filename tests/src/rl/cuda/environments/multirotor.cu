#include <backprop_tools/operations/cuda/group_1.h>
#include <backprop_tools/operations/cpu/group_1.h>
#include <backprop_tools/operations/cuda/group_2.h>
#include <backprop_tools/operations/cpu/group_2.h>
#include <backprop_tools/operations/cuda/group_3.h>
#include <backprop_tools/operations/cpu/group_3.h>

#include "../td3_full_training_parameters_multirotor.h"

#include <backprop_tools/rl/environments/multirotor/operations_cpu.h>

#include <backprop_tools/rl/components/off_policy_runner/operations_cuda.h>
#include <backprop_tools/rl/components/off_policy_runner/operations_cpu.h>


#include <gtest/gtest.h>

namespace bpt = backprop_tools;

using DTYPE = double;
constexpr DTYPE DIFF_THRESHOLD = bpt::utils::typing::is_same_v<DTYPE, float> ? 1e-6 : 1e-10;
using DEVICE_CPU = bpt::devices::DefaultCPU;
using DEVICE_CUDA = bpt::devices::DefaultCUDA;
using p = parameters_multirotor_0<DEVICE_CUDA, DTYPE>;
using rlp = p::rl<p::env::ENVIRONMENT>;

using TI = typename DEVICE_CUDA::index_t;

static constexpr TI N_ENVIRONMENTS = 1;
static constexpr TI REPLAY_BUFFER_CAP = 500000;
static constexpr TI ENVIRONMENT_STEP_LIMIT = 200;
struct OFF_POLICY_RUNNER_PARAMETERS: bpt::rl::components::off_policy_runner::DefaultParameters<DTYPE>{
    static constexpr DTYPE EXPLORATION_NOISE = 0;
};
using OFF_POLICY_RUNNER_SPEC = bpt::rl::components::off_policy_runner::Specification<DTYPE, TI, p::env::ENVIRONMENT, N_ENVIRONMENTS, REPLAY_BUFFER_CAP, ENVIRONMENT_STEP_LIMIT, OFF_POLICY_RUNNER_PARAMETERS>;
using OFF_POLICY_RUNNER_TYPE = bpt::rl::components::OffPolicyRunner<OFF_POLICY_RUNNER_SPEC>;
using CRITIC_BATCH_TYPE = bpt::rl::components::off_policy_runner::Batch<bpt::rl::components::off_policy_runner::BatchSpecification<OFF_POLICY_RUNNER_SPEC, rlp::ACTOR_CRITIC_TYPE::SPEC::PARAMETERS::CRITIC_BATCH_SIZE>>;
using ACTOR_BATCH_TYPE = bpt::rl::components::off_policy_runner::Batch<bpt::rl::components::off_policy_runner::BatchSpecification<OFF_POLICY_RUNNER_SPEC, rlp::ACTOR_CRITIC_TYPE::SPEC::PARAMETERS::ACTOR_BATCH_SIZE>>;
using CRITIC_TRAINING_BUFFERS_TYPE = bpt::rl::algorithms::td3::CriticTrainingBuffers<typename rlp::ACTOR_CRITIC_TYPE::SPEC>;
using ACTOR_TRAINING_BUFFERS_TYPE = bpt::rl::algorithms::td3::ActorTrainingBuffers<typename rlp::ACTOR_CRITIC_TYPE::SPEC>;

TEST(BACKPROP_TOOLS_RL_CUDA_ENVIRONMENTS_MULTIROTOR, TEST){
    DEVICE_CPU cpu;
    DEVICE_CUDA cuda;
    p::env::ENVIRONMENT envs[rlp::N_ENVIRONMENTS];
    for(TI env_i=0; env_i < rlp::N_ENVIRONMENTS; env_i++){
        auto parameters = p::env::parameters;
        parameters.mdp.init = bpt::rl::environments::multirotor::parameters::init::simple<DTYPE, typename DEVICE_CUDA::index_t, 4, p::env::REWARD_FUNCTION>;
        envs[0].parameters = parameters;
    }
    OFF_POLICY_RUNNER_TYPE off_policy_runner_cpu, off_policy_runner_feedback, off_policy_runner_cuda;
    OFF_POLICY_RUNNER_TYPE* off_policy_runner_cuda_struct;
    auto rng_cpu = bpt::random::default_engine(DEVICE_CPU::SPEC::RANDOM());
    auto rng_cuda = bpt::random::default_engine(DEVICE_CUDA::SPEC::RANDOM());

    bpt::init(cuda);
    bpt::malloc(cpu, off_policy_runner_cpu);
    bpt::malloc(cpu, off_policy_runner_feedback);
    bpt::malloc(cuda, off_policy_runner_cuda);
    cudaMalloc(&off_policy_runner_cuda_struct, sizeof(rlp::OFF_POLICY_RUNNER_TYPE));
    bpt::check_status(cuda);

    bpt::init(cpu, off_policy_runner_cpu, envs);
    bpt::copy(cuda, cpu, off_policy_runner_cuda, off_policy_runner_cpu);
    cudaMemcpy(off_policy_runner_cuda_struct, &off_policy_runner_cuda, sizeof(rlp::OFF_POLICY_RUNNER_TYPE), cudaMemcpyHostToDevice);
    bpt::check_status(cuda);

    bpt::randn(cpu, off_policy_runner_cpu.buffers.actions, rng_cpu);
    for(TI env_i=0; env_i < rlp::N_ENVIRONMENTS; env_i++) {
        bpt::randn(cpu, off_policy_runner_cpu.replay_buffers[env_i].data, rng_cpu);
    }
    bpt::copy(cuda, cpu, off_policy_runner_cuda, off_policy_runner_cpu);

    for(TI step_i = 0; step_i < 10; step_i++){
        bpt::rl::components::off_policy_runner::prologue(cpu, off_policy_runner_cpu, rng_cpu);
        bpt::rl::components::off_policy_runner::epilogue(cpu, off_policy_runner_cpu, rng_cpu);

        bpt::rl::components::off_policy_runner::prologue(cuda, off_policy_runner_cuda_struct, rng_cuda);
        bpt::rl::components::off_policy_runner::epilogue(cuda, off_policy_runner_cuda_struct, rng_cuda);
    }


    bpt::copy(cpu, cuda, off_policy_runner_feedback, off_policy_runner_cuda);

    std::cout << "next observations cpu: " << std::endl;
    bpt::print(cpu, off_policy_runner_cpu.buffers.next_observations);
    std::cout << "next observations cuda: " << std::endl;
    bpt::print(cpu, off_policy_runner_feedback.buffers.next_observations);

    auto abs_diff_observations = bpt::abs_diff(cpu, off_policy_runner_cpu.buffers.observations, off_policy_runner_feedback.buffers.observations);
    auto abs_diff_actions = bpt::abs_diff(cpu, off_policy_runner_cpu.buffers.actions, off_policy_runner_feedback.buffers.actions);
    auto abs_diff_next_observations = bpt::abs_diff(cpu, off_policy_runner_cpu.buffers.next_observations, off_policy_runner_feedback.buffers.next_observations);
    auto abs_diff_replay_buffer = bpt::abs_diff(cpu, off_policy_runner_cpu.replay_buffers[0], off_policy_runner_feedback.replay_buffers[0]);

    ASSERT_LT(abs_diff_observations, DIFF_THRESHOLD);
    ASSERT_LT(abs_diff_actions, DIFF_THRESHOLD);
    ASSERT_LT(abs_diff_next_observations, DIFF_THRESHOLD);
    ASSERT_LT(abs_diff_replay_buffer, DIFF_THRESHOLD);
}