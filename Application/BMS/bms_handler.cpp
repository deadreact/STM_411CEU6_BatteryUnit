/*
 * bms_handler.cpp
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#include "bms_handler.h"
#include <cstring>

#define VERIFY_INC(iter, value) if (*iter++ != value) return (BMSErrorFlags::ParseValidation | ((uint32_t)value << 16))

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

    template <typename T, typename RT = T>
    RT sum(const T* begin, const T* end)
    {
    	RT value{};
    	while (begin != end) {
    		value += *begin++;
    	}
    	return value;
    }

    uint8_t getbyte(uint8_t* &ptr) { return *ptr++; }
    uint16_t get2bytes(uint8_t* &ptr) { auto res = (uint16_t)ptr[0] << 8 | ptr[1]; ptr += 2; return res; }
    uint32_t get4bytes(uint8_t* &ptr) { auto res = (uint32_t)ptr[0] << 24 | (uint32_t) ptr[1] << 16 | (uint32_t) ptr[2] << 8 | ptr[3]; ptr += 4; return res; }

    inline uint16_t read2bytes(uint8_t* ptr) { return (uint16_t)ptr[0] << 8 | ptr[1]; }
    inline uint32_t read4bytes(uint8_t* ptr) { return (uint32_t)read2bytes(ptr) << 16 | read2bytes(ptr + 2); }

    uint32_t checkValidity(uint8_t rxData[])
    {
    	constexpr static const uint16_t startMark = 0x4e57;
    	constexpr static const uint16_t finishMark = 0x68;

    	if (startMark != read2bytes(rxData)) {
    		return BMSErrorFlags::ParseControlBytes;
    	}
    	
    	const uint16_t fullLength = read2bytes(rxData + 2) + 2;

    	if (fullLength <= BMSHandler::rxDataLen)
    	{
    		if (finishMark != rxData[fullLength-5]) {
    			return BMSErrorFlags::ParseControlBytes;
    		}
    		uint16_t checksum = sum<uint8_t, uint16_t>(rxData, rxData + fullLength - 4);
    		if (checksum != read2bytes(rxData + fullLength - 2)) {
    			return BMSErrorFlags::ParseChecksum;
    		}
    	}
    	else if (fullLength - 4 <= BMSHandler::rxDataLen)
    	{
    		if (finishMark != rxData[fullLength-5]) {
				return BMSErrorFlags::ParseControlBytes;
			}
    		// WARN: Can't check checksum
    	}
    	else
    	{
    		// WARN: Can't check finishMark and checksum
    	}

    	return 0;
    }

    uint32_t parseData(uint8_t rxData[], BatteryData& data)
    {
    	//    TODO validate by first 2 bits and crc!!!!!!!!!
    	if (uint32_t errCode = checkValidity(rxData)) {
    		return errCode;
    	}

    	uint8_t* it = rxData + 2;
    	const uint16_t length = read2bytes(it);
    	it += 2;
    	const uint32_t terminalNumber = read4bytes(it);
    	it += 4;
    	const uint8_t commandWord = *it; //0x01 Activation directive; 0x06 Read all data
    	it++;
    	const uint8_t frameSource = *it; //0. Data box, 1. Bluetooth, 2. GPS, 3, PC PC PC
		it++;
		const uint8_t transportType = *it; //0.Read data, 1.Answer frame 2.Data box active upload
		it += 1;

		VERIFY_INC(it, 0x79);
        data.cellCount = getbyte(it) / 3;
//        it += 2;

        for (uint8_t i = 0; i < data.cellCount; i++)
        {
        	VERIFY_INC(it, (i+1));
            data.cellVoltage[i] = get2bytes(it); // 0.001
//            it += 3;
        }

        VERIFY_INC(it, 0x80);
        const uint16_t reader_tube_temperature = get2bytes(it);// it += 3;
        VERIFY_INC(it, 0x81);
        const uint16_t battery_box_temperature = get2bytes(it);// it += 3;
        VERIFY_INC(it, 0x82);
        const uint16_t battery_temperature = get2bytes(it);// it += 3;
        // it += 9;
        VERIFY_INC(it, 0x83);
        data.voltage = get2bytes(it);

        if (sum(data.cellVoltage, data.cellVoltage + data.cellCount)/10 != data.voltage) {
//        	return BMSErrorFlags::ParseValidation;
        }

        // it += 3;
        VERIFY_INC(it, 0x84);
        data.current = getCurrent(get2bytes(it));
        // it += 3;
        VERIFY_INC(it, 0x85);
        data.soc = getbyte(it);
        // it += 2;
        VERIFY_INC(it, 0x86);
        const uint8_t numOfNTC = getbyte(it);// it += 2;
        VERIFY_INC(it, 0x87);
        const uint16_t numOfBatteryCycles = get2bytes(it);// it += 3;
        VERIFY_INC(it, 0x89);
        const uint32_t totalBatteryCycleCapacity = get4bytes(it);// it += 5;

        VERIFY_INC(it, 0x8A);
        const uint16_t numOfBatteryStrings = get2bytes(it);// it += 3;
        /*
        Bit 0: low capacity alarm 1. Alarm 0 is normal. 
        Only warning 
        Bit 1: MOS tube overtemperature alarm 1, 
        alarm 0, normal, alarm 
        Bit 2: charging overvoltage alarm 1, alarm 0, 
        normal, alarm 
        Bit 3: discharge undervoltage alarm 1, alarm 0, 
        normal, alarm 
        Bit 4: battery over temperature alarm 1, alarm 
        0, normal, alarm 
        Bit 5: charging overcurrent alarm 1, alarm 0, 
        normal, alarm 
        Bit 6: discharge overcurrent alarm 1, alarm 0, 
        normal, alarm 
        Bit 7: cell differential pressure alarm 1, alarm 0, 
        normal, alarm 
        Bit 8: overtemperature alarm in battery box 1, 
        alarm 0, normal, alarm 
        Bit 9: battery low temperature alarm 1, alarm 
        0, normal, alarm 
        Bit 10: monomer overvoltage alarm 1, alarm 0, 
        normal, alarm 
        Bit 11: monomer undervoltage alarm 1, alarm 
        0, normal, alarm 
        Bit 12: 309_ A protection 1 alarm 0 normal, 
        alarm 
        Bit 13: 309_ B protection 1 alarm 0 normal, 
        alarm 
        14 bits: reserved 
        15 bits: reserved 
        Example: 0x0001: indicates low capacity alarm 
        value 
        0x0001 ---- > low capacity alarm 
        0x0002 ---- > over temperature alarm of power 
        board

        */
        VERIFY_INC(it, 0x8B);
        const uint16_t batteryWarnMsg = get2bytes(it); //it += 3;
        /*
        0-bit:charging MOS state 1 on 0 off this is for 
        uploading prompt 
        1-bit:discharge MOS tube status 1 on 0 off. This 
        is for uploading prompt. 
        2-bit：The status of equalizing switch is 1 on 
        and 0 off. This is for uploading prompt 
        3-bit：The battery is disconnected. 1 is normal. 
        0 is disconnected. 
        This is an upload prompt, 
        Bits 4-15: reserved example: 00 01: indicates 
        that the charging MOS tube is on 
        */
        VERIFY_INC(it, 0x8C);
        const uint16_t batteryStatus = get2bytes(it);// it += 3;
        
        VERIFY_INC(it, 0x8E);
//        for (;*it != 0x9a; it += 3);

        auto capacityAhIt = rxData + (data.cellCount * 3 + 131);
        VERIFY_INC(capacityAhIt, 0xAA);
        data.capacityAh = get4bytes(capacityAhIt);

        capacityAhIt = rxData + (data.cellCount * 3 + 202);
		VERIFY_INC(capacityAhIt, 0xB9);
		data.capacityAh = get4bytes(capacityAhIt);


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
			debugMsg = errCode & BMSErrorFlags::ParseControlBytes
					? "parse control bytes"
					: errCode & BMSErrorFlags::ParseChecksum ? "parse error checksum" : "parse error validation: ";
			errFlags |= errCode;

			if (errCode & BMSErrorFlags::maskErrorDetails)
			{
				debugMsg += std::to_string(errCode >> 16);
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

