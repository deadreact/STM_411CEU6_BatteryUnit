/*
 * settings_data.h
 *
 *  Created on: Oct 20, 2024
 *      Author: deadreact
 */

#ifndef INC_SETTINGS_DATA_H_
#define INC_SETTINGS_DATA_H_

#include <common_defs.h>
#include <array>
#include <cinttypes>

struct SettingsEntryData
{
    using vector = std::array<int, 5>;

    const char* name{""};
    vector values;
    vector::size_type selectedIndex{0};

    SettingsEntryData() = default;
    SettingsEntryData(const char* name, const vector& values, vector::size_type defaultIndex = 0)
        : name(name)
        , values(values)
        , selectedIndex(defaultIndex)
    {
//        assert(!values.empty());
//        assert(defaultIndex < values.size());
    }

    int getValue() const
    {
        return values.at(selectedIndex);
    }

    bool operator==(const SettingsEntryData& other) const {
    	return name == other.name && values == other.values && selectedIndex == other.selectedIndex;
    }
    bool operator!=(const SettingsEntryData& other) const {
    	return !operator==(other);
    }
};

struct SettingsData
{
	enum SettingIndex : uint8_t
	{
		ChargePower = 0,
		ScreenType,
		Brightness,

		Count
	};

    using vector = std::array<SettingsEntryData, 3>;

    SettingsData()
    {
    	settings[0] = {"Charge power", {1200, 1000, 750, 500, 250}};
		settings[1] = {"Screen type", {1, 0}};
		settings[2] = {"Brightness", {100, 70, 30, 10}};
		selectedIndex = 0;
    }

    SettingsData(void*)
    {}

    bool active{false};
    vector settings;
    int selectedIndex{0};

    int getValue(SettingIndex index) const
    {
        return settings.at(index).getValue();
    }

    bool operator==(const SettingsData& other) const {
		return active == other.active && settings == other.settings && selectedIndex == other.selectedIndex;
	}
	bool operator!=(const SettingsData& other) const {
		return !operator==(other);
	}
};


#endif /* INC_SETTINGS_DATA_H_ */
