/*
 * bms_handler.h
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#ifndef BMS_BMS_HANDLER_H_
#define BMS_BMS_HANDLER_H_

#include <bms/bms_data.h>
#include <gpio_wrappers/interface.h>
#include <string>

#define SIMULATE_CHARGING 0
#define SIMULATE_UNCHARGING 0

class BMSHandler
{
public:
    BMSHandler();
    ~BMSHandler();

    bool isPowerOn() const { return !m_bmsPwrStatus.readPin(); }
    bool isPowerRequested() const { return m_bmsPwrRequest.readPin(); }

    void Request(uint8_t* frameData, uint16_t frameLen);

    bool RequestTurnOn();
    bool RequestTurnOff();
    void RequestAllData();
    void RequestData(uint8_t dataId);

    void Response(HAL_StatusTypeDef status);

    BMSStatus GetStatus() const { return m_status; }
    const BatteryData& GetData() const { return m_data; }

    std::string debugMsg;
    uint32_t errFlags{0};

    constexpr static const int rxDataLen = 280; // 279 bytes needed for 12 cells
    constexpr static const int txDataLen = 21;
protected:
    void UpdateData(const BatteryData& newData);
protected:
    SinglePinElement m_bmsPwrRequest{GPIOA, GPIO_PIN_8};
    SinglePinElement m_bmsPwrStatus{GPIOB, GPIO_PIN_7};
    BMSStatus m_status { BMSStatus::NoStatus };
    BatteryData m_data;
    uint32_t m_lastRequestTick{0};
    uint32_t m_lastResponseTick{0};
    uint32_t m_lastDataUpdateTick{0};
    uint32_t m_bmsOnResetTick{0};

    uint8_t rxData[rxDataLen];
    uint8_t txDataBuffer[txDataLen];
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
    void UpdateAndStop();
    BMSUpdaterEvent GetLastEvent() const { return m_lastEvent; }

private:
    BMSStatus m_prevStatus { BMSStatus::NoStatus };
    BMSUpdaterEvent m_lastEvent { BMSUpdaterEvent::NoEvent };
    const uint32_t m_requestTimeout{1000};
    const uint32_t m_invalidatePeriodMsec{500}; //
    const uint32_t m_validResponseTimeout{10000}; //
};


#endif /* BMS_BMS_HANDLER_H_ */
