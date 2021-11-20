#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <span>
#include <string_view>
#include <vector>

namespace hybrid {
    // container
    template <typename T, size_t N = 0>
    class array : public std::array<T, N> {
    public:
		constexpr array() = default;

		constexpr array(std::initializer_list<T> il) : std::array<T, N>(il) {}

        constexpr array(const array<T, 0>& other) {
            if (other.size() != N)
                throw std::invalid_argument{"sizes must match!"};
            std::copy(other.begin(), other.end(), this->begin());
        }

        constexpr array(std::vector<T> other) {
            if (other.size() != N)
                throw std::invalid_argument{"sizes must match!"};
            std::copy(other.begin(), other.end(), this->begin());
        }
    };

    template <typename T>
    class array<T, 0> : public std::vector<T> {
    public:
        constexpr array() = default;

        constexpr array(std::initializer_list<T> il) : std::vector<T>(il) {}

        template <size_t N>
        constexpr array(const array<T, N>& other) {
            this->resize(N);
            std::copy(other.begin(), other.end(), this->begin());
        }

		constexpr array(std::vector<T> other) : std::vector<T>(other) { }
    };

    template <typename T, size_t N1, size_t N2>
    constexpr bool operator==(const array<T, N1>& lhs, const array<T, N2>& rhs) {
        if (lhs.size() != rhs.size())
            return false;

		auto it1 = lhs.begin();
		auto it2 = rhs.begin();
        for (;it1 != lhs.end(); it1++, it2++) {
            if (*it1 != *it2)
                return false;
        }
        return true;
    }

	// allow casting hybrid containers from compile-time types to run-time
    template <typename T, size_t N>
    constexpr auto cast(const array<T, N>& arr) {
        return array<T, 0>(arr);
    }

    // get size of result at compile-time
    template <typename F, typename... Args>
    consteval size_t compute_size(F func, Args... args) {
        auto result = func(args...);
        return result.size();
    }

    // given size, performs calculation
    template <size_t Size, class F, class... Args>
    consteval auto compute_val(F func, Args... args) {
		auto calc = array<int>{ func(args...) };
		array<int, Size> arr{ calc };
        return arr;
    }

} // namespace hybrid

// put it all together
#define hybrid_compute(func, ...)                                                                  \
    hybrid::compute_val < []() consteval { return hybrid::compute_size(&func, __VA_ARGS__); }      \
    () > (&func, __VA_ARGS__)