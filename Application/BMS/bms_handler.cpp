/*
 * bms_handler.cpp
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#include "bms_handler.h"
#include <cstring>

constexpr static const int txDataLen = 21;
constexpr static const int txTransmitTimeout = 1000;

static const uint8_t TxData[txDataLen] = {0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x01, 0x29};

extern UART_HandleTypeDef huart2;
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

    template <typename T>
    T sum(const T* begin, const T* end)
    {
    	T value{};
    	while (begin != end) {
    		value += *begin++;
    	}
    	return value;
    }

    uint16_t read2bytes(uint8_t* ptr) { return (uint16_t)ptr[0] << 8 | ptr[1]; }
    uint32_t read4bytes(uint8_t* ptr) { return (uint32_t)ptr[0] << 24 | (uint32_t) ptr[1] << 16 | (uint32_t) ptr[2] << 8 | ptr[3]; }

    uint32_t parseData(uint8_t rxData[], BatteryData& data)
    {
    	//    TODO validate by first 2 bits and crc!!!!!!!!!
    	if (memcmp(rxData, TxData, 2) != 0) {
    		return BMSErrorFlags::ParseStartBits;
    	}
    	uint8_t* it = rxData + 12;

        data.cellCount = *it / 3;

        it += 2;
        for (uint8_t i = 0; i < data.cellCount; i++)
        {
            data.cellVoltage[i] = read2bytes(it); // 0.001
            it += 3;
        }
        it += 9;
        data.voltage = read2bytes(it);

        if (sum(data.cellVoltage, data.cellVoltage + data.cellCount)/10 != data.voltage) {
        	return BMSErrorFlags::ParseValidation;
        }

        it += 3;
        data.current = getCurrent(read2bytes(it));
        it += 3;
        data.capacity = *it;

        return 0;
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

BMSHandler::BMSHandler() {
    s_bmsHandler = this;
}

BMSHandler::~BMSHandler() {
    if (s_bmsHandler == this) {
        s_bmsHandler = nullptr;
    }
}

void BMSHandler::Request()
{
	if (m_status == BMSStatus::Requested) {
		return;
	}
	HAL_StatusTypeDef status{HAL_OK};

	if (m_status == BMSStatus::RequestTimedOut) {
		errFlags |= BMSErrorFlags::RequestTimeout;
		status = HAL_UART_Abort(&huart2);
	}

//	s_bmsHandler = this;
	m_lastRequestTick = HAL_GetTick();

	status = HAL_UARTEx_ReceiveToIdle_IT(&huart2, rxData, rxDataLen);

	if (status == HAL_ERROR)
	{
		auto errCode = HAL_UART_GetError(&huart2);
		if (errCode == HAL_UART_ERROR_ORE)
		{
			status = HAL_UART_Abort(&huart2);
			status = HAL_UARTEx_ReceiveToIdle_IT(&huart2, rxData, rxDataLen);
		}
	}

	if (status == HAL_OK)
	{
		//TODO: check status
		status = HAL_UART_Transmit_IT(&huart2, TxData, txDataLen);
	}

    m_status = getRequestStatus(status);
}

void BMSHandler::Response(HAL_StatusTypeDef status)
{
//	s_bmsHandler = nullptr;
	m_lastResponseTick = HAL_GetTick();

	m_status = getResponseStatus(status);

	if (m_status == BMSStatus::Ok)
	{
		BatteryData data;

		if (uint32_t errCode = parseData(rxData, data)) {
			debugMsg = errCode == BMSErrorFlags::ParseStartBits ? "parse error start bits" : "parse error validation";
			errFlags |= errCode;
			m_status = BMSStatus::Error;
		} else {
			UpdateData(data);
		}
	}

	memset(rxData, 0, sizeof(rxData));
}

void BMSHandler::UpdateData(const BatteryData& newData)
{
	m_lastDataUpdateTick = HAL_GetTick();
	errFlags &= ~BMSErrorFlags::maskMajorErrors;
    if (m_data != newData)
    {
        // check internal conditions
        m_data = newData; // std::move
        // notify others
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
	if (s_bmsHandler)
	{
		s_bmsHandler->debugMsg = "data received";
		s_bmsHandler->errFlags &= ~BMSErrorFlags::maskMinorErrors;
		s_bmsHandler->Response(HAL_OK);
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (s_bmsHandler)
	{
	    s_bmsHandler->debugMsg = "clbk";
	    if (huart->ErrorCode & HAL_UART_ERROR_PE) { s_bmsHandler->debugMsg += " parity"; }
	    if (huart->ErrorCode & HAL_UART_ERROR_NE) { s_bmsHandler->debugMsg += " noise"; }
	    if (huart->ErrorCode & HAL_UART_ERROR_FE) { s_bmsHandler->debugMsg += " frame"; }
	    if (huart->ErrorCode & HAL_UART_ERROR_ORE) { s_bmsHandler->debugMsg += " overrun"; }
	    if (huart->ErrorCode & HAL_UART_ERROR_DMA) { s_bmsHandler->debugMsg += " dma"; }
	    s_bmsHandler->debugMsg += " err";
	    s_bmsHandler->errFlags |= huart->ErrorCode;
		s_bmsHandler->Response(HAL_ERROR);
	}
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
			debugMsg = "request timed out";
			Request();
		}
	}
	else
	{
		if (HAL_GetTick() - m_lastDataUpdateTick > m_validResponseTimeout)
		{
			errFlags |= BMSErrorFlags::ValidResponseTimeout;
			debugMsg = "valid response timed out";
		}
		if (HAL_GetTick() - m_lastResponseTick > m_invalidatePeriodMsec)
		{
			m_status = BMSStatus::InfoTimedOut;
			debugMsg = "info timed out";
			Request();
		}
	}

	m_lastEvent = BMSUpdaterEvent::NoEvent;
	if (m_prevStatus != m_status)
	{
		m_lastEvent = BMSUpdaterEvent::Updated;
		m_prevStatus = m_status;
	}
}

