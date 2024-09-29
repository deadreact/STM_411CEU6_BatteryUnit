#include <gui/containers/SettingEntry.hpp>
#include <texts/TextKeysAndLanguages.hpp>

static const TEXTS titles[] = {T_SETTING_CHARGER_POWER, T_SETTING_SCREEN, T_SETTING_BRIGHTNESS};
static const TEXTS screen_values[] = {T_SETTING_SCREEN_VALUE_0, T_SETTING_SCREEN_VALUE_1, T_SETTING_SCREEN_VALUE_2};

SettingEntry::SettingEntry()
{

}

void SettingEntry::initialize()
{
    SettingEntryBase::initialize();
}


void SettingEntry::setIndex(int index)
{
	TypedTextId textId = titles[index];
	if (title.getTypedText().getId() != textId)
	{
		title.setTypedText(touchgfx::TypedText(textId));
		title.invalidate();
	}
}

void SettingEntry::setValue(int value)
{
	if (title.getTypedText().getId() == T_SETTING_SCREEN)
	{
		setValueTextId(screen_values[value]);
	}
	else
	{
		setValueTextId(T_SETTING_VALUE_WILDCARD);

		Unicode::snprintf(selectedValueBuffer, SELECTEDVALUE_SIZE, "%d", value);
		selectedValue.invalidate();
	}
}

void SettingEntry::setValueTextId(TypedTextId textId)
{
	if (selectedValue.getTypedText().getId() != textId)
	{
		selectedValue.setTypedText(touchgfx::TypedText(textId));
		selectedValue.invalidate();
	}
}

void SettingEntry::setSelected(bool selected)
{
	selection.setVisible(selected);
	selection.invalidate();
}

