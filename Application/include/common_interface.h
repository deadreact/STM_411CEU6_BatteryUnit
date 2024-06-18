/*
 * interfaces.h
 *
 *  Created on: Jun 18, 2024
 *      Author: deadreact
 */

#ifndef INCLUDE_COMMON_INTERFACE_H_
#define INCLUDE_COMMON_INTERFACE_H_

class ITickHandler
{
public:
    virtual ~ITickHandler() {}
    virtual void onTick() = 0;
};

#endif /* INCLUDE_COMMON_INTERFACE_H_ */
