/*
 * settings_handler.h
 *
 *  Created on: Sep 29, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef SRC_HANDLERS_SETTINGS_HANDLER_H_
#define SRC_HANDLERS_SETTINGS_HANDLER_H_

#include <gpio_wrappers/button.h>
#include <main.h> // GPIO defines & HAL
#include <settings_data.h>

class SettingsHandler
{
public:
	enum class Index : uint8_t
	{
		ChargerPower = 0,
		ScreenType,
		Brightness
	};
    SettingsHandler();

    void onTick();
    void handleEvents();

    int getValue(SettingsData::SettingIndex setting) const { return m_data.getValue(setting); }
    bool isSettingsActive() const { return m_data.active; }
    const SettingsData& getData() const { return m_data; }
private:
    void onBttn1Clicked();
    void onBttn1Hold();
    void onBttn2Clicked();
private:

    ButtonEventProvider m_btn1{bttn_settings_1_GPIO_Port, bttn_settings_1_Pin};
    ButtonEventProvider m_btn2{bttn_settings_2_GPIO_Port, bttn_settings_2_Pin};

    ButtonEventHandler m_btn1Handler{&m_btn1, [&]{ onBttn1Clicked(); }, [&]{ onBttn1Hold(); }};
    ButtonEventHandler m_btn2Handler{&m_btn2, [&]{ onBttn2Clicked(); }};

    SettingsData m_data;
};


#endif /* SRC_HANDLERS_SETTINGS_HANDLER_H_ */
