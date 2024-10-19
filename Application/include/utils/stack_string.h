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
	using base_type = stack_vector<char, 64>;
public:
	using base_type::begin;
	using base_type::cbegin;
public:
	stack_string(): stack_vector(1, 0) {}
	stack_string(const char* str) {
		const auto new_size = strlen(str);
		assert(new_size < capacity());
		strcpy(m_data, str);
		set_size(new_size);
	};
	
	stack_string& operator=(const char* str) {
		const auto new_size = strlen(str);
		assert(new_size < capacity());

		strcpy(m_data, str);
		set_size(new_size);
		
		return *this;
	}

	stack_string(const stack_string& str) { set_size(str.size()); strcpy(m_data, str.m_data); };
	stack_string& operator=(const stack_string& str) {
		if (this == &str) {
			return *this;
		}
		return operator=(str.c_str());
	}

	inline pointer end() { return m_data + size(); }
	inline const_pointer end() const { return m_data + size(); }
	inline const_pointer cend() const { return m_data + size(); }

	stack_string& operator+=(const stack_string& other) {
		const auto new_size = size() + other.size();
		assert(new_size < capacity());

		strcpy(m_data + size(), other.m_data);
		set_size(new_size);
		return *this;
	}

	stack_string& operator+=(const char* str) {
		const auto new_size = size() + strlen(str);
		assert(new_size < capacity());

		strcpy(m_data + size(), str);
		set_size(new_size);
		return *this;
	}

	bool operator==(const stack_string& str) const { return strcmp(m_data, str.m_data) == 0; }
	bool operator!=(const stack_string& str) const { return !operator==(str); }
	bool operator==(const char* str) const { return strcmp(m_data, str) == 0; }
	bool operator!=(const char* str) const { return !operator==(str); }

	size_type size() const { return base_type::size() - 1; }
	const char* c_str() const { return cbegin(); }
private:
    void set_size(size_type size)
	{
		base_type::m_size = size + 1;
	}
};


} //namespace utils

#endif /* INCLUDE_UTILS_STACK_STRING_H_ */
