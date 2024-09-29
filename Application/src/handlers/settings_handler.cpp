/*
 * settings_handler.cpp
 *
 *  Created on: Sep 29, 2024
 *      Author: Dmitriy.Gyr
 */

#include <handlers/settings_handler.h>

SettingsHandler::SettingsHandler()
{

}

void SettingsHandler::onTick()
{
	m_btn1.onTick();
	m_btn2.onTick();
}
void SettingsHandler::handleEvents()
{
	m_btn1Handler.handleEvents();
	m_btn2Handler.handleEvents();
}

void SettingsHandler::onBttn1Clicked()
{
	if (m_data.active)
	{
		m_data.selectedIndex = (m_data.selectedIndex + 1) % m_data.settings.size();
	}
}

void SettingsHandler::onBttn1Hold()
{
	if (!m_data.settings.empty())
	{
		m_data.active = !m_data.active;
	}
}

void SettingsHandler::onBttn2Clicked()
{
	if (m_data.active)
	{
		SettingsEntryData& setting = m_data.settings.at(m_data.selectedIndex);
		setting.selectedIndex = (setting.selectedIndex + 1) % setting.values.size();
	}
}
