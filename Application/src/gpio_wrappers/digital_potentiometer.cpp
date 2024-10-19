/*
 * digital_potentiometer.cpp
 *
 *  Created on: Oct 12, 2024
 *      Author: Dmitriy.Gyr
 */

#include <gpio_wrappers/digital_potentiometer.h>


class Lock
{
public:
	static Lock& get()
	{
		static Lock lock;
		return lock;
	}

	bool tryLock(const DigitalPotentiometer* locker)
	{
		if (m_locker == nullptr)
		{
			m_locker = locker;
		}
		return m_locker == locker;
	}

	bool tryUnlock(const DigitalPotentiometer* locker)
	{
		if (m_locker == locker)
		{
			m_locker = nullptr;
		}
		return m_locker == nullptr;
	}
private:
	Lock() = default;

	const DigitalPotentiometer* m_locker{nullptr};
};


void DigitalPotentiometer::onTick()
{
	if (m_csState == CSState::Deselection && m_deselectionTimeout.isReached())
	{
		Lock::get().tryUnlock(this);
		m_csState = CSState::Unselected;
	}

	if (m_goalValue != m_currentValue)
	{
		changeValue(m_goalValue > m_currentValue);
	}
	else
	{
		deselect();
	}
}

void DigitalPotentiometer::changeValue(bool increase)
{
	if (select() && m_incTimeout.isReached())
	{
		m_potUD.writePin((increase ^ m_inversedUD) ? GPIO_PIN_RESET : GPIO_PIN_SET);
		m_potINC.togglePin();
		m_incTimeout.reset();

		if (m_potINC.readPin() == GPIO_PIN_RESET)
		{
			m_currentValue += increase ? 1 : -1;
		}
	}
}

bool DigitalPotentiometer::select()
{
	if (m_csState == CSState::Unselected)
	{
		if (Lock::get().tryLock(this))
		{
			m_potCS.writePin(GPIO_PIN_RESET);
			m_csState = CSState::Selected;
		}
	}
	return m_csState == CSState::Selected;
}

void DigitalPotentiometer::deselect(bool store)
{
	if (m_csState == CSState::Selected)
	{
		m_potINC.writePin(store ? GPIO_PIN_SET : GPIO_PIN_RESET);
		m_potCS.writePin(GPIO_PIN_SET);
		m_csState = CSState::Deselection;
		m_deselectionTimeout.reset();
	}
}

void DigitalPotentiometer::setValue(uint8_t value)
{
	if (m_goalValue != value)
	{
		m_goalValue = value;
		if (value == 100)
		{
			m_currentValue = 0;
		}
		else if (value == 0)
		{
			m_currentValue = 100;
		}
	}
}

