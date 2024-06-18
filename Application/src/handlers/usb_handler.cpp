/*
 * usb_handler.cpp
 *
 *  Created on: Apr 19, 2024
 *      Author: Dmitriy.Gyr
 */

#include <handlers/usb_handler.h>

void USBHandler::onTick()
{
	m_btn.onTick();
	m_tube.onTick();
}

void USBHandler::handleEvents()
{
	m_btnHandler.handleEvents();
}

void USBHandler::setActive(bool active)
{
	if (m_isActive != active)
	{
		m_isActive = active;
		if (!active) {
			m_usbOn.writePin(GPIO_PIN_RESET);
		}
	}
}
