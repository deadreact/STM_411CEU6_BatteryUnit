/*
 * tft_display_320x240.cpp
 *
 *  Created on: Apr 19, 2024
 *      Author: Dmitriy.Gyr
 */

#include <gpio_wrappers/tft_display_320x240.h>
#include <ili9341.h>
#include <app_touchgfx.h>

void TFTDisplay320x240::onTick()
{
	if (readPin()) {
		MX_TouchGFX_Process();
	}
}

void TFTDisplay320x240::toggle()
{
	ILI9341_EnableSleepMode(readPin());
	togglePin();
}
