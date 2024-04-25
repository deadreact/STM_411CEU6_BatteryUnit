/*
 * timeout.h
 *
 *  Created on: Apr 22, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef INCLUDE_UTILS_TIMEOUT_H_
#define INCLUDE_UTILS_TIMEOUT_H_


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
		TimeoutBase(T t): timeout(t) {}

		uint32_t startTick{0};
		T timeout;
	};
} //namespace detail

class Timeout : public detail::TimeoutBase<uint32_t>
{
	constexpr static const uint32_t kInvalidTimeout = 0xffffffff;
public:
	Timeout(uint32_t t = kInvalidTimeout): TimeoutBase(t) {}
	Timeout& operator=(uint32_t t) { timeout = t; return *this; }

	using TimeoutBase::reset;
	inline void reset(uint32_t t) { timeout = t; reset(); }
	inline void invalidate() { timeout = kInvalidTimeout; }
	inline bool isValid() const { return timeout < kInvalidTimeout; }
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


#endif /* INCLUDE_UTILS_TIMEOUT_H_ */
