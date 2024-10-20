/*
 * tft_display_320x240.h
 *
 *  Created on: Apr 19, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef INCLUDE_HANDLERS_DISPLAY_HANDLER_H_
#define INCLUDE_HANDLERS_DISPLAY_HANDLER_H_

#include <common_defs.h>
#include <gpio_wrappers/pin_wrapper.h>
#include <handlers/inverter_handler.h>
#include <utils/timeout.h>

struct DisplaySensitiveData
{
	ScreenId screenId{ScreenId::DefaultScreen};
	InverterState invState{InverterState::Off};
	bool usbState{false};
	bool chargerPlugged{false};
	int brightnessSetting{100};

	inline bool operator==(const DisplaySensitiveData& other) const {
		return screenId == other.screenId
			&& invState == other.invState
			&& usbState == other.usbState
			&& chargerPlugged == other.chargerPlugged
			&& brightnessSetting == other.brightnessSetting; }

	inline bool operator!=(const DisplaySensitiveData& other) const { return !operator==(other); }
};


class Display320x240 : public ITickHandler
{
	constexpr static const uint16_t kOff = 0;
	constexpr static const uint16_t kOnRelaxed = 450;
	constexpr static const uint16_t kOn = 999;
public:
	Display320x240() { TIM2->CCR3 = m_brightness; }
	virtual void onTick() override;

	inline bool isOn() const { return m_brightness == kOn; }
	inline bool isOff() const { return m_brightness == kOff; }

	void toggle();
	void on();
	void off();

	void setActive(bool active);
private:
	CTimeout m_brightnessTimeout{30000};
	uint16_t m_brightness{kOn};
	DisplaySensitiveData m_sensData;
	bool m_isActive{true};
	bool m_guiActive{true};
};

#endif /* INCLUDE_HANDLERS_DISPLAY_HANDLER_H_ */
