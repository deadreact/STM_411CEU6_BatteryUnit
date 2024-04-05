/*
 * program.cpp
 *
 *  Created on: Apr 4, 2024
 *      Author: Dmitriy.Gyr
 */

#include "program.h"
#include "button.h"


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


class Program
{
public:
	Program(const ProgramDescriptor& desc)
		: btnEvProvider(desc.btnGPIOx, desc.btnPin)
		, btnEventHandler()
	{}
    void init();
    void onTick();
    void handleEvents();

    ButtonEventProvider btnEvProvider;
    KeyButtonToScreenHandler btnEventHandler;
};


static Program& getProgram(const ProgramDescriptor* desc = nullptr)
{
	static Program p(*desc);
	return p;
}

void Program_Init(const void* desc)
{
	auto& p = getProgram((ProgramDescriptor*)desc);
	p.init();
}

void Program_Process()
{
    auto& p = getProgram();
    p.onTick();
    p.handleEvents();
}


void Program::init()
{

}

void Program::onTick()
{
	btnEvProvider.onTick();
}

void Program::handleEvents()
{
	btnEventHandler.handleEvent(btnEvProvider.getLastEvent());
}
