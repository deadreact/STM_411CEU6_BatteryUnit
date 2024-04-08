/*
 * bms_handler.cpp
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#include "bms_handler.h"
#include "main.h"
#include <cstdlib>
#include <cstring>

BatteryData::BatteryData(const BatteryData& other)
    : voltage(other.voltage)
    , current(other.current)
    , capacity(other.capacity)
    , energyAh(other.energyAh)
{
    setCellCount(other.cellCount);
    memcpy(cellVoltage, other.cellVoltage, sizeof(uint16_t)*cellCount);
}

BatteryData& BatteryData::operator=(const BatteryData& other)
{
    setCellCount(other.cellCount);
    memcpy(cellVoltage, other.cellVoltage, sizeof(uint16_t)*cellCount);
    voltage = other.voltage;
    current = other.current;
    capacity = other.capacity;
    energyAh = other.energyAh;

    return *this;
}

BatteryData::BatteryData(BatteryData&& other)
    : cellVoltage(other.cellVoltage)
    , cellCount(other.cellCount)
    , voltage(other.voltage)
    , current(other.current)
    , capacity(other.capacity)
    , energyAh(other.energyAh)
{
    other.cellVoltage = nullptr;
    other.cellCount = 0;
}

BatteryData& BatteryData::operator=(BatteryData&& other)
{
    cellVoltage = other.cellVoltage;
    cellCount = other.cellCount;
    voltage = other.voltage;
    current = other.current;
    capacity = other.capacity;
    energyAh = other.energyAh;

    other.cellVoltage = nullptr;
    other.cellCount = 0;

    return *this;
}

BatteryData::~BatteryData()
{
    if (cellVoltage != nullptr)
    {
        free(cellVoltage);
        cellVoltage = nullptr;
        cellCount = 0;
    }
}

void BatteryData::setCellCount(uint16_t count)
{
    if (count != cellCount)
    {
        if (cellVoltage == nullptr)
        {
            if (cellVoltage = (uint16_t*)malloc(sizeof(uint16_t)* count)) {
                count = cellCount;
            }
        }
        else if (uint16_t* ptr = (uint16_t*)realloc(cellVoltage, sizeof(uint16_t)* count))
        {
            cellVoltage = ptr;
            count = cellCount;
        }
    }
}

bool BatteryData::operator==(const BatteryData& other) const
{
    if (cellCount == other.cellCount) {
        for (uint16_t cell = 0; cell < cellCount; ++cell) {
            if (cellVoltage[cell] != other.cellVoltage[cell]) {
                return false;
            }
        }
    }
    return voltage == other.voltage && current == other.current && capacity == other.capacity && energyAh == other.energyAh;
}

//---------------------------------------------------------------------------
namespace
{
    int8_t getTemperature(const int16_t value)
    {
       if (value > 100)
         return (100 - (int16_t) value);

       return value;
    }

    uint16_t getCurrent(const uint16_t value)
    {
        if ((value & 0x8000) == 0x8000)
        {
            return (value & 0x7FFF);
        } else
        {
            return (value & 0x7FFF) * -1;
        }
    }
} // namespace
//----------------------- TEMP!!! Dirty hack -------------------------------
BMSHandler* s_bmsHandler = nullptr;
//---------------------------------------------------------------------------

constexpr static const int txDataLen = 21;
constexpr static const int rxDataLen = 256;
constexpr static const int txTransmitTimeout = 1000;

static const uint8_t TxData[txDataLen] = {0x4E, 0x57, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x01, 0x29};

extern UART_HandleTypeDef huart2;

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
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, TxData, txDataLen, txTransmitTimeout);

    if (status == HAL_OK)
    {
        m_status = BMSStatus::Requested;
    }
}

void BMSHandler::ParseData(uint8_t *rawData)
{
    BatteryData data;

    data.setCellCount(rawData[1] / 3);

    for (uint8_t i = 0; i < data.cellCount; i++)
    {
        uint8_t cell_number = rawData[3*i + 2];
        data.cellVoltage[i] = (uint16_t) rawData[3*i + 3] << 8 | rawData[3*i + 4]; // 0.001
    }
//    cells++;
    uint16_t pos = (data.cellCount) * 3 + 2; // 50

    int8_t power_tube_temperature = getTemperature((uint16_t) rawData[pos + 1] << 8 | rawData[pos + 2]);
    pos += 3; // 53
    //54 55
    int8_t sensor_temperature_1 = getTemperature((uint16_t) rawData[pos + 1] << 8 | rawData[pos + 2]);
    pos += 3; // 56
    //57 58
    int8_t sensor_temperature_2 = getTemperature((uint16_t) rawData[pos + 1] << 8 | rawData[pos + 2]);

    pos += 3; // 59

    // 0x83 0x14 0xEF: Total battery voltage                       5359 * 0.01 = 53.59V      0.01 V
    // 60 61
    data.voltage = (uint16_t) rawData[pos + 1] << 8 | rawData[pos + 2]; // 0.01f
    pos += 3; // 62

    // 0x84 0x80 0xD0: Current data                                32976                     0.01 A
    //63 64
    uint16_t battery_current = getCurrent((uint16_t) rawData[pos + 1] << 8 | rawData[pos + 2]);
    uint8_t current_low_byte = rawData[pos + 1];
    uint8_t current_hi_byte = rawData[pos + 2];
    // test current
    uint16_t pckd_bat_curr = (uint16_t) rawData[pos + 1] << 8 | rawData[pos + 2];
    data.current = (pckd_bat_curr & 0x7FFF) * ((pckd_bat_curr & 0x8000) == 0x8000 ? 1 : -1); // 0.01f

    pos += 3; // 65
    // 0x85 0x0F: Battery remaining capacity
    // 66
    uint8_t battery_soc = (uint8_t) rawData[pos + 1];
    pos += 2; // 67
    // 0x86 0x02: Number of battery temperature sensors             2                        1.0  count
    // 68
    uint8_t temperature_sensor_count = (uint8_t) rawData[pos + 1];
    pos += 2; // 69
    // 0x87 0x00 0x04: Number of battery cycles                     4                        1.0  count
    // 70 71
    uint16_t battery_cycles = (uint16_t) rawData[pos + 1] << 8 | rawData[pos + 2];
    pos += 3; // 72
    // 0x89 0x00 0x00 0x00 0x00: Total battery cycle capacity
    // 73 74 75 76
    uint32_t battery_cycle_capacity = (uint32_t) rawData[pos + 1] << 24 | (uint32_t) rawData[pos + 2] << 16 | (uint32_t) rawData[pos + 3] << 8 | rawData[pos + 4];
    pos += 5; // 77
    // ignore strings number
    pos += 3;

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
    data.energyAh = (data.voltage * 0.01f) * (data.current * 0.01f) * 0.00002777777 * 2.0; // 0.1/60/60

    // if Ok ??
    m_status = BMSStatus::Ok;
    UpdateData(static_cast<BatteryData&&>(data)); // std::move
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

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    uint8_t RxData[rxDataLen] = {};

    HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_IT(&huart2, RxData, rxDataLen);
    if (status == HAL_OK)
    {
    //    TODO validate by first 2 bits and crc!!!!!!!!!

        s_bmsHandler->ParseData(&RxData[11]);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{

}



