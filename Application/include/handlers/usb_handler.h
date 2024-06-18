/*
 * usb_handler.h
 *
 *  Created on: Apr 19, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef INCLUDE_HANDLERS_USB_HANDLER_H_
#define INCLUDE_HANDLERS_USB_HANDLER_H_

#include <gpio_wrappers/button.h>
#include <gpio_wrappers/io_tube.h>
#include <main.h> // GPIO defines & HAL

class USBHandler
{
public:
	void onTick();
	void handleEvents();

    void setActive(bool active);

    inline bool isOn() const { return m_usbOn.readPin(); }
private:

	ButtonEventProvider m_btn{bttn_usb_on_GPIO_Port, bttn_usb_on_Pin};
	PinWrapper m_usbOn{usb_on_GPIO_Port, usb_on_Pin};
	IOTube m_tube{m_usbOn, {bttn_usb_led_GPIO_Port, bttn_usb_led_Pin}};
	ButtonEventHandler m_btnHandler{&m_btn, [&]{ if (m_isActive) m_usbOn.togglePin(); }};

	bool m_isActive{true};
};

#endif /* INCLUDE_HANDLERS_USB_HANDLER_H_ */
