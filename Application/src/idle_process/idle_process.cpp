/*
 * idle_process.cpp
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#include "idle_process.h"
#include "ili9341.h"

#include "power_modes.h"
#include <gpio_wrappers/button.h>
#include <gpio_wrappers/led.h>
#include <gpio_wrappers/io_tube.h>
#include <gpio_wrappers/usb_handler.h>
#include <gpio_wrappers/tft_display_320x240.h>
#include "main.h"
#include <cstring>
#include <shared_data.h>

#include <bms/bms_handler.h>
#include <inverter/inverter_handler.h>
#include "../charger_handler.h"

// ---------------------------------------------------------------

struct IdleProcess::Impl
{
	void displayDeepReset();

    void onTick();
    void handleEvents();

    void handlePowerState();

    void updateAnalog();

    void onPwrClick();
    void onPwrHold();
    void onPwrPress();
    void onPwrRelease();

    // Data
    ProcessData sharedData;
    Timeout m_wakedUpTimeout;
    // Handlers
    BMSUpdater m_bmsUpdater;
    InverterHandler m_invHandler;
    USBHandler m_usbHandler;
    ChargerHandler m_chargerHandler;

//    TFTDisplay320x240 screen{LED_GPIO_Port, LED_Pin};

    Display320x240 screen;
    LedIndicator screenLed{bttn_screen_led_GPIO_Port, bttn_screen_led_Pin, LedIndicationType::Blinking};
    ButtonEventProvider btnScrSwitch{GPIOA, GPIO_PIN_0, 800, 800};
    ButtonEventProvider btnPwr{bttn_screen_on_GPIO_Port, bttn_screen_on_Pin};
    ButtonEventHandler btnScrSwitchHandler{&btnScrSwitch
   	, [&]{ displayDeepReset();/*sharedData.screenId = (sharedData.screenId + 1) % 3;*/ }
    };

    ButtonEventHandler btnPwrHandler{&btnPwr, [&]{ onPwrClick();}, [&]{ onPwrHold();}};
};


void IdleProcess::Impl::displayDeepReset()
{
	screen.setActive(false);

	LCD_Reboot();

	screen.setActive(true);
}

void IdleProcess::Impl::onTick()
{
	m_invHandler.onTick();
	m_usbHandler.onTick();

	screenLed.onTick();
    btnScrSwitch.onTick();
    btnPwr.onTick();

    m_bmsUpdater.update();
    m_chargerHandler.update();

//    if (sharedData.screenId == 2) {
//        updateAnalog();
//    }

    screen.onTick();
}

void IdleProcess::Impl::handleEvents()
{
    btnScrSwitchHandler.handleEvents();
    btnPwrHandler.handleEvents();
    m_usbHandler.handleEvents();
    m_invHandler.handleEvents();

    sharedData.invState = m_invHandler.getState();
    sharedData.usbState = m_usbHandler.isOn();

    BMSUpdaterEvent bmsEvent = m_bmsUpdater.takeLastEvent();
    if (bmsEvent != BMSUpdaterEvent::NoEvent)
    {
        sharedData.errMsg = m_bmsUpdater.debugMsg;
        sharedData.errFlags = m_bmsUpdater.errFlags;

        if (bmsEvent == BMSUpdaterEvent::DataUpdated) {
        	sharedData.updateBatteryData(m_bmsUpdater.getData());

        	if (sharedData.getPowerModeState() == PowerModeState::WakedUp)
        	{
        		m_wakedUpTimeout.reset(0);
        	}

//        	sharedData.bms = m_bmsUpdater.getData();
//        	if (sharedData.bms.soc > 0) {
//        		//TODO: find better solution (backup register)
//        		sharedData.lastPositiveSOC = sharedData.bms.soc;
//        	}
//        	if (sharedData.isBMSDataValid()) {
//        		sharedData.smoothedCurrent.set(sharedData.bms.current);
//        	} else {
//        		sharedData.smoothedCurrent.reset();
//        	}

        }
    }

    const auto chargState = m_chargerHandler.getState();
    if (chargState == ChargerState::Error) {
    	if (bmsEvent != BMSUpdaterEvent::NoEvent) {
    		sharedData.errMsg += " charge err";
    		sharedData.errFlags |= 0x01000000;
    	} else {
    		sharedData.errMsg = "charge err";
    		sharedData.errFlags |= 0x01000000;
    	}
	}
    else
    {
    	sharedData.errFlags &= ~0x01000000;
    }
    sharedData.chargerPlugged = m_chargerHandler.isDcOk();

    handlePowerState();
}

