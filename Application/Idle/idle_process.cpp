/*
 * idle_process.cpp
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#include "idle_process.h"
#include <shared_data.h>
#include "app_touchgfx.h"
#include "stm32f4xx_hal.h"
#include "../button.h"

// ----------------- tmp here --------------------------
class ScreenBrightnessController
{
public:
	ScreenBrightnessController()
	{
		ApplyBrightness();
	}

	void toggleScreen() {
		screenOn = !screenOn;
		ApplyBrightness();

		if (!screenOn)
		{
			__disable_irq();
			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
			__enable_irq();
		}
	}

	void incBrightness() {
		brightness = (brightness + 40) % 1000;
		ApplyBrightness();
	}
private:
	void ApplyBrightness() {
		brightnessHandle = screenOn ? brightness : 0;
	}
private:
	volatile uint32_t& brightnessHandle {TIM2->CCR4};
	uint32_t brightness{500};
	bool screenOn {true};
	ButtonEvent lastEvent {ButtonEvent::NoEvent};
};

class KeyButtonToScreenHandler
{
public:
	void handleEvent(ButtonEvent event)
	{
		switch (event)
		{
			case ButtonEvent::Release:
			{
				if (lastEvent == ButtonEvent::Press) {
					controller.toggleScreen();
				}
				lastEvent = event;

			} break;
			case ButtonEvent::Press:
				lastEvent = event;
				break;
			case ButtonEvent::Hold:
			{
				controller.incBrightness();
				lastEvent = event;
			} break;
			default:
				break;
		}
	}
private:
	ScreenBrightnessController controller;
	ButtonEvent lastEvent{ButtonEvent::NoEvent};
};

// ---------------------------------------------------------------

struct IdleProcess::Impl
{
	void OnTick();
	void HandleEvents();

	ButtonEventProvider btnEvProvider{GPIOA, GPIO_PIN_0};
	KeyButtonToScreenHandler btnEventHandler;

	ProcessData<ProcessId::Idle> sharedData;
};

void IdleProcess::Impl::OnTick()
{
	btnEvProvider.onTick();
}

void IdleProcess::Impl::HandleEvents()
{
	btnEventHandler.handleEvent(btnEvProvider.getLastEvent());
}


// -----------------------------------------------------------------

IdleProcess::~IdleProcess()
{
	delete m_pimpl;
}

void IdleProcess::Init()
{
	m_pimpl = new IdleProcess::Impl;
	SharedData::get().setProcessId<ProcessId::Idle>(&m_pimpl->sharedData);
}

void IdleProcess::Run()
{
	m_isRunning = true;

	while (m_isRunning)
	{
		m_pimpl->OnTick();
		m_pimpl->HandleEvents();
		MX_TouchGFX_Process();
	}
}


