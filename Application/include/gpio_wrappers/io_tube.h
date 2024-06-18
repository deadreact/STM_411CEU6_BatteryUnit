/*
 * io_tube.h
 *
 *  Created on: Apr 17, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef SRC_GPIO_WRAPPERS_IO_TUBE_H_
#define SRC_GPIO_WRAPPERS_IO_TUBE_H_

#include <gpio_wrappers/pin_wrapper.h>

class IOTube
{
public:
	IOTube(const PinWrapper& in, const PinWrapper& out, bool inverted = false);

	void onTick();
private:
	const PinWrapper m_in;
	PinWrapper m_out;
	uint8_t m_inverted{0};
};

#endif /* SRC_GPIO_WRAPPERS_IO_TUBE_H_ */
