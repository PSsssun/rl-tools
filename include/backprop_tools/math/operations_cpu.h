#include "../version.h"
#if (defined(BACKPROP_TOOLS_DISABLE_INCLUDEGUARDS) || !defined(BACKPROP_TOOLS_MATH_OPERATIONS_CPU_H)) && (BACKPROP_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define BACKPROP_TOOLS_MATH_OPERATIONS_CPU_H

#include "operations_generic.h"

#include "../devices/cpu.h"

#include <cmath>
#include <algorithm> // required for clamp(...)

BACKPROP_TOOLS_NAMESPACE_WRAPPER_START
namespace backprop_tools::math {

    template<typename T>
    T sqrt(const devices::math::CPU&, const T x) {
        return std::sqrt(x);
    }
    template<typename T>
    T tanh(const devices::math::CPU&, const T x) {
        return std::tanh(x);
    }
    template<typename T>
    T exp(const devices::math::CPU&, const T x) {
        return std::exp(x);
    }
    template<typename T>
    T sin(const devices::math::CPU&, const T x) {
        return std::sin(x);
    }
    template<typename T>
    T cos(const devices::math::CPU&, const T x) {
        return std::cos(x);
    }
    template<typename T>
    T acos(const devices::math::CPU&, const T x) {
        return std::acos(x);
    }
    template<typename TX, typename TY>
    auto pow(const devices::math::CPU&, const TX x, const TY y) {
        return std::pow(x, y);
    }
    template<typename T>
    auto log(const devices::math::CPU&, const T x) {
        return std::log(x);
    }
    template<typename T>
    auto floor(const devices::math::CPU&, const T x) {
        return std::floor(x);
    }
    template<typename T>
    auto is_nan(const devices::math::CPU&, const T x) {
        return std::isnan(x);
    }
    template<typename T>
    auto is_finite(const devices::math::CPU&, const T x) {
        return std::isfinite(x);
    }
    template<typename T>
    T clamp(const devices::math::CPU&, T x, T min, T max){
        return std::clamp(x, min, max);
    }
    template<typename T>
    T min(const devices::math::CPU&, T x, T y){
        return std::min(x, y);
    }
    template<typename T>
    T max(const devices::math::CPU&, T x, T y){
        return std::max(x, y);
    }
    template<typename T>
    T abs(const devices::math::CPU&, T x){
        return std::abs(x);
    }
    template<typename T>
    T nan(const devices::math::CPU&){
        return std::numeric_limits<T>::quiet_NaN();
    }
    template<typename T>
    T fast_tanh(const devices::math::CPU& dev, T x) {
        x = clamp(dev, x, -(T)3.0, (T)3.0);
        T x_squared = x * x;
        return x * (27 + x_squared) / (27 + 9 * x_squared);
    }
    template<typename T>
    T atan2(const devices::math::CPU& dev, T a, T b) {
        return std::atan2(a, b);
    }
}
BACKPROP_TOOLS_NAMESPACE_WRAPPER_END
#endif
