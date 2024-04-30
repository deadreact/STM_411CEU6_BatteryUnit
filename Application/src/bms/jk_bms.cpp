/*
 * jk_bms.cpp
 *
 *  Created on: Apr 15, 2024
 *      Author: deadreact
 */

#include "jk_bms.h"

#define SINGLE_BATTERY_VOLTAGE 0x79
#define READER_TUBE_TEMPERATURE 0x80
#define BATTERY_BOX_TEMPERATURE 0x81
#define BATTERY_TEMPERATURE 0x82
#define TOTAL_BATTERY_VOLTAGE 0x83
#define CURRENT_DATA 0x84
#define SOC 0x85
#define NUMBER_OF_NTS 0x86
#define NUMBER_BATTERY_CYCLES 0x87
#define TOTAL_BATTERY_CYCLE_CAPACITY 0x89
#define TOTAL_NUMBER_OF_BATTERY_STRINGS 0x8a
#define BATTERY_WARNING_MSG 0x8b
#define BATTERY_STATUS_INFO 0x8c
#define TOTAL_VOLTAGE_OVERVOLTAGE_PROTECTION 0x8e
#define TOTAL_VOLTAGE_UNDERVOLTAGE_PROTECTION 0x8f
#define SINGLE_OVERVOLTAGE_PROTECTION 0x90
#define MONOMER_OVERVOLTAGE_RECOVERY_VOLTAGE 0x91
#define SINGLE_OVERVOLTAGE_PROTECTION_DELAY 0x92
#define DIFFERENTIAL_VOLTAGE_PROTECTION_VALUE 0x93
#define DISCHARGE_OVERCURRENT_PROTECTION_VALUE 0x94
#define DISCHARGE_OVERCURRENT_DELAY 0x95
#define CHARGING_OVERCURRENT_PROTECTION_VALUE 0x96
#define CHARGING_OVERCURRENT_DELAY 0x97
#define EQUALIZING_STARTING_VOLTAGE 0x98
#define EQUALIZING_OPENING_DIFFERENTIAL 0x99
#define ACTIVE_EQUALIZATION_SWITCH 0x9a
#define POWER_TUBE_TEMPERATURE_PROTECTION_VALUE 0x9b
#define BATTERY_BOX_TEMPERATURE_PROTECTION_VALUE 0x9f
#define RECOVERY_VALUE_OF_BATTERY_IN_BOX 0xa0
#define BATTERY_TEMPERATURE_DIFFERENCE 0xa1
#define BATTERY_CHARGING_HIGH_TEMPERATURE_PROTECTION_VALUE 0xa2
#define HIGH_TEMPERATURE_PROTECTION_VALUE_FOR_BATTERY_CHARGING 0xa3
#define HIGH_TEMPERATURE_PROTECTION_VALUE_FOR_BATTERY_DISCHARGE 0xa4
#define CHARGING_CRYOPROTECTION_VALUE 0xa5
#define RECOVERY_VALUE_OF_CHARGE_CRYOPROTECTION 0xa6
#define DISCHARGE_CRYOPROTECTION_VALUE 0xa7
#define DISCHARGE_LOW_TEMPERATURE_PROTECTION_RECOVERY_VALUE 0xa8
#define NUMBER_OF_BATTERY_STRINGS_SETTINGS 0xa9
#define BATTERY_CAPACITY_SETTINGS 0xaa
#define CHARGING_MOS_SWITCH 0xab
#define DISCHARGE_MOS_SWITCH 0xac
#define CURRENT_CALIBRATION 0xad
#define PROTECTIVE_BOARD_ADDRESS 0xae
#define BATTERY_TYPE 0xaf
#define SLEEP_WAIT_TIME 0xb0
#define LOW_CAPACITY_ALARM_VALUE 0xb1
#define MODIFY_PARAMETER_PASSWORD 0xb2
#define SPECIAL_CHARGER_SWITCH 0xb3
#define DEVICE_ID_CODE 0xb4
#define DATE_OF_PRODUCTION 0xb5
#define SYSTEM_WORKING_TIME 0xb6
#define SOFTWARE_VERTION_NUMBER 0xb7

#define START_CURRENT_CALIBRATION 0xb8
#define ACTUAL_BATTERY_CAPACITY 0xb9
#define NAMING_OF_FACTORY_ID 0xba
#define RESTART_THE_SYSTEM 0xbb
#define RESTORE_FACTORY_SETTINGS 0xbc
#define REMOTE_UPGRADE_IDENTITY 0xbd
#define CORE_LOW_VOLTAGE_OFF_GPS 0xbe
#define CORE_LOW_VOLTAGE_RECOVERY_GPS 0xbf
// #define PROTOCOL_VERSION

