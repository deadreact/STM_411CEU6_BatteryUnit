/*
 * jk_bms.h
 *
 *  Created on: Apr 15, 2024
 *      Author: deadreact
 */

#ifndef BMS_JK_BMS_H_
#define BMS_JK_BMS_H_

#include <bms/bms_data.h>

namespace jk
{
// returns frame length
uint16_t fillFrame(uint8_t txData[], uint8_t command = 0x06/*Read all*/, const uint8_t* data = nullptr, uint16_t dataLen = 1);
// returns errCode
uint32_t parseFrame(BatteryData& data, uint8_t rxData[], uint16_t rxDataLen);

} //namespace jk


#endif /* BMS_JK_BMS_H_ */
