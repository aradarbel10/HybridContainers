#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>
#include <vector>

namespace hybrid {
    template <typename T, size_t N = 0>
    class array : public std::array<T, N> {
    public:
        constexpr array() = default;

        template <size_t S, template <typename, size_t> class C>
        constexpr array(const C<T, S>& other) {
            if (other.size() != N)
                throw std::invalid_argument{"sizes must match!"};
            std::copy(other.begin(), other.end(), this->begin());
        }

        constexpr array(std::initializer_list<T> il) {
            if (il.size() != N)
                throw std::invalid_argument{"sizes must match!"};
            std::copy(il.begin(), il.end(), this->begin());
        }
    };

    template <typename T, size_t N>
    constexpr auto lift(const std::array<T, N>& container) {
        return hybrid::array<T, N>(container);
    }

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

        constexpr array(std::vector<T> other) : std::vector<T>(other) {}
    };

    template <typename T>
    constexpr auto lift(const std::vector<T>& container) {
        return hybrid::array<T>(container);
    }

	template <typename T, size_t N1, size_t N2>
	constexpr bool operator==(const hybrid::array<T, N1>& lhs, const hybrid::array<T, N2>& rhs) {
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
} // namespace hybrid