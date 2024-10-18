#include <gui/containers/Settings.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <utils/stack_vector.h>

static const TypedTextId titles[] = {T_SETTING_CHARGER_POWER, T_SETTING_SCREEN, T_SETTING_BRIGHTNESS};

static const utils::stack_vector<TypedTextId, 5> values[] = {
	{T_SETTING_CHARGER_POWER_VALUE_0, T_SETTING_CHARGER_POWER_VALUE_1, T_SETTING_CHARGER_POWER_VALUE_2, T_SETTING_CHARGER_POWER_VALUE_3, T_SETTING_CHARGER_POWER_VALUE_4},
    {T_SETTING_SCREEN_VALUE_1, T_SETTING_SCREEN_VALUE_0},
    {T_SETTING_BRIGHTNESS_VALUE_0, T_SETTING_BRIGHTNESS_VALUE_1, T_SETTING_BRIGHTNESS_VALUE_2, T_SETTING_BRIGHTNESS_VALUE_3}
};

Settings::Settings()
{

}

void Settings::initialize()
{
    SettingsBase::initialize();

    auto size = std::ssize(titles);
	entries.setNumberOfItems(size);

	for (int i = 0; i < size; ++i)
	{
		entriesListItems[i].setTitle(titles[i]);
		entriesListItems[i].setValues(values[i]);
	}
}


void Settings::setData(const SettingsData& data)
{
	for (int i = 0; i < m_selectedSettings.size(); ++i)
	{
		if (m_selectedSettings[i] != data.settings[i].selectedIndex)
		{
			entriesListItems[i].selectValue(data.settings[i].selectedIndex);
			m_selectedSettings[i] = data.settings[i].selectedIndex;
		}
	}

	if (m_selectedEntry != data.selectedIndex)
	{
		if (m_selectedEntry >= 0)
			entriesListItems[m_selectedEntry].setSelected(false);
		if (data.selectedIndex >= 0)
			entriesListItems[data.selectedIndex].setSelected(true);

		m_selectedEntry = data.selectedIndex;
		invalidate();
	}
}
