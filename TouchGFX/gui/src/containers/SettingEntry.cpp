#include <gui/containers/SettingEntry.hpp>

SettingEntry::SettingEntry()
{

}

void SettingEntry::initialize()
{
    SettingEntryBase::initialize();
}

void SettingEntry::setTitle(TypedTextId textId)
{
	if (title.getTypedText().getId() != textId)
	{
		title.setTypedText(touchgfx::TypedText(textId));
		title.invalidate();
	}
}

void SettingEntry::setValues(const utils::stack_vector<TypedTextId, 5>& values)
{
	m_values = values;

	selectedValue.setTypedText(touchgfx::TypedText(values.front()));
	selectedValue.invalidate();
}

void SettingEntry::selectValue(uint16_t index)
{
	TypedTextId textId = m_values.at(index);
	if (title.getTypedText().getId() != textId)
	{
		selectedValue.setTypedText(touchgfx::TypedText(textId));
		selectedValue.invalidate();
	}
}

void SettingEntry::setSelected(bool selected)
{
	if (selected != selection.isVisible())
	{
		//#5B5B5B
		auto color = selected ? 0xFFFFFFFF : 0xFF5B5B5B;
		title.setColor(color);
		selectedValue.setColor(color);
		selection.setVisible(selected);
		invalidate();
	}
}

