#pragma once

#include "set.hpp"

namespace hybrid {
	namespace impl {
		template <typename T, typename S>
		struct less_by_key {
			constexpr bool operator()(const std::pair<T, S>& lhs, const std::pair<T, S>& rhs) { return lhs.first < rhs.first; }
		};

		template <typename KeyT, typename ValT, size_t N = 0>
		class impl_map : protected hybrid::impl::impl_set<std::pair<KeyT, ValT>, less_by_key<KeyT, ValT>, N> {
		private:
			using parent_type = hybrid::impl::impl_set<std::pair<KeyT, ValT>, less_by_key<KeyT, ValT>, N>;

		protected:
			constexpr impl_map() = default;
			constexpr impl_map(std::initializer_list<std::pair<KeyT, ValT>> il) : parent_type(il) {}

		public:
			using parent_type::begin;
			using parent_type::empty;
			using parent_type::end;
			using parent_type::size;

			constexpr bool insert(const KeyT& key, const ValT& val) { return parent_type::insert({key, val}); }

			constexpr bool contains(const KeyT& key) const {
				std::pair<KeyT, ValT> pair;
				pair.first = key;
				return parent_type::contains(pair);
			}

			constexpr bool erase(const KeyT& key) {
				std::pair<KeyT, ValT> pair;
				pair.first = key;
				return parent_type::erase(pair);
			}

			constexpr ValT& operator[](const KeyT& key) {
				std::pair<KeyT, ValT> pair;
				pair.first = key;
				auto iter = parent_type::find(pair);

				if (iter == end())
					throw std::out_of_range{"key not found"};
				else
					return iter->second;
			}
		};
	} // namespace impl

	template <typename KeyT, typename ValT, size_t N = 0>
	class map : public impl::impl_map<KeyT, ValT, N> {
	public:
		constexpr map() = default;

		template <size_t S, template <typename, typename, size_t> class C>
		constexpr map(const C<KeyT, ValT, S>& other) {
			if (other.size() != N)
				throw std::invalid_argument{"sizes must match!"};
			std::copy(other.begin(), other.end(), this->begin());
		}

		constexpr map(std::initializer_list<std::pair<KeyT, ValT>> il) {
			if (il.size() != N)
				throw std::invalid_argument{"sizes must match!"};
			std::copy(il.begin(), il.end(), this->begin());
		}
	};

	template <typename KeyT, typename ValT>
	class map<KeyT, ValT, 0> : public impl::impl_map<KeyT, ValT> {
	public:
		constexpr map() = default;

		constexpr map(std::initializer_list<std::pair<KeyT, ValT>> il) : impl::impl_map<KeyT, ValT>(il) { }

		template <size_t N>
		constexpr map(const map<KeyT, ValT, N>& other) {
			this->resize(N);
			std::copy(other.begin(), other.end(), this->begin());
		}

	};

	template <typename KeyT, typename ValT, size_t N>
	constexpr auto lift(const hybrid::map<KeyT, ValT, N>& container) {
		return container;
	}

	template <typename T1, typename T2, size_t N1, size_t N2>
	constexpr bool operator==(const map<T1, T2, N1>& lhs, const map<T1, T2, N2>& rhs) {
		if (lhs.size() != rhs.size())
			return false;

		auto it1 = lhs.begin();
		auto it2 = rhs.begin();
		for (; it1 != lhs.end(); it1++, it2++) {
			if (impl::less_by_key<T1, T2>{}(*it1, *it2) || impl::less_by_key<T1, T2>{}(*it2, *it1))
				return false;
		}
		return true;
	}
} // namespace hybrid