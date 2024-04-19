/*
 * io_tube.cpp
 *
 *  Created on: Apr 17, 2024
 *      Author: Dmitriy.Gyr
 */

#include <gpio_wrappers/io_tube.h>

IOTube::IOTube(const SinglePinElement& in, const SinglePinElement& out, bool inverted)
		: m_in(in)
		, m_out(out)
		, m_inverted(inverted ? 1 : 0)
	{}

 void IOTube::onTick()
 {
	 m_out.writePin(GPIO_PinState(m_in.readPin() ^ m_inverted));
 }