#define VERIFY(frame, value) if (!frame.verify(uint8_t(value))) return (BMSErrorFlags::ParseValidation | ((uint32_t)value << 16))

static const uint16_t stx = 0x4E57;
static const uint8_t endIdentity = 0x68;
static const uint32_t bmsTerminalNum = 0x00000000;
static const uint8_t frameSourcePC = 0x03;
static const uint32_t recordNumber = 0x00000000;
static const uint16_t crc = 0x0000;

namespace
{
    class JKFrame
    {
    public:
        JKFrame(uint8_t* ptr): begin(ptr), ptr(ptr) {}

        inline JKFrame& operator<<(uint8_t data) { *ptr++ = data; return *this; }
        inline JKFrame& operator<<(uint16_t data) { ptr[0] = data >> 8; ptr[1] = data; ptr += 2; return *this; }
        inline JKFrame& operator<<(uint32_t data) { ptr[0] = data >> 24; ptr[1] = data >> 16; ptr[2] = data >> 8; ptr[3] = data; ptr += 4; return *this; }

        inline JKFrame& operator>>(uint8_t& data) { data = *ptr++; return *this; }
        inline JKFrame& operator>>(uint16_t& data) { data = (uint16_t)ptr[0] << 8 | ptr[1]; ptr += 2; return *this; }
        inline JKFrame& operator>>(uint32_t& data) { data = (uint32_t)ptr[0] << 24 | (uint32_t) ptr[1] << 16 | (uint32_t) ptr[2] << 8 | ptr[3]; ptr += 4; return *this; }
        inline JKFrame& operator>>(int16_t& data) { data = ((int16_t)(ptr[0] & 0x7F) << 8 | ptr[1]) * (ptr[0] & 0x80 ? 1 : -1); ptr += 2; return *this; }

        inline JKFrame& read_temperature(int8_t& data) { data = ptr[1] <= 100 ? ptr[1] : -int8_t(ptr[1] - 100); ptr += 2; return *this;}
        inline int8_t read_temperature() { int8_t t; read_temperature(t); return t; }

        inline uint8_t readbyte() { return *ptr++; }
        inline uint16_t read2bytes() { uint16_t data; *this >> data; return data; }
        inline uint32_t read4bytes() { uint32_t data; *this >> data; return data; }

        inline bool verify(uint8_t data) { return *ptr++ == data; }
        inline bool verify(uint16_t data) { uint16_t data1; *this >> data1; return data == data1; }
        inline bool verify(uint32_t data) { uint32_t data1; *this >> data1; return data == data1; }

        inline JKFrame& moveto(uint16_t index) { ptr = begin + index; return *this; }

        inline uint16_t checksum() const
        {
            uint16_t value{};
            auto rbegin = ptr;
            auto rend = begin - 1;
            while (rbegin != rend) {
                value += *rbegin--;
            }
            return value;
        }

        inline uint16_t len() const { return ptr - begin; }
    private:
        uint8_t* const begin;
        uint8_t* ptr;
    };

    // ----------------------------------------------------------------------

