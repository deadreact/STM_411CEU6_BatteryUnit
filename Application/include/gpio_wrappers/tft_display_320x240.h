/*
 * tft_display_320x240.h
 *
 *  Created on: Apr 19, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef SRC_GPIO_WRAPPERS_TFT_DISPLAY_320X240_H_
#define SRC_GPIO_WRAPPERS_TFT_DISPLAY_320X240_H_

#include <gpio_wrappers/interface.h>


class TFTDisplay320x240 : protected SinglePinElement, public ITickHandler
{
public:
	using SinglePinElement::SinglePinElement;

	virtual void onTick() override;

	inline bool isOn() const { return readPin(); }

	void toggle();
	void on() { if (!readPin()) toggle(); }
	void off() { if (readPin()) toggle(); }
private:
//	uint32_t m_
};

#endif /* SRC_GPIO_WRAPPERS_TFT_DISPLAY_320X240_H_ */
