/*
 * revision_data.h
 *
 *  Created on: Apr 20, 2024
 *      Author: deadreact
 */

#ifndef INCLUDE_REVISION_DATA_H_
#define INCLUDE_REVISION_DATA_H_

#include <stdint.h>

template <typename T>
class RevisionData : public T
{
public:
	RevisionData() = default;
	RevisionData(const T& other)
		: T(other)
		, m_revision(1)
	{}

	RevisionData& operator=(const T& other) {
		T::operator=(other);
		++m_revision;
		return *this;
	}

	uint32_t getDataRevision() const { return m_revision; }
private:
	uint32_t m_revision{0};
};


#endif /* INCLUDE_REVISION_DATA_H_ */
