#pragma once

#include <algorithm>
#include <array>
#include <vector>
#include <stdexcept>

namespace hybrid {
    // array container
    template <typename T, size_t N = 0>
    class array : public std::array<T, N> {
    public:
		constexpr array() = default;

		template <size_t S, template<typename, size_t> class C>
		constexpr array(const C<T, S>& other) {
			if (other.size() != N)
				throw std::invalid_argument{ "sizes must match!" };
			std::copy(other.begin(), other.end(), this->begin());
		}

		constexpr array(std::initializer_list<T> il) {
			if (il.size() != N)
				throw std::invalid_argument{ "sizes must match!" };
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

		constexpr array(std::vector<T> other) : std::vector<T>(other) { }
    };

	template <typename T>
	constexpr auto lift(const std::vector<T>& container) {
		return hybrid::array<T>(container);
	}

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


	// set container
	template <typename ValT, size_t N = 0>
	class impl_set : protected hybrid::array<ValT, N> {
	public:

		constexpr impl_set() = default;

		constexpr impl_set(std::initializer_list<ValT> il) {
			for (auto i = il.begin(); i != il.end(); i++) {
				insert(*i);
			}
		}

		using hybrid::array<ValT, N>::size;
		using hybrid::array<ValT, N>::empty;
		using hybrid::array<ValT, N>::begin;
		using hybrid::array<ValT, N>::end;
		using hybrid::array<ValT, N>::cbegin;
		using hybrid::array<ValT, N>::cend;
		using hybrid::array<ValT, N>::rbegin;
		using hybrid::array<ValT, N>::rend;

		constexpr void insert(const ValT& val) {
			if (!contains(val)) {
				this->push_back(val);
			};
		}

		constexpr bool contains(const ValT& val) const {
			for (int i = 0; i < size(); i++) {
				if (this->at(i) == val) return true;
			}
			return false;
		}

		constexpr bool erase(const ValT& val) {
			size_t size_before = size();
			this->std::vector<ValT>::erase(std::remove(begin(), end(), val));
			return size() != size_before;
		}

		constexpr auto find(const ValT& val) {
			return std::find(begin(), end(), val);
		}
	};

	template<typename T, size_t S1, size_t S2>
	constexpr bool operator==(const impl_set<T, S1>& lhs, const impl_set<T, S2>& rhs) {
		if (lhs.size() != rhs.size())
			return false;
		return std::is_permutation(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <typename T, size_t N = 0>
	class set : public impl_set<T, N> {
	public:
		constexpr set() = default;

		template <size_t S, template<typename, size_t> class C>
		constexpr set(const C<T, S>& other) {
			if (other.size() != N)
				throw std::invalid_argument{ "sizes must match!" };
			std::copy(other.begin(), other.end(), this->begin());
		}

		constexpr set(std::initializer_list<T> il) {
			if (il.size() != N)
				throw std::invalid_argument{ "sizes must match!" };
			std::copy(il.begin(), il.end(), this->begin());
		}
	};

	template <typename T>
	class set<T, 0> : public impl_set<T> {
	public:
		constexpr set() = default;

		constexpr set(std::initializer_list<T> il) : impl_set<T>(il) { }

		template <size_t N>
		constexpr set(const set<T, N>& other) {
			this->resize(N);
			std::copy(other.begin(), other.end(), this->begin());
		}
	};

	template <typename T, size_t N>
	constexpr auto lift(const hybrid::set<T, N>& container) {
		return container;
	}



	// allow casting hybrid containers from compile-time types to run-time
    template <size_t TargetSize = 0, size_t N, typename T, template<typename, size_t> class C>
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