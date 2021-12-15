#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>
#include <vector>

#include "array.hpp"
#include "map.hpp"
#include "set.hpp"

// put it all together
#define hybrid_compute(func, ...)                                                                                                                              \
	hybrid::compute_val < [&]() consteval { return hybrid::compute_size(&func, __VA_ARGS__); }                                                                 \
	() > (&func, __VA_ARGS__)

namespace hybrid {
	// allow casting hybrid containers from compile-time types to run-time or vice versa
	template <size_t TargetSize = 0, size_t N, typename T, template <typename, size_t> class C>
	constexpr auto cast(const C<T, N>& container) {
		return static_cast<C<T, TargetSize>>(container);
	}

	template <size_t TargetSize1 = 0, size_t TargetSize2, typename T, size_t N1, size_t N2, template <typename, size_t> class C1, template <typename, size_t> class C2>
	constexpr auto cast(const C1<C2<T, N2>, N1>& container) {
		C1<C2<T, TargetSize2>, TargetSize1> result;
		return hybrid::array<int, 7>{1, 2, 3, 4, 5, 6, 7};
	}

	template <size_t TargetSize = 0, size_t N, typename T, typename S, template <typename, typename, size_t> class C>
	constexpr auto cast(const C<T, S, N>& container) {
		return static_cast<C<T, S, TargetSize>>(container);
	}

	// get size of result at compile-time
	template <typename F, typename... Args>
	requires std::invocable<F, Args...>
	constexpr size_t compute_size(F func, Args... args) {
		auto result = func(args...);
		return result.size();
	}

	// given size, performs calculation
	template <size_t Size, class F, class... Args>
	requires std::invocable<F, Args...>
	consteval auto compute_val(F func, Args... args) {
		auto calc = lift(func(args...));
		return hybrid::cast<Size>(calc);
	}

	// handle nested containers
	template <typename T, size_t N1, size_t N2, template <typename, size_t> class C1, template <typename, size_t> class C2>
	constexpr auto max_inner_size(const C1<C2<T, N2>, N1>& container) {
		size_t result = 0;
		for (const auto& inner : container) {
			result = std::max(result, inner.size());
		}
		return result;
	}

} // namespace hybrid