/*
 * usb_handler.h
 *
 *  Created on: Apr 19, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef SRC_GPIO_WRAPPERS_USB_HANDLER_H_
#define SRC_GPIO_WRAPPERS_USB_HANDLER_H_

#include <gpio_wrappers/button.h>
#include <gpio_wrappers/io_tube.h>
#include <main.h> // GPIO defines & HAL

class USBHandler
{
public:
	void onTick();
	void handleEvents();
private:
	ButtonEventProvider m_btn{bttn_usb_on_GPIO_Port, bttn_usb_on_Pin};
	SinglePinElement m_usbOn{usb_on_GPIO_Port, usb_on_Pin};
//	IOTube m_tube{m_usbOn, {bttn_usb_led_GPIO_Port, bttn_usb_led_Pin}};
	IOTube m_tube{m_usbOn, {GPIOC, GPIO_PIN_13}};

	ButtonEventHandler m_btnHandler{&m_btn, [&]{ m_usbOn.togglePin(); }};
};

#endif /* SRC_GPIO_WRAPPERS_USB_HANDLER_H_ */
