/*
 * io_tube.h
 *
 *  Created on: Apr 17, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef SRC_GPIO_WRAPPERS_IO_TUBE_H_
#define SRC_GPIO_WRAPPERS_IO_TUBE_H_

#include <gpio_wrappers/interface.h>

class IOTube : public ITickHandler
{
public:
	IOTube(const SinglePinElement& in, const SinglePinElement& out, bool inverted = false);

	VIRTUAL void onTick() OVERRIDE;
private:
	SinglePinElement m_in;
	SinglePinElement m_out;
	uint8_t m_inverted{0};
};

#endif /* SRC_GPIO_WRAPPERS_IO_TUBE_H_ */
