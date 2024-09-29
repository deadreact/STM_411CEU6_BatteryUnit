#include <gui/containers/Settings.hpp>

Settings::Settings()
{

}

void Settings::initialize()
{
    SettingsBase::initialize();
}


void Settings::setData(const SettingsData& data)
{
	if (m_data != data)
	{
		if (m_data.settings != data.settings)
		{
			scrollList1.setNumberOfItems(data.settings.size());

			for (int i = 0; i < data.settings.size(); i++)
			{
				const auto& setting = data.settings.at(i);
				scrollList1ListItems[i].setIndex(i);
				scrollList1ListItems[i].setValue(setting.getValue());
			}
		}

		if (m_data.selectedIndex != data.selectedIndex)
		{
			if (m_data.selectedIndex >= 0)
				scrollList1ListItems[m_data.selectedIndex].setSelected(false);
			if (data.selectedIndex >= 0)
				scrollList1ListItems[data.selectedIndex].setSelected(true);
		}

		m_data = data;
		invalidate();
	}
}
