#pragma once

#include <tuple>
#include <type_traits>

namespace RicCoreUtil{
    /**
     * @brief SFINAE to check if type is tuple 
     * 
     * @tparam typename 
     */
    template <typename> struct is_tuple: std::false_type {};
    template <typename ...T> struct is_tuple<std::tuple<T...>>: std::true_type {};
    template <typename ...T> struct is_tuple<const std::tuple<T...>>: std::true_type {};
};