/*
 * digital_potentiometer.h
 *
 *  Created on: Oct 12, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef INCLUDE_GPIO_WRAPPERS_DIGITAL_POTENTIOMETER_H_
#define INCLUDE_GPIO_WRAPPERS_DIGITAL_POTENTIOMETER_H_

#include <main.h> // GPIO defines & HAL
#include <gpio_wrappers/pin_wrapper.h>
#include <utils/timeout.h>

class DigitalPotentiometer
{
public:
	DigitalPotentiometer(GPIO_TypeDef* chipSelect_GPIOx, uint16_t chipSelect_GPIO_Pin, bool inversedUD = false)
		: m_potCS(chipSelect_GPIOx, chipSelect_GPIO_Pin)
		, m_inversedUD(inversedUD)
	{}

	void onTick();

	void setValue(uint8_t value);
	uint8_t getValue() const { return m_currentValue; }
private:
	bool select();
	void deselect(bool store = false);
	void changeValue(bool increase);

	PinWrapper m_potCS;
//	PinWrapper m_potINC{pot_INC_GPIO_Port, pot_INC_Pin};
//	PinWrapper m_potUD{pot_UD_GPIO_Port, pot_UD_Pin};

	uint8_t m_currentValue{100};
	uint8_t m_goalValue{0};

	const bool m_inversedUD;
};


//----------------------------------------------------------------

//class AsyncDigitalPotentiometer
//{
//public:
//	enum class CSState
//	{
//		Selected,
//		Deselection,
//		Unselected
//	};
//
//	AsyncDigitalPotentiometer(GPIO_TypeDef* chipSelect_GPIOx, uint16_t chipSelect_GPIO_Pin, bool inversedUD = false)
//		: m_potCS(chipSelect_GPIOx, chipSelect_GPIO_Pin)
//		, m_csState(m_potCS.readPin() ? CSState::Unselected : CSState::Selected)
//		, m_inversedUD(inversedUD)
//	{}
//
//	void onTick();
//
//	void setValue(uint8_t value);
//	uint8_t getValue() const { return m_currentValue; }
//private:
//	bool select();
//	void deselect(bool store = false);
//	void changeValue(bool increase);
//
//	PinWrapper m_potCS;
//	PinWrapper m_potINC{pot_INC_GPIO_Port, pot_INC_Pin};
//	PinWrapper m_potUD{pot_UD_GPIO_Port, pot_UD_Pin};
//
//	uint8_t m_currentValue{100};
//	uint8_t m_goalValue{0};
//	CTimeout m_incTimeout{1};
//	CTimeout m_deselectionTimeout{20};
//
//	CSState m_csState;
//
//	const bool m_inversedUD;
//};

#endif /* INCLUDE_GPIO_WRAPPERS_DIGITAL_POTENTIOMETER_H_ */
