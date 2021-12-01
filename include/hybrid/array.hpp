#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>
#include <vector>

namespace hybrid {
	template <typename T, size_t N = 0>
	class array : private std::array<T, N> {
	private:
		using parent = std::array<T, N>;
		size_t m_size = 0;

	protected:
		using parent::at;

	public:
		constexpr array() = default;

		template <size_t S, template <typename, size_t> class C>
		constexpr array(const C<T, S>& other) : std::array<T, N>() {
			if (other.size() > N)
				throw std::invalid_argument{"statically allocated array can't hold more than its capacity!"};
			std::copy(other.begin(), other.end(), this->begin());
			m_size = other.size();
		}

		constexpr array(std::initializer_list<T> il) : std::array<T, N>() {
			if (il.size() > N)
				throw std::invalid_argument{"statically allocated array can't hold more than its capacity!"};
			std::copy(il.begin(), il.end(), this->begin());
			m_size = il.size();
		}

		constexpr T& operator[](size_t index) {
			if (index >= m_size)
				throw std::out_of_range{"cannot access element past size of array!"};
			return parent::operator[](index);
		}

		constexpr const T& operator[](size_t index) const {
			if (index >= m_size)
				throw std::out_of_range{"cannot access element past size of array!"};
			return parent::operator[](index);
		}

		[[nodiscard]] constexpr size_t size() const { return m_size; }
		[[nodiscard]] constexpr bool empty() const { return m_size == 0; }
		[[nodiscard]] constexpr size_t max_size() const { return N; }
		[[nodiscard]] constexpr size_t capacity() const { return N; }

		using parent::front;
		constexpr T& back() { return operator[](m_size - 1); }

		using parent::begin;
		constexpr auto end() { return begin() + m_size; }
		constexpr const auto end() const { return begin() + m_size; }

		constexpr void clear() { m_size = 0; }
		constexpr void push_back(const T& val) { operator[](m_size++) = val; }
		constexpr void insert(int index, const T& val) {
			if (index > m_size)
				throw std::out_of_range{"cannot insert element past end of array!"};
			else if (m_size >= N)
				throw std::invalid_argument{"statically allocated array can't hold more than its capacity!"};

			m_size++;
			if (index + 1 < m_size) std::copy(begin() + index, end(), begin() + index + 1);
			(*this)[index] = val;
		}
		constexpr void insert(parent::iterator iter, const T& val) {
			insert(std::distance(begin(), iter), val);
		}
		constexpr void resize(size_t new_size) {
			if (new_size > N)
				throw std::invalid_argument{"can't resize statically allocated array to more than its capacity!"};
			m_size = new_size;
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