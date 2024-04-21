/*
 * application_utils.h
 *
 *  Created on: Apr 20, 2024
 *      Author: deadreact
 */

#ifndef INCLUDE_APPLICATION_UTILS_H_
#define INCLUDE_APPLICATION_UTILS_H_


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

class Timeout
{
public:
	Timeout(uint16_t t = 0): timeout(t) {}
	Timeout& operator=(uint16_t t) { timeout = t; return *this; }

	inline void reset() { startTick = HAL_GetTick(); }
	inline void reset(uint16_t t) { timeout = t; reset(); }
	inline void inc() { startTick += timeout; }

	inline bool isReached() const { return (HAL_GetTick() - startTick) > timeout; }
	inline uint16_t getTimeout() const { return timeout; }
protected:
	uint32_t startTick{0};
	uint16_t timeout{0};
};

template <uint16_t timeout>
class StaticTimeout
{
	static_assert(timeout > 0);
public:
	inline void reset() { startTick = HAL_GetTick(); }
	inline void inc() { startTick += timeout; }

	inline bool isReached() const { return (HAL_GetTick() - startTick) > timeout; }
	inline constexpr uint16_t getTimeout() const { return timeout; }
protected:
	uint32_t startTick{0};
};


#endif /* INCLUDE_APPLICATION_UTILS_H_ */
