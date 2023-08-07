#include "training.h"
#include <cassert>
struct AblationSpecBase: parameters::DefaultAblationSpec{
    static constexpr bool DISTURBANCE = true;
    static constexpr bool OBSERVATION_NOISE = true;
    static constexpr bool ASYMMETRIC_ACTOR_CRITIC = true;
    static constexpr bool ROTOR_DELAY = true;
    static constexpr bool ACTION_HISTORY = true;
    static constexpr bool ENABLE_CURRICULUM = true;
    static constexpr bool USE_INITIAL_REWARD_FUNCTION = true;
    static constexpr TI NUM_RUNS = 50;
};

template <bool T_DISTURBANCE, bool T_OBSERVATION_NOISE, bool T_ASYMMETRIC_ACTOR_CRITIC, bool T_ROTOR_DELAY, bool T_ACTION_HISTORY, bool T_ENABLE_CURRICULUM, bool T_USE_INITIAL_REWARD_FUNCTION>
struct AblationSpecTemplate: AblationSpecBase {
    static constexpr bool DISTURBANCE = T_DISTURBANCE;
    static constexpr bool OBSERVATION_NOISE = T_OBSERVATION_NOISE;
    static constexpr bool ASYMMETRIC_ACTOR_CRITIC = T_ASYMMETRIC_ACTOR_CRITIC;
    static constexpr bool ROTOR_DELAY = T_ROTOR_DELAY;
    static constexpr bool ACTION_HISTORY = T_ACTION_HISTORY;
    static constexpr bool ENABLE_CURRICULUM = T_ENABLE_CURRICULUM;
    static constexpr bool USE_INITIAL_REWARD_FUNCTION = T_USE_INITIAL_REWARD_FUNCTION;
    static_assert(!ACTION_HISTORY || ROTOR_DELAY); // action history implies rotor delay
    static_assert(!ENABLE_CURRICULUM || USE_INITIAL_REWARD_FUNCTION); // curriculum implies initial reward function
};

int main(int argc, char** argv){
    // assert exactly one command line argument and convert it to int
    TI job_array_id;
    assert(argc == 1 || argc == 2);
    if(argc == 2){
        job_array_id = std::stoi(argv[1]);
    }
    else{
        job_array_id = 0;
    }
    TI ablation_id = job_array_id / AblationSpecBase::NUM_RUNS;
    TI run_id = job_array_id % AblationSpecBase::NUM_RUNS;
    // DISTURBANCE OBSERVATION_NOISE ASYMMETRIC_ACTOR_CRITIC ROTOR_DELAY ACTION_HISTORY ENABLE_CURRICULUM USE_INITIAL_REWARD_FUNCTION

    using ABLATION_SPEC_0 = AblationSpecTemplate< true,  true,  true,  true,  true,  true,  true>;
    using ABLATION_SPEC_1 = AblationSpecTemplate< true,  true,  true,  true,  true, false,  true>;
    using ABLATION_SPEC_2 = AblationSpecTemplate< true,  true,  true,  true,  true, false, false>;
    using ABLATION_SPEC_3 = AblationSpecTemplate< true,  true,  true,  true, false,  true,  true>;
    using ABLATION_SPEC_4 = AblationSpecTemplate< true,  true,  true, false, false,  true,  true>;
    using ABLATION_SPEC_5 = AblationSpecTemplate< true,  true, false,  true,  true,  true,  true>;
    using ABLATION_SPEC_6 = AblationSpecTemplate< true, false,  true,  true,  true,  true,  true>;
    using ABLATION_SPEC_7 = AblationSpecTemplate<false,  true,  true,  true,  true,  true,  true>;

    switch(ablation_id){
        case 0:
            train<ABLATION_SPEC_0>(run_id);
            break;
        case 1:
            train<ABLATION_SPEC_1>(run_id);
            break;
        case 2:
            train<ABLATION_SPEC_2>(run_id);
            break;
        case 3:
            train<ABLATION_SPEC_3>(run_id);
            break;
        case 4:
            train<ABLATION_SPEC_4>(run_id);
            break;
        case 5:
            train<ABLATION_SPEC_5>(run_id);
            break;
        case 6:
            train<ABLATION_SPEC_6>(run_id);
            break;
        case 7:
            train<ABLATION_SPEC_7>(run_id);
            break;
        default:
            std::cout << "Invalid ablation id: " << ablation_id << std::endl;
            return 1;
    }

    return 0;
}