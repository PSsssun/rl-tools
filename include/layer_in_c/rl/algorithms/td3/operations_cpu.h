#ifndef LAYER_IN_C_RL_ALGORITHMS_TD3_OPERATIONS_CPU_H
#define LAYER_IN_C_RL_ALGORITHMS_TD3_OPERATIONS_CPU_H

#include <layer_in_c/devices.h>
#include <layer_in_c/utils/generic/math.h>
#include "td3.h"

#include "operations_generic.h"

#include <layer_in_c/rl/components/operations_cpu.h>
#include <layer_in_c/nn_models/operations_cpu.h>

#include <random>
#include <cstring>

namespace layer_in_c{
    template <typename DEVICE, typename SPEC, typename RNG>
    void init(rl::algorithms::td3::ActorCritic<DEVICE, SPEC>& actor_critic, RNG& rng){
        layer_in_c::init_weights(actor_critic.actor   , rng);
        layer_in_c::init_weights(actor_critic.critic_1, rng);
        layer_in_c::init_weights(actor_critic.critic_2, rng);
        layer_in_c::reset_optimizer_state(actor_critic.actor);
        layer_in_c::reset_optimizer_state(actor_critic.critic_1);
        layer_in_c::reset_optimizer_state(actor_critic.critic_2);

        actor_critic.actor_target = actor_critic.actor;
        actor_critic.critic_target_1 = actor_critic.critic_1;
        actor_critic.critic_target_2 = actor_critic.critic_2;
    }
    template <typename SPEC, typename CRITIC_TYPE, typename REPLAY_BUFFER_DEVICE, size_t REPLAY_BUFFER_CAPACITY, typename RNG, bool DETERMINISTIC=false>
    typename SPEC::T train_critic(
            rl::algorithms::td3::ActorCritic<devices::CPU, SPEC>& actor_critic,
            CRITIC_TYPE& critic,
            rl::components::ReplayBuffer<
                    REPLAY_BUFFER_DEVICE,
                    rl::components::replay_buffer::Spec<
                            typename SPEC::T,
                            SPEC::ENVIRONMENT::OBSERVATION_DIM,
                            SPEC::ENVIRONMENT::ACTION_DIM,
                            REPLAY_BUFFER_CAPACITY
                    >
            >& replay_buffer,
            typename SPEC::T target_next_action_noise[SPEC::PARAMETERS::CRITIC_BATCH_SIZE][SPEC::ENVIRONMENT::ACTION_DIM],
            RNG& rng
    ) {
        typedef typename SPEC::T T;
        assert(replay_buffer.full || replay_buffer.position >= SPEC::PARAMETERS::CRITIC_BATCH_SIZE);
        T loss = 0;
        zero_gradient(critic);
        std::uniform_int_distribution<size_t> sample_distribution(0, (replay_buffer.full ? REPLAY_BUFFER_CAPACITY : replay_buffer.position) - 1);
        for (int batch_step_i=0; batch_step_i < SPEC::PARAMETERS::CRITIC_BATCH_SIZE; batch_step_i++){
            size_t sample_index = DETERMINISTIC ? batch_step_i : sample_distribution(rng);
            T next_state_action_value_input[SPEC::ENVIRONMENT::OBSERVATION_DIM + SPEC::ENVIRONMENT::ACTION_DIM];
            std::memcpy(next_state_action_value_input, replay_buffer.next_observations[sample_index], sizeof(T) * SPEC::ENVIRONMENT::OBSERVATION_DIM); // setting the first part with next observations
            evaluate(actor_critic.actor_target, next_state_action_value_input, &next_state_action_value_input[SPEC::ENVIRONMENT::OBSERVATION_DIM]); // setting the second part with next actions
            for(size_t action_i=0; action_i < SPEC::ENVIRONMENT::ACTION_DIM; action_i++){
                T noisy_next_action = next_state_action_value_input[SPEC::ENVIRONMENT::OBSERVATION_DIM + action_i] + target_next_action_noise[batch_step_i][action_i];
                noisy_next_action = utils::math::clamp<T>(noisy_next_action, -1, 1);
                next_state_action_value_input[SPEC::ENVIRONMENT::OBSERVATION_DIM + action_i] = noisy_next_action;
            }
            T next_state_action_value_critic_1 = evaluate(actor_critic.critic_target_1, next_state_action_value_input);
            T next_state_action_value_critic_2 = evaluate(actor_critic.critic_target_2, next_state_action_value_input);

            T min_next_state_action_value = std::min(
                    next_state_action_value_critic_1,
                    next_state_action_value_critic_2
            );
            T state_action_value_input[SPEC::ENVIRONMENT::OBSERVATION_DIM + SPEC::ENVIRONMENT::ACTION_DIM];
            memcpy(state_action_value_input, replay_buffer.observations[sample_index], sizeof(T) * SPEC::ENVIRONMENT::OBSERVATION_DIM); // setting the first part with the current observation
            memcpy(&state_action_value_input[SPEC::ENVIRONMENT::OBSERVATION_DIM], replay_buffer.actions[sample_index], sizeof(T) * SPEC::ENVIRONMENT::ACTION_DIM); // setting the first part with the current action
            T target_action_value[1] = {replay_buffer.rewards[sample_index] + SPEC::PARAMETERS::GAMMA * min_next_state_action_value * (!replay_buffer.terminated[sample_index])};

            forward_backward_mse<typename CRITIC_TYPE::DEVICE, typename CRITIC_TYPE::SPEC, SPEC::PARAMETERS::CRITIC_BATCH_SIZE>(critic, state_action_value_input, target_action_value);
            static_assert(CRITIC_TYPE::SPEC::OUTPUT_LAYER::SPEC::ACTIVATION_FUNCTION == nn::activation_functions::IDENTITY); // Ensuring the critic output activation is identity so that we can just use the pre_activations to get the loss value
            T loss_sample = nn::loss_functions::mse<T, 1, SPEC::PARAMETERS::CRITIC_BATCH_SIZE>(critic.output_layer.pre_activations, target_action_value);
            loss += loss_sample;
        }
        update(critic);
        return loss;
    }

