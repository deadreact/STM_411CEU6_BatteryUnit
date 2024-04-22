/*
 * tft_display_320x240.h
 *
 *  Created on: Apr 19, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef SRC_GPIO_WRAPPERS_TFT_DISPLAY_320X240_H_
#define SRC_GPIO_WRAPPERS_TFT_DISPLAY_320X240_H_

#include <gpio_wrappers/interface.h>
#include <utils/timeout.h>
#include <inverter/inverter_handler.h>


class TFTDisplay320x240 : protected SinglePinElement, public ITickHandler
{
public:
	using SinglePinElement::SinglePinElement;

	virtual void onTick() override;

	inline bool isOn() const { return readPin(); }

	void toggle();
	void on() { if (!isOn()) toggle(); }
	void off() { if (isOn()) toggle(); }
private:
//	uint32_t m_
};

struct DisplaySensitiveData
{
	int screenId{1};
	InverterState invState{InverterState::Off};
	bool usbState{false};
	bool chargerPlugged{false};

	inline bool operator==(const DisplaySensitiveData& other) const {
		return screenId == other.screenId
			&& invState == other.invState
			&& usbState == other.usbState
			&& chargerPlugged == other.chargerPlugged; }

	inline bool operator!=(const DisplaySensitiveData& other) const { return !operator==(other); }
};

class Display320x240 : public ITickHandler
{
	constexpr static const uint16_t kOff = 0;
	constexpr static const uint16_t kOnRelaxed = 450;
	constexpr static const uint16_t kOn = 900;
public:
	Display320x240() { TIM2->CCR3 = m_brightness; }
	virtual void onTick() override;

	inline bool isOn() const { return m_brightness == kOn; }
	inline bool isOff() const { return m_brightness == kOff; }

	void toggle();
	void on();
	void off();
private:
	CTimeout m_brightnessTimeout{30000};
	uint16_t m_brightness{kOn};
	DisplaySensitiveData m_sensData;
};

#endif /* SRC_GPIO_WRAPPERS_TFT_DISPLAY_320X240_H_ */
