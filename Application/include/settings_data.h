/*
 * settings_data.h
 *
 *  Created on: Sep 29, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef INCLUDE_SETTINGS_DATA_H_
#define INCLUDE_SETTINGS_DATA_H_

#include <utils/stack_string.h>


struct SettingsEntryData
{
    using vector = utils::stack_vector<int, 10>;

    utils::stack_string name;
    vector values;
    vector::size_type selectedIndex{0};

    SettingsEntryData() = default;
    SettingsEntryData(const char* name, const vector& values, vector::size_type defaultIndex = 0)
        : name(name)
        , values(values)
        , selectedIndex(defaultIndex)
    {
        assert(!values.empty());
        assert(defaultIndex < values.size());
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
    using vector = utils::stack_vector<SettingsEntryData, 3>;

    SettingsData()
    {
    	settings.push_back({"Charge power", {250, 500, 750, 1000, 1200}, 4});
		settings.push_back({"Screen type", {0, 1}, 1});
		settings.push_back({"Brightness", {25, 50, 75, 100}, 3});
		selectedIndex = 0;
    }

    SettingsData(nullptr_t)
    {}

    bool active{false};
    vector settings;
    int selectedIndex{-1};

    int getValue(vector::size_type index) const
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


#endif /* INCLUDE_SETTINGS_DATA_H_ */
