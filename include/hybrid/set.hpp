#pragma once

#include "array.hpp"

namespace hybrid {
	namespace impl {
		template <typename ValT, class Ord = std::less<ValT>, size_t N = 0>
		class impl_set : protected hybrid::array<ValT, N> {
		private:
			using hybrid::array<ValT, N>::at;

			constexpr int binary_search(const ValT& val, int first, int last) const {
				if (last == first)
					return first;

				auto mid = first + static_cast<int>((last - first) / 2);
				if (Ord{}(val, at(mid))) {
					return binary_search(val, first, mid);
				} else if (Ord{}(at(mid), val)) {
					return binary_search(val, mid + 1, last);
				} else { // ==
					return mid;
				}
			}

			constexpr int binary_search(const ValT& val) { return binary_search(val, 0, size()); }

			constexpr static bool vals_eq(const ValT& lhs, const ValT& rhs) { return !Ord{}(lhs, rhs) && !Ord{}(rhs, lhs); }

		public:
			constexpr impl_set() = default;

			constexpr impl_set(auto first, auto last) : hybrid::array<ValT, N>() {
				for (auto i = first; i != last; i++) {
					insert(*i);
				}
			}

			template <size_t S, template <typename, size_t> class C>
			constexpr impl_set(const C<ValT, S>& other) {
				auto i = other.begin();
				while (i != other.end()) {
					insert(*i);
					i++;
				}
			}

			constexpr impl_set(std::initializer_list<ValT> il) : impl_set(il.begin(), il.end()) {}

			using hybrid::array<ValT, N>::size;
			using hybrid::array<ValT, N>::empty;
			using hybrid::array<ValT, N>::begin;
			using hybrid::array<ValT, N>::end;

			constexpr bool insert(const ValT& val) {
				int index = binary_search(val);
				if (index < size() && vals_eq(val, at(index))) {
					return false;
				}
				hybrid::array<ValT, N>::insert(begin() + index, val);
				return true;
			}

			constexpr bool contains(const ValT& val) const {
				int index = binary_search(val);
				return index < size() && vals_eq(val, at(index));
			}

			constexpr bool erase(const ValT& val) {
				int index = binary_search(val);
				if (index < size() && vals_eq(val, at(index))) {
					std::vector<ValT>::erase(begin() + index);
					return true;
				}
				return false;
			}

			constexpr auto find(const ValT& val) {
				int index = binary_search(val);
				return ((index < size() && vals_eq(val, at(index))) ? begin() + index : end());
			}
		};
	} // namespace impl

	template <typename T, size_t N = 0>
	class set : public impl::impl_set<T, std::less<T>, N> {
	public:
		constexpr set() = default;

		template <size_t S, template <typename, size_t> class C>
		constexpr set(const C<T, S>& other) : impl::impl_set<T, std::less<T>, N>(other) {}

		constexpr set(std::initializer_list<T> il) : impl::impl_set<T, std::less<T>, N>(il) {}

	};

	template <typename T, size_t N>
	constexpr auto lift(const hybrid::set<T, N>& container) {
		return container;
	}

	template <typename T, size_t N1, size_t N2>
	constexpr bool operator==(const set<T, N1>& lhs, const set<T, N2>& rhs) {
		if (lhs.size() != rhs.size())
			return false;

		auto it1 = lhs.begin();
		auto it2 = rhs.begin();
		for (; it1 != lhs.end(); it1++, it2++) {
			if (std::less{}(*it1, *it2) || std::less{}(*it2, *it1))
				return false;
		}
		return true;
	}
} // namespace hybrid