    uint32_t checkValidity(uint8_t rxData[], int16_t rxDataLen)
    {
        JKFrame frame(rxData);

        if (!frame.verify(stx)) {
            return BMSErrorFlags::ParseControlBytes;
        }

        const uint16_t fullLength = frame.read2bytes() + 2;

        if (fullLength <= rxDataLen)
        {
            frame.moveto(fullLength-5);
            if (!frame.verify(endIdentity)) {
                return BMSErrorFlags::ParseControlBytes;
            }
            uint32_t crcChecksum = (uint32_t)crc << 16 | frame.checksum();
            if (!frame.verify(crcChecksum)) {
                return BMSErrorFlags::ParseChecksum;
            }
        }
        else if (fullLength - 4 <= rxDataLen)
        {
            frame.moveto(fullLength-5);
            if (!frame.verify(endIdentity)) {
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

    uint32_t parseProperty(JKFrame& frame, BatteryData& data)
    {
        switch (frame.readbyte())
        {
            case SINGLE_BATTERY_VOLTAGE:
            {
                data.cellVoltage.resize(frame.readbyte() / 3);

                for (uint8_t i = 0; i < data.cellVoltage.size(); i++)
                {
                    VERIFY(frame, (i+1));
                    frame >> data.cellVoltage[i];
                }
            } break;
            case TOTAL_BATTERY_VOLTAGE:
                frame >> data.voltage;
                break;
            case CURRENT_DATA:
                frame >> data.voltage;
                break;
            case SOC:
                frame >> data.voltage;
                break;
            default:
                return BMSErrorFlags::ParseUnsupportedType;
        }

        return 0;
    }

    uint32_t parseAllProperties(JKFrame& frame, BatteryData& data)
    {
        VERIFY(frame, SINGLE_BATTERY_VOLTAGE);
        data.cellVoltage.resize(frame.readbyte() / 3);

        for (uint8_t i = 0; i < data.cellVoltage.size(); i++)
        {
            VERIFY(frame, (i+1));
            frame >> data.cellVoltage[i];
        }

        VERIFY(frame, READER_TUBE_TEMPERATURE);
        const int8_t reader_tube_temperature = frame.read_temperature();
        VERIFY(frame, BATTERY_BOX_TEMPERATURE);
        data.battery_box_temperature = frame.read_temperature();
        VERIFY(frame, BATTERY_TEMPERATURE);
        data.battery_temperature = frame.read_temperature();

        VERIFY(frame, TOTAL_BATTERY_VOLTAGE);
        frame >> data.voltage;

        VERIFY(frame, CURRENT_DATA);
        frame >> data.current;

        VERIFY(frame, SOC);
        frame >> data.soc;

        VERIFY(frame, NUMBER_OF_NTS);
        const uint8_t numOfNTC = frame.readbyte();
        VERIFY(frame, NUMBER_BATTERY_CYCLES);
        const uint16_t numOfBatteryCycles = frame.read2bytes();
        VERIFY(frame, TOTAL_BATTERY_CYCLE_CAPACITY);
        const uint32_t totalBatteryCycleCapacity = frame.read4bytes();

        VERIFY(frame, TOTAL_NUMBER_OF_BATTERY_STRINGS);
        const uint16_t numOfBatteryStrings = frame.read2bytes();
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
        VERIFY(frame, BATTERY_WARNING_MSG);
        frame >> data.warningMsg;
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
        VERIFY(frame, BATTERY_STATUS_INFO);
        frame >> data.status;

        VERIFY(frame, TOTAL_VOLTAGE_OVERVOLTAGE_PROTECTION);

        frame.moveto(data.cellVoltage.size() * 3 + 129);
        VERIFY(frame, BATTERY_CAPACITY_SETTINGS);
        frame >> data.capacityAh;

        // frame.moveto(data.cellVoltage.size() * 3 + 200);
        // VERIFY(frame, ACTUAL_BATTERY_CAPACITY);
        // frame >> data.capacityAh;

        return 0;
    }

}

namespace jk
{

uint16_t fillFrame(uint8_t txData[], uint8_t command, const uint8_t* data, uint16_t dataLen)
{
    static const uint8_t transportType = 0x00;

    uint16_t len = uint16_t(dataLen + 0x12);

    JKFrame frame(txData);
    frame << stx << len << bmsTerminalNum << command << frameSourcePC << transportType;

    if (data) {
        for (; dataLen > 0; dataLen-- ) {
            frame << *data;
        }
    } else {
        frame << uint8_t(0x00);
    }
    frame << recordNumber << endIdentity;

    const uint16_t checksum = frame.checksum();
    frame << crc << checksum;

    return frame.len();
}

uint32_t parseFrame(BatteryData& data, uint8_t rxData[], uint16_t rxDataLen)
{
    if (uint32_t errCode = checkValidity(rxData, rxDataLen)) {
        return errCode;
    }

    JKFrame frame(rxData + 2);

    const uint16_t length = frame.read2bytes();
    const uint32_t terminalNumber = frame.read4bytes();
    const uint8_t commandWord = frame.readbyte(); //0x01 Activation directive; 0x06 Read all data
    const uint8_t frameSource = frame.readbyte(); //0. Data box, 1. Bluetooth, 2. GPS, 3, PC PC PC
    const uint8_t transportType = frame.readbyte(); //0.Read data, 1.Answer frame 2.Data box active upload

    switch (commandWord)
    {
    case 0x03:
        return parseProperty(frame, data);
    case 0x06:
        return parseAllProperties(frame, data);
    default:
    	return BMSErrorFlags::ParseUnsupportedType;
    }
}

} //namespace jk
