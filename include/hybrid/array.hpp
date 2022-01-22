#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>

namespace hybrid {
	template<typename T>
	class vector {
	private:
		T* m_data = nullptr;
		size_t m_size = 0;
		size_t m_cap = 0;

	public:
		constexpr vector() = default;

		constexpr friend void swap(vector<T>& lhs, vector<T>& rhs) {
			std::swap(lhs.m_data, rhs.m_data);
			std::swap(lhs.m_cap, rhs.m_cap);
			std::swap(lhs.m_data, rhs.m_data);
		}

		constexpr vector(const vector<T>& other) {
			m_size = other.m_size;
			m_cap = other.m_cap;

			if (other.m_data) {
				m_data = new T[m_cap]();
				std::memcpy(m_data, other.m_data, m_size);
			}
		}

		constexpr vector(vector<T>&& other) {
			swap(*this, other);
		}

		constexpr vector<T>& operator=(const vector<T>& other) {
			vector<T> temp(other);
			swap(*this, temp);
			return *this;
		}

		constexpr vector<T>& operator=(vector<T>&& other) {
			delete[] m_data;
			swap(*this, other);
		}

		constexpr ~vector() {
			delete[] m_data;
		}

		constexpr T& at(size_t index) {
			if (index >= m_size) throw std::out_of_range{ "vector index out of range!" };
			return m_data[index];
		}

		constexpr const T& at(size_t index) const {
			if (index >= m_size) throw std::out_of_range{ "vector index out of range!" };
			return m_data[index];
		}

		constexpr T& operator[](size_t index) { return at(index); }
		constexpr const T& operator[](size_t index) const { return at(index); }

		constexpr T& front() { return at(0); }
		constexpr const T& front() const { return at(0); }
		constexpr T& back() { return at(m_size - 1); }
		constexpr const T& back() const { return at(m_size - 1); }

		constexpr T* data() { return m_data; }
		constexpr const T* data() const { return m_data; }

		constexpr T* begin() { return m_data; }
		constexpr const T* begin() const { return m_data; }
		constexpr T* end() { return m_data + m_size * sizeof(T); }
		constexpr const T* end() const { return m_data + m_size * sizeof(T); }

		constexpr bool empty() { return m_size == 0; }
		constexpr size_t size() { return m_size; }
		constexpr size_t capacity() { return m_cap; }

		constexpr void reserve(size_t count) {
			if (count > m_cap) {
				T* old = m_data;

				m_data = new T[count]();
				std::memcpy(m_data, old, m_size);

				delete[] old;
			}
		}

		constexpr void clear() {
			if (m_data) delete[] m_data;
			m_size = 0;
			m_cap = 0;
		}

		constexpr void push_back(const T& elem) {
			if (m_size < m_cap) {
				*(m_data + m_size) = elem;
				++m_size;
			} else {
				
			}
		}

	};



	template <typename T, size_t N = 0>
	class array : private std::array<T, N> {
	private:
		using parent = std::array<T, N>;
		size_t m_size = 0;

	protected:
		using parent::at;

	public:

		using value_type = parent::value_type;

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

		constexpr array(std::input_iterator auto first, std::input_iterator auto last) {
			m_size = std::distance(first, last);
			if (m_size > N)
				throw std::invalid_argument{"statically allocated array can't hold more than its capacity!"};
			
			std::copy(first, last, this->begin());
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
		constexpr parent::iterator erase(parent::iterator iter) {

		}
	};

	template <typename T, size_t N>
	constexpr auto lift(const std::array<T, N>& container) {
		return hybrid::array<T, N>(container);
	}

	template <typename T>
	class array<T, 0> : public std::vector<T> {
	public:
		using value_type = std::vector<T>::value_type;

		constexpr array() = default;

		constexpr array(std::initializer_list<T> il) : std::vector<T>(il) {}

		template <size_t N>
		constexpr array(const array<T, N>& other) {
			this->resize(N);
			std::copy(other.begin(), other.end(), this->begin());
		}

		constexpr array(std::vector<T> other) : std::vector<T>(other) {}

		constexpr array(std::input_iterator auto first, std::input_iterator auto last) : std::vector<T>(first, last) {}
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