void IdleProcess::Impl::handlePowerState()
{
	if (sharedData.getPowerModeState() == PowerModeState::StopRequested)
	{
		m_bmsUpdater.setActive(false);
		m_invHandler.setActive(false);
		m_usbHandler.setActive(false);
		screenLed.setIndicationType(LedIndicationType::FastBlinking);
		if (m_bmsUpdater.getStatus() == BMSStatus::NoStatus)
		{
//        	if (NVIC_GetEnableIRQ(bttn_screen_on_EXTI_IRQn))
			if (!btnPwr.isPressed())
			{
				screen.off();
				screenLed.setIndicationType(LedIndicationType::Off);
				sharedData.setPowerModeState(PowerModeState::StopReady);
			}
		}
	}
	else
	{
		if (sharedData.getPowerModeState() == PowerModeState::WakedUp)
		{
//			sharedData.smoothedCurrent.reset();

			m_bmsUpdater.setActive(true);
			m_invHandler.setActive(true);
			m_usbHandler.setActive(true);
			m_chargerHandler.resetState();

			if (m_wakedUpTimeout.isReached() && !btnPwr.isPressed())
			{
				sharedData.setPowerModeState(PowerModeState::Normal);
			}
		}

		if (m_bmsUpdater.isPowerOn())
		{
			if (m_bmsUpdater.errFlags == 0 && sharedData.bms.isValid())
			{
				screenLed.setIndicationType(LedIndicationType::On);
			}
			else
			{
				screenLed.setIndicationType(LedIndicationType::ShuffleBlinking);
			}
		}
		else
		{
			screenLed.setIndicationType(LedIndicationType::Off);
		}
	}
}


void IdleProcess::Impl::onPwrPress()
{
//	HAL_NVIC_DisableIRQ(bttn_screen_on_EXTI_IRQn);
}

void IdleProcess::Impl::onPwrRelease()
{
//	HAL_NVIC_EnableIRQ(bttn_screen_on_EXTI_IRQn);
}

void IdleProcess::Impl::onPwrClick()
{
	if (sharedData.getPowerModeState() == PowerModeState::Normal) {
		screen.toggle();
	}
}

void IdleProcess::Impl::onPwrHold()
{
	if (sharedData.getPowerModeState() == PowerModeState::Normal) {
		sharedData.setPowerModeState(PowerModeState::StopRequested);
		screen.off();
	}
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------

IdleProcess::IdleProcess() : Process(5) {};

IdleProcess::~IdleProcess()
{
    delete m_pimpl;
}

void IdleProcess::init()
{
    m_pimpl = new IdleProcess::Impl;
    SharedData::shareReadOnly(m_pimpl->sharedData);
    m_pimpl->screen.on();
}

void IdleProcess::update()
{
    m_pimpl->onTick();
    m_pimpl->handleEvents();

    if (m_pimpl->sharedData.getPowerModeState() == PowerModeState::StopReady)
    {
    	HAL_NVIC_EnableIRQ(bttn_screen_on_EXTI_IRQn);
        EnterStopMode();
        m_pimpl->sharedData.setPowerModeState(PowerModeState::WakedUp);
        m_pimpl->m_wakedUpTimeout.reset(5000);
        m_pimpl->screen.on();
    }
}


