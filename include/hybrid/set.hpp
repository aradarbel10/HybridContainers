#pragma once

#include "array.hpp"

namespace hybrid {
	namespace impl {
		template <typename ValT, size_t N = 0>
		requires std::totally_ordered<ValT>
		class impl_set : protected hybrid::array<ValT, N> {
		private:
			using iterator = hybrid::array<ValT, N>::iterator;

			using hybrid::array<ValT, N>::at;

			constexpr int binary_search(const ValT& val, int first, int last) const {
				if (last == first)
					return first;

				auto mid = first + static_cast<int>((last - first) / 2);
				if (val < at(mid)) {
					return binary_search(val, first, mid);
				} else if (val > at(mid)) {
					return binary_search(val, mid + 1, last);
				} else { // ==
					return mid;
				}
			}

			constexpr int binary_search(const ValT& val) {
				return binary_search(val, 0, size());
			}

		public:
			constexpr impl_set() { }

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

			constexpr bool insert(const ValT& val) {
				int index = binary_search(val);
				if (index < size() && val == at(index)) {
					return false;
				}
				hybrid::array<ValT, N>::insert(begin() + index, val);
				return true;
			}

			constexpr bool contains(const ValT& val) const {
				int index = binary_search(val);
				return at(index) == val;
			}

			constexpr bool erase(const ValT& val) {
				int index = binary_search(val);
				if (index < size() && val == at(index)) {
					std::vector<ValT>::erase(begin() + index);
					return true;
				}
				return false;
			}

			constexpr auto find(const ValT& val) {
				int index = binary_search(val);
				return ((index < size() && at(index) == val) ? begin() + index : end());
			}
		};

		//template <typename T, size_t S1, size_t S2>
		//constexpr bool operator==(const impl_set<T, S1>& lhs, const impl_set<T, S2>& rhs) {
		//	if (lhs.size() != rhs.size())
		//		return false;
		//	return std::is_permutation(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
		//}
	} // namespace impl

	template <typename T, size_t N = 0>
	class set : public impl::impl_set<T, N> {
	public:
		constexpr set() = default;

		template <size_t S, template <typename, size_t> class C>
		constexpr set(const C<T, S>& other) {
			if (other.size() != N)
				throw std::invalid_argument{"sizes must match!"};
			std::copy(other.begin(), other.end(), this->begin());
		}

		constexpr set(std::initializer_list<T> il) {
			if (il.size() != N)
				throw std::invalid_argument{"sizes must match!"};
			std::copy(il.begin(), il.end(), this->begin());
		}
	};

	template <typename T>
	class set<T, 0> : public impl::impl_set<T> {
	public:
		constexpr set() = default;

		constexpr set(std::initializer_list<T> il) : impl::impl_set<T>(il) {}

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
} // namespace hybrid