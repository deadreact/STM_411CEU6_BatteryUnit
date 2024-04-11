/*
 * bms_handler.h
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#ifndef BMS_BMS_HANDLER_H_
#define BMS_BMS_HANDLER_H_

#include "bms_data.h"
#include "stm32f4xx_hal.h"
#include <string>

class BMSHandler
{
public:
    BMSHandler();
    ~BMSHandler();

    void Request();
    void Response(HAL_StatusTypeDef status);

    BMSStatus GetStatus() const { return m_status; }
    const BatteryData& GetData() const { return m_data; }

    std::string debugMsg;
    uint32_t errFlags{0};

    constexpr static const int rxDataLen = 280; // 279 bytes needed for 12 cells
protected:
    void UpdateData(const BatteryData& newData);
protected:
    BMSStatus m_status { BMSStatus::NoStatus };
    BatteryData m_data;
    uint32_t m_lastRequestTick{0};
    uint32_t m_lastResponseTick{0};
    uint32_t m_lastDataUpdateTick{0};

    uint8_t rxData[rxDataLen];
};

enum class BMSUpdaterEvent : uint8_t
{
	NoEvent,
	Updated
};

class BMSUpdater : public BMSHandler
{
public:
	void Update();
	BMSUpdaterEvent GetLastEvent() const { return m_lastEvent; }

private:
	BMSStatus m_prevStatus { BMSStatus::NoStatus };
	BMSUpdaterEvent m_lastEvent { BMSUpdaterEvent::NoEvent };
	const uint32_t m_requestTimeout{1000};
	const uint32_t m_invalidatePeriodMsec{500}; //
	const uint32_t m_validResponseTimeout{10000}; //
};


#endif /* BMS_BMS_HANDLER_H_ */
