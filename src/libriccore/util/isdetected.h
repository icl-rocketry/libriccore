/**
 * @file isdetected.h
 * @author Kiran de Silva (kd619@ic.ac.uk)
 * @brief Taken directly from c++17 TSv2 experimental impelmentation as unsure if 
 * espidf will have the experimental headers
 * @version 0.1
 * @date 2023-04-06
 */
#pragma once
#include <type_traits>
namespace RicCoreUtil{
namespace detail {
template <class Default, class AlwaysVoid,
          template<class...> class Op, class... Args>
struct detector {
  using value_t = std::false_type;
  using type = Default;
};
 
template <class Default, template<class...> class Op, class... Args>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
  using value_t = std::true_type;
  using type = Op<Args...>;
};
 
} // namespace detail
 
struct nonesuch{};

template <template<class...> class Op, class... Args>
using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;
};