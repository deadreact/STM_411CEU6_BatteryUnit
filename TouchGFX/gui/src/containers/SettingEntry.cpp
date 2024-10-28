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

	circles.setNumberOfItems(values.size());
	circles.setWidth(circlesListItems[0].getWidth() * values.size());
//	circles.setX(selectedValue.getX() + selectedValue.getWidth()/2 - circles.getWidth()/2);
	circles.setX(249 - (10*values.size())/2);
	circlesListItems[0].setColor(selection.isVisible() ? 0xFF000000 : 0xFFFFF5D9);
	circles.invalidate();

	selectedValue.setTypedText(touchgfx::TypedText(values.front()));
	selectedValue.invalidate();
}

void SettingEntry::selectValue(uint16_t index)
{
	if (index != selectedIndex)
	{
		circlesListItems[selectedIndex].setColor(selection.isVisible() ? 0x66000000 : 0x66FFF5D9);
		selectedIndex = index;
		circlesListItems[selectedIndex].setColor(selection.isVisible() ? 0xFF000000 : 0xFFFFF5D9);
		circles.invalidate();
		TypedTextId textId = m_values.at(index);
		if (title.getTypedText().getId() != textId)
		{
			selectedValue.setTypedText(touchgfx::TypedText(textId));
			selectedValue.invalidate();
		}
	}

}

void SettingEntry::setSelected(bool selected)
{
	if (selected != selection.isVisible())
	{
		//#5B5B5B
		auto strongColor = selected ? 0xFF000000 : 0xFFFFF5D9;
		auto weakColor = selected ? 0x66000000 : 0x66FFF5D9;

		for (int i = 0; i < circles.getNumberOfItems(); i++)
		{
			circlesListItems[i].setColor(weakColor);
		}

		circlesListItems[selectedIndex].setColor(strongColor);

		title.setColor(strongColor);
		selectedValue.setColor(strongColor);
		selection.setVisible(selected);
		invalidate();
	}
}

