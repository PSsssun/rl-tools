#include <gtest/gtest.h>
#include <highfive/H5File.hpp>

#include "layer_in_c/nn_models/models.h"
#include "layer_in_c/nn_models/persist.h"
#include <layer_in_c/nn/nn.h>
#include <layer_in_c/nn_models/operations_generic.h>
#include <layer_in_c/utils/rng_std.h>

#include "../utils/utils.h"

#include "default_network.h"
#include "../utils/nn_comparison.h"

#include <layer_in_c/utils/persist.h>
TEST(NeuralNetworkPersist, Saving) {

    NetworkType network_1, network_2;
    std::mt19937 rng(2);
    lic::init_weights<NETWORK_SPEC, lic::utils::random::stdlib::uniform<DTYPE, typeof(rng)>, typeof(rng)>(network_1, rng);
    lic::init_weights<NETWORK_SPEC, lic::utils::random::stdlib::uniform<DTYPE, typeof(rng)>, typeof(rng)>(network_2, rng);
    {
        auto output_file = HighFive::File(std::string("test.hdf5"), HighFive::File::Overwrite);
        lic::save(network_1, output_file.createGroup("three_layer_fc"));
    }

    DTYPE diff_pre_load = abs_diff(network_1, network_2);
    ASSERT_GT(diff_pre_load, 10);
    std::cout << "diff_pre_load: " << diff_pre_load << std::endl;
    {
        auto input_file = HighFive::File(std::string("test.hdf5"), HighFive::File::ReadOnly);
        lic::load(network_2, input_file.getGroup("three_layer_fc"));
    }
    DTYPE diff_post_load = abs_diff(network_1, network_2);
    ASSERT_EQ(diff_post_load, 0);
    std::cout << "diff_post_load: " << diff_post_load << std::endl;
}
