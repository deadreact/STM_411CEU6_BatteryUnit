#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include <gui/containers/Settings.hpp>
#include <shared_data.h>


void Model::tick()
{
	const auto& data = SharedData::getData();

	if (m_settingsData != data.settings)
	{
		m_settingsData = data.settings;

		if (m_settingsData.active)
		{
			if (settingsPopUp == nullptr)
			{
				settingsPopUp = new Settings;
				settingsPopUp->initialize();
				settingsPopUp->setXY(40,  40);
				modelListener->onSettingsCreated();
			}

			settingsPopUp->setData(m_settingsData);
		}
		else if (settingsPopUp)
		{
			modelListener->onSettingsWillBeDestroyed();
			delete settingsPopUp;
			settingsPopUp = nullptr;
		}
	}
}
