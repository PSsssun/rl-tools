#ifndef NEURAL_NETWORK_MODELS_H
#define NEURAL_NETWORK_MODELS_H
#include <layer_in_c/nn/nn.h>
namespace layer_in_c::nn_models::three_layer_fc {
    template <typename T_T, int INPUT_DIM, int LAYER_1_DIM, nn::activation_functions::ActivationFunction LAYER_1_FN, int LAYER_2_DIM, nn::activation_functions::ActivationFunction LAYER_2_FN, int OUTPUT_DIM, nn::activation_functions::ActivationFunction OUTPUT_LAYER_FN>
    struct StructureSpecification{
        typedef T_T T;
        typedef nn::layers::dense::LayerSpec<T,   INPUT_DIM, LAYER_1_DIM,      LAYER_1_FN> LAYER_1;
        typedef nn::layers::dense::LayerSpec<T, LAYER_1_DIM, LAYER_2_DIM,      LAYER_2_FN> LAYER_2;
        typedef nn::layers::dense::LayerSpec<T, LAYER_2_DIM,  OUTPUT_DIM, OUTPUT_LAYER_FN> OUTPUT_LAYER;
    };
    template <typename STRUCTURE_SPEC>
    struct InferenceSpecification{
        typedef typename STRUCTURE_SPEC::T T;
        typedef nn::layers::dense::Layer<typename STRUCTURE_SPEC::LAYER_1> LAYER_1;
        typedef nn::layers::dense::Layer<typename STRUCTURE_SPEC::LAYER_2> LAYER_2;
        typedef nn::layers::dense::Layer<typename STRUCTURE_SPEC::OUTPUT_LAYER> OUTPUT_LAYER;
    };

    template <typename STRUCTURE_SPEC>
    struct InferenceBackwardSpecification{
        typedef typename STRUCTURE_SPEC::T T;
        typedef nn::layers::dense::LayerBackward<typename STRUCTURE_SPEC::LAYER_1> LAYER_1;
        typedef nn::layers::dense::LayerBackward<typename STRUCTURE_SPEC::LAYER_2> LAYER_2;
        typedef nn::layers::dense::LayerBackward<typename STRUCTURE_SPEC::OUTPUT_LAYER> OUTPUT_LAYER;
    };

    template<typename STRUCTURE_SPEC, typename T_SGD_PARAMETERS>
    struct SGDSpecification{
        typedef typename STRUCTURE_SPEC::T T;
        typedef T_SGD_PARAMETERS SGD_PARAMETERS;
        typedef nn::layers::dense::LayerBackwardSGD<typename STRUCTURE_SPEC::LAYER_1, SGD_PARAMETERS> LAYER_1;
        typedef nn::layers::dense::LayerBackwardSGD<typename STRUCTURE_SPEC::LAYER_2, SGD_PARAMETERS> LAYER_2;
        typedef nn::layers::dense::LayerBackwardSGD<typename STRUCTURE_SPEC::OUTPUT_LAYER, SGD_PARAMETERS> OUTPUT_LAYER;
    };

    template<typename STRUCTURE_SPEC, typename T_ADAM_PARAMETERS>
    struct AdamSpecification{
        typedef typename STRUCTURE_SPEC::T T;
        typedef T_ADAM_PARAMETERS ADAM_PARAMETERS;
        typedef nn::layers::dense::LayerBackwardAdam<typename STRUCTURE_SPEC::LAYER_1, ADAM_PARAMETERS> LAYER_1;
        typedef nn::layers::dense::LayerBackwardAdam<typename STRUCTURE_SPEC::LAYER_2, ADAM_PARAMETERS> LAYER_2;
        typedef nn::layers::dense::LayerBackwardAdam<typename STRUCTURE_SPEC::OUTPUT_LAYER, ADAM_PARAMETERS> OUTPUT_LAYER;
    };

    template<typename T_SPEC>
    struct NeuralNetwork{
        static constexpr int INPUT_DIM = T_SPEC::LAYER_1::SPEC::INPUT_DIM;
        static constexpr int OUTPUT_DIM = T_SPEC::OUTPUT_LAYER::SPEC::OUTPUT_DIM;
        typedef T_SPEC SPEC;
        typename SPEC::LAYER_1 layer_1;
        typename SPEC::LAYER_2 layer_2;
        typename SPEC::OUTPUT_LAYER output_layer;
    };

    template<typename SPEC>
    struct NeuralNetworkBackward: public NeuralNetwork<SPEC>{
    };

    template<typename SPEC>
    struct NeuralNetworkSGD: public NeuralNetworkBackward<SPEC>{
    };

    template<typename SPEC>
    struct NeuralNetworkAdam: public NeuralNetworkBackward<SPEC>{
        uint32_t age = 1;
    };


}

#endif