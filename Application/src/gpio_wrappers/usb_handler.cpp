/*
 * usb_handler.cpp
 *
 *  Created on: Apr 19, 2024
 *      Author: Dmitriy.Gyr
 */

#include <gpio_wrappers/usb_handler.h>

void USBHandler::onTick()
{
	m_btn.onTick();
	m_tube.onTick();
}

void USBHandler::handleEvents()
{
	m_btnHandler.handleEvents();
}