    template <typename SPEC, typename REPLAY_BUFFER_DEVICE, size_t REPLAY_BUFFER_CAPACITY, typename RNG, bool DETERMINISTIC = false>
    typename SPEC::T train_actor(
            rl::algorithms::td3::ActorCritic<devices::CPU, SPEC>& actor_critic,
            rl::components::ReplayBuffer<
                    REPLAY_BUFFER_DEVICE,
                    rl::components::replay_buffer::Spec<
                            typename SPEC::T,
                            SPEC::ENVIRONMENT::OBSERVATION_DIM,
                            SPEC::ENVIRONMENT::ACTION_DIM,
                            REPLAY_BUFFER_CAPACITY
                    >
            >& replay_buffer,
            RNG& rng
    ) {
        typedef typename SPEC::T T;
        typedef typename SPEC::PARAMETERS PARAMETERS;
        typedef typename SPEC::ENVIRONMENT ENVIRONMENT;
        T actor_value = 0;
        zero_gradient(actor_critic.actor);
        std::uniform_int_distribution<size_t> sample_distribution(0, (replay_buffer.full ? REPLAY_BUFFER_CAPACITY : replay_buffer.position) - 1);
        for (size_t sample_i=0; sample_i < PARAMETERS::ACTOR_BATCH_SIZE; sample_i++){
            size_t sample_index = DETERMINISTIC ? sample_i : sample_distribution(rng);
            T state_action_value_input[ENVIRONMENT::OBSERVATION_DIM + ENVIRONMENT::ACTION_DIM];
            memcpy(state_action_value_input, replay_buffer.observations[sample_index], sizeof(T) * ENVIRONMENT::OBSERVATION_DIM); // setting the first part with next observations
            forward(actor_critic.actor, state_action_value_input, &state_action_value_input[ENVIRONMENT::OBSERVATION_DIM]);

            auto& critic = actor_critic.critic_1;
            T critic_output = forward_univariate(critic, state_action_value_input);
            actor_value += critic_output/SPEC::PARAMETERS::ACTOR_BATCH_SIZE;
            T d_output[1] = {-(T)1/SPEC::PARAMETERS::ACTOR_BATCH_SIZE}; // we want to maximise the critic output using gradient descent
            T d_critic_input[ENVIRONMENT::OBSERVATION_DIM + ENVIRONMENT::ACTION_DIM];
            backward(critic, state_action_value_input, d_output, d_critic_input);
            T d_actor_input[ENVIRONMENT::OBSERVATION_DIM];
            backward(actor_critic.actor, state_action_value_input, &d_critic_input[ENVIRONMENT::OBSERVATION_DIM], d_actor_input);
        }
        update(actor_critic.actor);
        return actor_value;
    }
    template <typename SPEC, typename CRITIC_TYPE, typename REPLAY_BUFFER_DEVICE, size_t REPLAY_BUFFER_CAPACITY, typename RNG>
    typename SPEC::T train_critic(
        rl::algorithms::td3::ActorCritic<devices::CPU, SPEC>& actor_critic,
        CRITIC_TYPE& critic,
        rl::components::ReplayBuffer<
            REPLAY_BUFFER_DEVICE,
            rl::components::replay_buffer::Spec<
                typename SPEC::T,
                SPEC::ENVIRONMENT::OBSERVATION_DIM,
                SPEC::ENVIRONMENT::ACTION_DIM,
                REPLAY_BUFFER_CAPACITY
            >
        >& replay_buffer,
        RNG& rng
    ) {
        typedef typename SPEC::T T;
        std::normal_distribution<T> target_next_action_noise_distribution(0, SPEC::PARAMETERS::TARGET_NEXT_ACTION_NOISE_STD);
        T action_noise[SPEC::PARAMETERS::CRITIC_BATCH_SIZE][SPEC::ENVIRONMENT::ACTION_DIM];
        for(int batch_sample_i=0; batch_sample_i < SPEC::PARAMETERS::CRITIC_BATCH_SIZE; batch_sample_i++){
            for(int action_i=0; action_i < SPEC::ENVIRONMENT::ACTION_DIM; action_i++){
                action_noise[batch_sample_i][action_i] = utils::math::clamp(
                        target_next_action_noise_distribution(rng),
                        -SPEC::PARAMETERS::TARGET_NEXT_ACTION_NOISE_CLIP,
                        SPEC::PARAMETERS::TARGET_NEXT_ACTION_NOISE_CLIP
                );
            }
        }
        return train_critic(actor_critic, critic, replay_buffer, action_noise, rng);
    }
}


#endif