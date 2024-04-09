/*
 * bms_handler.cpp
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#include "bms_handler.h"
#include <cstring>

//---------------------------------------------------------------------------
namespace
{
    int8_t getTemperature(const int16_t value)
    {
       if (value > 100)
         return (100 - (int16_t) value);

       return value;
    }

    int16_t getCurrent(const uint16_t value)
    {
        return (value & 0x7FFF) * ((value & 0x8000) == 0x8000 ? 1 : -1);
    }

    uint16_t read2bytes(uint8_t* ptr) { return (uint16_t)(*ptr << 8) | *(ptr+1); }

    bool parseData(uint8_t rawData[], BatteryData& data)
    {
    	//    TODO validate by first 2 bits and crc!!!!!!!!!
    	uint8_t* it = rawData + 12;

        data.cellCount = *it / 3;

        it += 2;
        for (uint8_t i = 0; i < data.cellCount; i++)
        {
//            uint8_t cell_number = it[3*i + 1];
            data.cellVoltage[i] = read2bytes(it); // 0.001
            it += 3;
        }

        int8_t power_tube_temperature = getTemperature(read2bytes(it));
        it += 3; // 53
        //54 55
        int8_t sensor_temperature_1 = getTemperature(read2bytes(it));
        it += 3; // 56
        //57 58
        int8_t sensor_temperature_2 = getTemperature(read2bytes(it));

        it += 3; // 59

        // 0x83 0x14 0xEF: Total battery voltage                       5359 * 0.01 = 53.59V      0.01 V
        // 60 61
        data.voltage = read2bytes(it); // 0.01f
        it += 3; // 62

        // 0x84 0x80 0xD0: Current data                                32976                     0.01 A
        //63 64
        uint16_t battery_current = getCurrent(read2bytes(it));
        uint8_t current_low_byte = it[0];
        uint8_t current_hi_byte = it[1];
        // test current
        uint16_t pckd_bat_curr = read2bytes(it);
        data.current = (pckd_bat_curr & 0x7FFF) * ((pckd_bat_curr & 0x8000) == 0x8000 ? 1 : -1); // 0.01f

        it += 3; // 65
        // 0x85 0x0F: Battery remaining capacity
        // 66
        uint8_t battery_soc = *it;
        it += 2; // 67
        // 0x86 0x02: Number of battery temperature sensors             2                        1.0  count
        // 68
        uint8_t temperature_sensor_count = *it;
        it += 2; // 69
        // 0x87 0x00 0x04: Number of battery cycles                     4                        1.0  count
        // 70 71
        uint16_t battery_cycles = read2bytes(it);
        it += 3; // 72
        // 0x89 0x00 0x00 0x00 0x00: Total battery cycle capacity
        // 73 74 75 76
        uint32_t battery_cycle_capacity = (uint32_t) it[0] << 24 | (uint32_t) it[1] << 16 | (uint32_t) it[2] << 8 | it[3];
        it += 5; // 77
        // ignore strings number
        it += 3;

    //    battery_current = (uint16_t) current_low_byte | current_hi_byte;
    //    batCurrent = (float)(10000-battery_current) * 0.01f - 100.0f;

        data.capacity = battery_soc;

        //test energy
    //    if(energyAh == -1000000.0f) // initial setup
    //    {
    //        if(battery_cycle_capacity > 0)
    //        {
    //            energyAh = battery_cycle_capacity;
    //        }
    //        energyAh = 0;
    //    }
        // WHY += ???
//        data.energyAh += (data.voltage * 0.01f) * (data.current * 0.01f) * 0.00002777777 * 2.0; // 0.1/60/60

        return true;
    }

    BMSStatus getRequestStatus(HAL_StatusTypeDef status)
    {
    	switch (status)
		{
		case HAL_OK:		return BMSStatus::Requested;
		case HAL_TIMEOUT:	return BMSStatus::RequestTimedOut;
		case HAL_ERROR:		return BMSStatus::Error;
		case HAL_BUSY:		return BMSStatus::Error;
		}
    	return {};
    }

    BMSStatus getResponseStatus(HAL_StatusTypeDef status)
	{
		switch (status)
		{
		case HAL_OK:
			return BMSStatus::Ok;
		case HAL_TIMEOUT:
		case HAL_ERROR:
		case HAL_BUSY:
			return BMSStatus::Error;
		}
		return {};
	}
} // namespace
//----------------------- TEMP!!! Dirty hack -------------------------------
BMSHandler* s_bmsHandler = nullptr;
//---------------------------------------------------------------------------

constexpr static const int txDataLen = 21;
constexpr static const int txTransmitTimeout = 1000;

static const uint8_t TxData[txDataLen] = {0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x01, 0x29};

extern UART_HandleTypeDef huart2;

BMSHandler::BMSHandler() {
//    s_bmsHandler = this;
}

BMSHandler::~BMSHandler() {
//    if (s_bmsHandler == this) {
//        s_bmsHandler = nullptr;
//    }
}

void BMSHandler::Request()
{
	if (m_status == BMSStatus::Requested) {
		return;
	}
	HAL_StatusTypeDef status{HAL_OK};

	if (m_status == BMSStatus::RequestTimedOut) {
		status = HAL_UART_Abort_IT(&huart2);
	}

	s_bmsHandler = this;
	m_lastRequestTick = HAL_GetTick();

	status = HAL_UARTEx_ReceiveToIdle_IT(&huart2, rxData, rxDataLen);
	if (status != HAL_OK)
	{
		//TODO: check status
		status = HAL_UART_Abort_IT(&huart2);
		status = HAL_UARTEx_ReceiveToIdle_IT(&huart2, rxData, rxDataLen);
	}

	//TODO: check status
    status = HAL_UART_Transmit(&huart2, TxData, txDataLen, txTransmitTimeout);
    m_status = getRequestStatus(status);
}

void BMSHandler::Response(HAL_StatusTypeDef status)
{
	s_bmsHandler = nullptr;
	m_lastResponseTick = HAL_GetTick();

	m_status = getResponseStatus(status);

	if (m_status == BMSStatus::Ok)
	{
		BatteryData data;

		if (parseData(rxData, data)) {
			UpdateData(static_cast<BatteryData&&>(data));
		} else {
			m_status = BMSStatus::Error;
		}
	}

	memset(rxData, 0, sizeof(rxData));
}

void BMSHandler::UpdateData(BatteryData&& newData)
{
    if (m_data != newData)
    {
        // check internal conditions
        m_data = static_cast<BatteryData&&>(newData); // std::move
        // notify others
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
	HAL_UART_RxEventTypeTypeDef event = HAL_UARTEx_GetRxEventType(huart);
//	if (event == HAL_UART_RXEVENT_IDLE)
//	{
//	    HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_IT(&huart2, rxData, rxDataLen);
//
//	}
	static const char* msg = "data received";
	s_bmsHandler->debugMsg = msg;
	s_bmsHandler->Response(HAL_OK);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{

    // #define HAL_UART_ERROR_PE                0x00000001U   /*!< Parity error        */
    // #define HAL_UART_ERROR_NE                0x00000002U   /*!< Noise error         */
    // #define HAL_UART_ERROR_FE                0x00000004U   /*!< Frame error         */
    // #define HAL_UART_ERROR_ORE               0x00000008U   /*!< Overrun error       */
    // #define HAL_UART_ERROR_DMA

    static const char* err1 = "callback parity error";
    static const char* err2 = "callback noise error";
    static const char* err4 = "callback frame error";
    static const char* err8 = "callback overrun error";
    static const char* err16 = "callback dma error";

	switch (huart->ErrorCode)
	{
        case HAL_UART_ERROR_PE:  s_bmsHandler->debugMsg = err1; break;
        case HAL_UART_ERROR_NE:  s_bmsHandler->debugMsg = err2; break;
        case HAL_UART_ERROR_FE:  s_bmsHandler->debugMsg = err4; break;
        case HAL_UART_ERROR_ORE: s_bmsHandler->debugMsg = err8; break;
        case HAL_UART_ERROR_DMA: s_bmsHandler->debugMsg = err16; break;
	}
	s_bmsHandler->Response(HAL_ERROR);
}

void BMSUpdater::Update()
{
	if (m_status == BMSStatus::NoStatus)
	{
		Request();
	}
	else if (m_status == BMSStatus::Requested)
	{
		if ((HAL_GetTick() - m_lastRequestTick > m_requestTimeout))
		{
			m_status = BMSStatus::RequestTimedOut;
			static const char* msg = "request timed out";
			debugMsg = msg;
			Request();
		}
	}
	else if (HAL_GetTick() - m_lastResponseTick > m_invalidatePeriodMsec)
	{
		m_status = BMSStatus::InfoTimedOut;
		static const char* msg = "info timed out";
		debugMsg = msg;
		Request();
	}

	m_lastEvent = BMSUpdaterEvent::NoEvent;
	if (m_prevStatus != m_status)
	{
		m_lastEvent = BMSUpdaterEvent::Updated;
		m_prevStatus = m_status;
	}
}

