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
#include <main.h> // GPIO defines & HAL
#include <utils/timeout.h>

#define SIMULATE_CHARGING 0
#define SIMULATE_UNCHARGING 0

class BMSHandler
{
public:
    BMSHandler();
    ~BMSHandler();

    bool isPowerOn() const { return m_bmsPwrStatus.readPin(); }
    bool isPowerRequested() const { return m_bmsPwrRequest.readPin(); }

    void request(uint8_t* frameData, uint16_t frameLen);

    bool requestTurnOn();
    bool requestTurnOff();
    void requestAllData();
    void requestData(uint8_t dataId);

    void response(HAL_StatusTypeDef status);

    BMSStatus getStatus() const { return m_status; }
    const BatteryData& getData() const { return m_data; }

    std::string debugMsg;
    uint32_t errFlags{0};

    constexpr static const int rxDataLen = 280; // 279 bytes needed for 12 cells
    constexpr static const int txDataLen = 21;
protected:
    void updateData(const BatteryData& newData);
protected:
    SinglePinElement m_bmsPwrRequest{bms_on_GPIO_Port, bms_on_Pin};
    const SinglePinElement m_bmsPwrStatus{bms_ok_GPIO_Port, bms_ok_Pin};
    BMSStatus m_status { BMSStatus::NoStatus };
    BatteryData m_data;

    CTimeout m_requestTimeout{1000};
    CTimeout m_responseTimeout{500};
    CTimeout m_dataInvalidationTimeout{10000};
    Timeout m_bmsOnResetTimeout{0};

    uint8_t rxData[rxDataLen];
    uint8_t txDataBuffer[txDataLen];
};

enum class BMSUpdaterEvent : uint8_t
{
    NoEvent,
    Updated,
	DataUpdated
};

class BMSUpdater : public BMSHandler
{
public:
    void update();
    BMSUpdaterEvent takeLastEvent();

    void setActive(bool active);
private:
    bool m_isActive{true};
    BMSStatus m_prevStatus { BMSStatus::NoStatus };
    BMSUpdaterEvent m_lastEvent { BMSUpdaterEvent::NoEvent };

    StaticTimeout<10000> m_bmsTurnOnTimeout;
    StaticTimeout<10000> m_bmsTurnOffTimeout;
};


#endif /* BMS_BMS_HANDLER_H_ */
