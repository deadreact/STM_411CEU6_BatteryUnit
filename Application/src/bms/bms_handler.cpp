/*
 * bms_handler.cpp
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#include <bms/bms_handler.h>

#include <shared_data.h>
#include <cstring>
#include "jk_bms.h"
#include "stm32f4xx_ll_usart.h"
#include <stdlib.h>

//static const uint8_t TxData[BMSHandler::txDataLen] = {0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, /**/0x06, 0x03, 0x00, /**/0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x01, 0x29};

extern UART_HandleTypeDef huart2;
//---------------------------------------------------------------------------
namespace
{
    BMSStatus getRequestStatus(HAL_StatusTypeDef status)
    {
        switch (status)
        {
        case HAL_OK:        return BMSStatus::Requested;
        case HAL_TIMEOUT:    return BMSStatus::RequestTimedOut;
        case HAL_ERROR:        return BMSStatus::Error;
        case HAL_BUSY:        return BMSStatus::Error;
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

void BMSHandler::request(uint8_t* frameData, uint16_t frameLen)
{
    if (m_status == BMSStatus::Requested) {
        return;
    }
    HAL_StatusTypeDef status{HAL_OK};

    if (m_status == BMSStatus::RequestTimedOut) {
        status = HAL_UART_Abort(&huart2);
    }

//    s_bmsHandler = this;
    m_requestTimeout.reset();

    status = HAL_UARTEx_ReceiveToIdle_IT(&huart2, rxData, rxDataLen);

    if (status == HAL_BUSY)
    {
    	status = HAL_UART_Abort(&huart2);
    	status = HAL_UARTEx_ReceiveToIdle_IT(&huart2, rxData, rxDataLen);
    }
    if (status == HAL_ERROR)
    {
        auto errCode = HAL_UART_GetError(&huart2);
        if (errCode == HAL_UART_ERROR_ORE)
        {
            status = HAL_UART_Abort(&huart2);
//            __HAL_UART_CLEAR_IT(&huart2, UART_CLEAR_OREF);
            LL_USART_ClearFlag_ORE(huart2.Instance);
            status = HAL_UARTEx_ReceiveToIdle_IT(&huart2, rxData, rxDataLen);
        }
    }

    if (status == HAL_OK)
    {
        //TODO: check status
//        status = HAL_UART_Transmit_IT(&huart2, TxData, txDataLen);
        status = HAL_UART_Transmit_IT(&huart2, frameData, frameLen);
    }

    m_status = getRequestStatus(status);
}

bool BMSHandler::requestTurnOn()
{
    if (isPowerOn() == isPowerRequested()) {
    	m_bmsPwrRequest.togglePin();
    }
    return isPowerOn();
}

bool BMSHandler::requestTurnOff()
{
	if (isPowerOn()) {
		if (m_bmsOnResetTimeout.isReached())
		{
			if (!isPowerRequested()) {
				m_bmsOnResetTimeout.reset(2500);
			} else {
				m_bmsOnResetTimeout.reset(500);
			}
			m_bmsPwrRequest.togglePin();
		}

	}
	return !isPowerOn();
}

void BMSHandler::requestAllData()
{
    uint16_t len = jk::fillFrame(txDataBuffer);
    request(txDataBuffer, len);
}

void BMSHandler::requestData(uint8_t dataId)
{
    uint16_t len = jk::fillFrame(txDataBuffer, 0x03, &dataId, 1);
    request(txDataBuffer, len);
}

void BMSHandler::response(HAL_StatusTypeDef status)
{
//    s_bmsHandler = nullptr;
    m_responseTimeout.reset();

    m_status = getResponseStatus(status);

    if (m_status == BMSStatus::Ok)
    {
        BatteryData data(m_data);

        if (uint32_t errCode = jk::parseFrame(data, rxData, rxDataLen)) {
            debugMsg = errCode & BMSErrorFlags::ParseControlBytes
                    ? "parse control bytes"
                    : errCode & BMSErrorFlags::ParseChecksum ? "parse error checksum" : "parse error validation: ";
            errFlags |= errCode;

            if (errCode & BMSErrorFlags::maskErrorDetails)
            {
				uint8_t errDetail = errCode >> 16;
				const char buff[] = {'0' + (errDetail >> 4), '0' + (errDetail & 0x7f), '\0'};
				debugMsg += buff;
            }

            m_status = BMSStatus::Error;
        } else {
#if SIMULATE_CHARGING
            static int simCurrent = 90;
            data.current += simCurrent;
#elif SIMULATE_UNCHARGING
            static int simCurrent = 90;
            data.current -= simCurrent;
#endif
            errFlags &= ~BMSErrorFlags::maskErrorDetails;
            updateData(data);
        }
    }

    memset(rxData, 0, sizeof(rxData));
}

void BMSHandler::updateData(const BatteryData& newData)
{
    m_dataInvalidationTimeout.reset();
    errFlags &= ~BMSErrorFlags::maskMajorErrors;
    if (m_data != newData)
    {
        // check internal conditions
        m_data = newData;
        // notify others
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if (s_bmsHandler)
    {
        s_bmsHandler->debugMsg = "data received";
        s_bmsHandler->errFlags &= ~BMSErrorFlags::maskMinorErrors;
        s_bmsHandler->response(HAL_OK);
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
        s_bmsHandler->response(HAL_ERROR);
    }
}


void BMSUpdater::update()
{
	if (m_isActive)
	{
		if (requestTurnOn())
		{
			m_bmsTurnOnTimeout.reset();

			if (m_status == BMSStatus::NoStatus)
			{
				requestAllData();
			}
			else if (m_status == BMSStatus::Requested)
			{
				if (m_requestTimeout.isReached())
				{
					m_status = BMSStatus::RequestTimedOut;
					debugMsg = "request timed out";
				}
			}
			else
			{
				if (m_dataInvalidationTimeout.isReached())
				{
					errFlags |= BMSErrorFlags::ValidResponseTimeout;
					debugMsg = "valid response timed out";
					requestAllData();
				}
				else if (m_status == BMSStatus::RequestTimedOut)
				{
					errFlags |= BMSErrorFlags::RequestTimeout;
					requestAllData();
				}
				else if (m_responseTimeout.isReached())
				{
					m_status = BMSStatus::InfoTimedOut;
					debugMsg = "info timed out";
					requestAllData();
				}
			}

			if (m_prevStatus != m_status)
			{
				if (m_prevStatus == BMSStatus::Requested && m_status == BMSStatus::Ok) {
					m_lastEvent = BMSUpdaterEvent::DataUpdated;
				} else {
					m_lastEvent = BMSUpdaterEvent::Updated;
				}

				m_prevStatus = m_status;
			}
		}
		else if (m_bmsTurnOnTimeout.isReached())
		{
			errFlags |= BMSErrorFlags::TurnOnTimeout;
		}
	}
	else
	{
		if (requestTurnOff())
		{
			m_bmsTurnOffTimeout.reset();
			HAL_UART_Abort(&huart2);
			m_status = BMSStatus::NoStatus;
		}
		else if (m_bmsTurnOffTimeout.isReached())
		{
			errFlags |= BMSErrorFlags::TurnOffTimeout;
		}
	}
}

BMSUpdaterEvent BMSUpdater::takeLastEvent() {
	auto tmp = m_lastEvent;
	m_lastEvent = BMSUpdaterEvent::NoEvent;
	return tmp;
}

void BMSUpdater::setActive(bool active)
{
	if (m_isActive != active)
	{
		m_isActive = active;
		errFlags = 0;
		if (active) {
			m_bmsTurnOnTimeout.reset();
		} else {
			m_bmsTurnOffTimeout.reset();
		}
	}
}


