/*
 * stack_vector.h
 *
 *  Created on: Apr 25, 2024
 *      Author: deadreact
 */

#ifndef INCLUDE_UTILS_STACK_VECTOR_H_
#define INCLUDE_UTILS_STACK_VECTOR_H_

#include <stdint.h>
#include <assert.h>
#include <cstring>
#include <type_traits>

namespace detail
{
    template <typename T, bool = std::is_integral<T>::value>
    struct helper
    {
        static bool is_eq(const T* arr1, const T* arr2, uint8_t size) {
            for (const T* end1 = arr1 + size; arr1 != end1; ++arr1, ++arr2) {
                if (*arr1 != *arr2) return false;
            }
            return true;
        }

        static void assign(T* arr, uint8_t n, const T& value = {}) {
            for (const T* end = arr + n; arr != end; ++arr) {
                *arr = value;
            }
        }

        static void copy(T* target, const T* source, uint8_t size) {
            for (const T* end = target + size; target != end; ++target, ++source) {
                *target = *source;
            }
        }
    };

    template <typename T>
    struct helper<T, true>
    {
        static bool is_eq(const T* arr1, const T* arr2, uint8_t size) {
            return memcmp(arr1, arr2, size * sizeof(arr1[0])) == 0;
        }

        static void assign(T* arr, uint8_t n, const T& value = 0) {
            memset(arr, value, n * sizeof(T));
        }

        static void copy(T* target, const T* source, uint8_t size) {
            memcpy(target, source, size * sizeof(target[0]));
        }
    };

} //namespace detail

namespace utils
{
    template <typename T, uint8_t capacity>
    class stack_vector
    {
        using helper_type = detail::helper<T>;
    public:
        using type = stack_vector<T, capacity>;
        using size_type = uint8_t;
        using value_type = T;
        using reference = T&;
        using const_reference = const T&;
        using pointer = T*;
        using const_pointer = const T*;
    public:

        stack_vector() = default;

        stack_vector(const type& other)
            : m_size(other.m_size)
        {
            helper_type::copy(m_data, other.m_data, m_size);
        }

        type& operator=(const type& other)
        {
            m_size = other.m_size;
            helper_type::copy(m_data, other.m_data, m_size);
            return *this;
        }

        stack_vector(type&& other)
            : m_size(other.m_size)
        {
            helper_type::copy(m_data, other.m_data, m_size);
            other.resize(0);
        }

        type& operator=(type&& other) {
            m_size = other.m_size;
            helper_type::copy(m_data, other.m_data, m_size);
            other.resize(0);
            return *this;
        }

        stack_vector(size_type n, const value_type& value) { assign(n, value); }
        stack_vector(const T* _begin, const T* _end)
            : m_size(_end - _begin)
        {
            helper_type::copy(m_data, _begin, m_size);
        }

        inline void push_back(const value_type& el) { assert(m_size < capacity); m_data[m_size++] = el; }
        inline void pop_back() { assert(m_size > 0); --m_size; }

        inline pointer insert(const_pointer pos, const value_type& el) {
            assert(pos <= cend() && m_size < capacity);

            pointer it = end();
            for (; it != pos; --it) {
                *it = *(it - 1);
            }
            ++m_size;
            *it = el;
            return it;
        }

        inline void insert(size_type index, const value_type& el) { insert(cbegin() + index); }

        inline pointer erase(const_pointer pos) {
            assert(cbegin() <= pos && pos < cend());

            --m_size;
            for (pointer it = const_cast<pointer>(pos); it != end(); ++it) {
                *it = *(it + 1);
            }
        }

        inline void erase(size_type index) { erase(cbegin() + index); }

        inline void assign(size_type n, const value_type& value) { resize(n > m_size ? n : m_size); helper_type::assign(m_data, n, value); }
        inline void clear() { helper_type::assign(m_data, m_size); resize(0); }

        inline reference at(size_type index) { assert(index < m_size); return m_data[index]; }
        inline const_reference at(size_type index) const { assert(index < m_size); return m_data[index]; }

        inline reference operator[](size_type index) { return at(index); }
        inline const_reference operator[](size_type index) const { return at(index); }

        inline reference front() { assert(!empty()); return *m_data; }
        inline const_reference front() const { assert(!empty()); return *m_data; }

        inline reference back() { assert(!empty()); return m_data[m_size - 1]; }
        inline const_reference back() const { assert(!empty()); return m_data[m_size - 1]; }

        inline pointer begin() { return m_data; }
        inline pointer end() { return m_data + m_size; }
        inline const_pointer begin() const { return m_data; }
        inline const_pointer end() const { return m_data + m_size; }
        inline const_pointer cbegin() const { return m_data; }
        inline const_pointer cend() const { return m_data + m_size; }

        inline bool operator==(const type& other) const {
            return m_size == other.m_size && helper_type::is_eq(begin(), other.begin(), m_size);
        }

        inline bool operator!=(const type& other) const { return !operator==(other); }

        inline void resize(size_type size) { assert(size <= capacity); m_size = size; }
        inline size_type size() const { return m_size; }
        inline bool empty() const { return m_size == 0; }
    private:
        value_type m_data[capacity]{};
        size_type m_size{0};
    };
} //namespace utils


#endif /* INCLUDE_UTILS_STACK_VECTOR_H_ */
