/*
 * stack_string.h
 *
 *  Created on: Apr 25, 2024
 *      Author: deadreact
 */

#ifndef INCLUDE_UTILS_STACK_STRING_H_
#define INCLUDE_UTILS_STACK_STRING_H_

#include <utils/stack_vector.h>

namespace utils
{

class stack_string : protected stack_vector<char, 64>
{
public:
	using type::begin;
	using type::cbegin;
public:
	stack_string(): stack_vector(1, 0) {}
	stack_string(const char* str) { assert(strlen(str) < capacity()); strcpy(m_data, str); };
	stack_string& operator=(const char* str) { assert(strlen(str) < capacity()); strcpy(m_data, str); return *this; }

	stack_string(const stack_string& str) { strcpy(m_data, str.m_data); };
	stack_string& operator=(const stack_string& str) { return *this = str.c_str(); }

	inline pointer end() { return m_data + size(); }
	inline const_pointer end() const { return m_data + size(); }
	inline const_pointer cend() const { return m_data + size(); }

	stack_string& operator+=(const stack_string& other) {
		assert(size() + other.size() < capacity());
		strcpy(m_data + size(), other.m_data);
		return *this;
	}

	stack_string& operator+=(const char* str) {
		assert(size() + strlen(str) < capacity());
		strcpy(m_data + size(), str);
		return *this;
	}

	bool operator==(const stack_string& str) const { return strcmp(m_data, str.m_data) == 0; }
	bool operator!=(const stack_string& str) const { return !operator==(str); }
	bool operator==(const char* str) const { return strcmp(m_data, str) == 0; }
	bool operator!=(const char* str) const { return !operator==(str); }

	size_type size() const { return type::size() - 1; }
	const char* c_str() const { return cbegin(); }
};


} //namespace utils

#endif /* INCLUDE_UTILS_STACK_STRING_H_ */
