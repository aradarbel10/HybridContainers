#pragma once

#include "array.hpp"

namespace hybrid {
    namespace impl {
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
                    if (this->at(i) == val)
                        return true;
                }
                return false;
            }

            constexpr bool erase(const ValT& val) {
                size_t size_before = size();
                this->std::vector<ValT>::erase(std::remove(begin(), end(), val));
                return size() != size_before;
            }

            constexpr auto find(const ValT& val) { return std::find(begin(), end(), val); }
        };

        template <typename T, size_t S1, size_t S2>
        constexpr bool operator==(const impl_set<T, S1>& lhs, const impl_set<T, S2>& rhs) {
            if (lhs.size() != rhs.size())
                return false;
            return std::is_permutation(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
        }
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