/*
 * application_utils.h
 *
 *  Created on: Apr 20, 2024
 *      Author: deadreact
 */

#ifndef INCLUDE_APPLICATION_UTILS_H_
#define INCLUDE_APPLICATION_UTILS_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

namespace detail
{
	template <typename T>
	class TimeoutBase
	{
	public:
		inline void reset() { startTick = HAL_GetTick(); }
		inline void inc() { startTick += timeout; }

		inline T getTimeout() const { return timeout; }
		inline uint32_t getStartTick() const { return startTick; }
		inline uint32_t getDuration() const { return HAL_GetTick() - startTick; }
		inline bool isReached() const { return getDuration() > timeout; }
	protected:
		TimeoutBase(T timeout): timeout(timeout) {}

		uint32_t startTick{0};
		T timeout;
	};
} //namespace detail

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

class Timeout : public detail::TimeoutBase<uint32_t>
{
public:
	Timeout(uint32_t t = 0xffffffff): TimeoutBase(t) {}
	Timeout& operator=(uint32_t t) { timeout = t; return *this; }

	using TimeoutBase::reset;
	inline void reset(uint32_t t) { timeout = t; reset(); }
	inline void invalidate() { timeout = 0xffffffff; }
	inline bool isValid() const { return timeout < 0xffffffff; }
};

class CTimeout : public detail::TimeoutBase<const uint32_t>
{
public:
	CTimeout(uint32_t t): TimeoutBase(t) {}
};

template <uint32_t timeout>
class StaticTimeout
{
	static_assert(timeout > 0);
public:
	inline void reset() { startTick = HAL_GetTick(); }
	inline void inc() { startTick += timeout; }

	inline bool isReached() const { return (HAL_GetTick() - startTick) > timeout; }
	inline constexpr uint32_t getTimeout() const { return timeout; }
protected:
	uint32_t startTick{0};
};


#endif /* INCLUDE_APPLICATION_UTILS_H_ */
