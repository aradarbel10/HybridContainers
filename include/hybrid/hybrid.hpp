#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>
#include <vector>

#include "array.hpp"
#include "set.hpp"

namespace hybrid {

	// comparison between arbitrary containers
	template <typename T, size_t N1, size_t N2, template<typename, size_t> class C>
	constexpr bool operator==(const C<T, N1>& lhs, const C<T, N2>& rhs) {
		if (lhs.size() != rhs.size())
			return false;

		auto it1 = lhs.begin();
		auto it2 = rhs.begin();
		for (; it1 != lhs.end(); it1++, it2++) {
			if (*it1 != *it2)
				return false;
		}
		return true;
	}

    // allow casting hybrid containers from compile-time types to run-time or vice versa
    template <size_t TargetSize = 0, size_t N, typename T, template <typename, size_t> class C>
    constexpr auto cast(const C<T, N>& container) {
        return static_cast<C<T, TargetSize>>(container);
    }

    // get size of result at compile-time
    template <typename F, typename... Args>
    requires std::invocable<F, Args...>
    consteval size_t compute_size(F func, Args... args) {
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

} // namespace hybrid

// put it all together
#define hybrid_compute(func, ...)                                                                  \
    hybrid::compute_val < []() consteval { return hybrid::compute_size(&func, __VA_ARGS__); }      \
    () > (&func, __VA_ARGS__)