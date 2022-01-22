#pragma once

#include "set.hpp"

namespace hybrid {
	namespace impl {
		template <typename T, typename S>
		struct less_by_key {
			constexpr bool operator()(const std::pair<T, S>& lhs, const std::pair<T, S>& rhs) { return lhs.first < rhs.first; }
		};

		template <typename KeyT, typename ValT, size_t N = 0>
		class impl_map {
		private:

			hybrid::impl::impl_set<KeyT, N> keys{};
			hybrid::array<ValT, N> vals{};

		protected:
			constexpr impl_map() = default;
			constexpr impl_map(std::initializer_list<std::pair<KeyT, ValT>> il) {
				for (const auto& pair : il) {
					auto index = keys.binary_search(pair.first);

					keys.insert(index, pair.first);
					vals.insert(index, pair.second);
				}
			}

			template <size_t S, template <typename, typename, size_t> class C>
			constexpr impl_map(const C<KeyT, ValT, S>& other) {
				auto i = other.begin();
				while (i != other.end()) {
					auto index = keys.binary_search(i->first);

					keys.insert(index, i->first);
					vals.insert(index, i->second);

					++i;
				}
			}

		public:

			struct iterator {
				using iterator_category = std::contiguous_iterator_tag;
				using difference_type = std::ptrdiff_t;
				using value_type = std::pair<KeyT, ValT>;
				using pointer = value_type*;
				using reference = value_type&;

				constexpr iterator(size_t i) : index(i) { }

				constexpr reference operator*() const { return { keys[index], vals[index] }; }
				constexpr pointer operator->() { std::make_pair{&keys[index], &vals[index]}; }

				constexpr iterator& operator++() {
					index++;
					return *this;
				}

				constexpr iterator operator++(int) {
					iterator tmp = *this;
					++(*this);
					return tmp;
				}

				constexpr friend bool operator==(const iterator& lhs, const iterator& rhs) { return lhs.index == rhs.index; }

			private:

				size_t index;

			};

			using parent::empty;
			using parent::end;
			using parent::size;
			using parent::insert;

			constexpr auto begin() const { return iterator{0}; }
			constexpr auto end() const { return iterator{keys.size() - 1} };

			constexpr bool empty() { return keys.empty(); }
			constexpr size_t size() { return keys.size(); }

			constexpr auto insert(const std::pair<KeyT, ValT>& pair) {
				int index = keys.binary_search(pair.first);
				if (index < size() && keys.vals_eq(pair.first, keys[index])) {
					return false;
				}

				keys.insert(keys.begin() + index, pair.first);
				vals.insert(vals.begin() + index, pair.second);

				return true;
			}

			constexpr bool insert(const KeyT& key, const ValT& val) {
				auto pair = std::pair<KeyT, ValT>(key, val);
				return parent::insert(pair);
			}

			constexpr bool contains(const KeyT& key) const {
				return keys.contains(key);
			}

			constexpr bool erase(const KeyT& key) {
				int index = keys.binary_search(key);

				if (index >= size()) {
					return false;
				}

				keys.erase(key);
				vals.erase()

				std::pair<KeyT, ValT> pair;
				pair.first = key;
				return parent::erase(pair);

				return true;
			}

			constexpr ValT& operator[](const KeyT& key) {
				std::pair<KeyT, ValT> pair;
				pair.first = key;
				auto iter = parent::find(pair);

				if (iter == end())
					throw std::out_of_range{"key not found"};
				else
					return iter->second;
			}

			constexpr const ValT& operator[](const KeyT& key) const {
				std::pair<KeyT, ValT> pair;
				pair.first = key;
				auto iter = parent::find(pair);

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
		constexpr map(const C<KeyT, ValT, S>& other) : impl::impl_map<KeyT, ValT, N>(other) {}
		constexpr map(std::initializer_list<std::pair<KeyT, ValT>> il) : impl::impl_map<KeyT, ValT, N>(il) {}
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