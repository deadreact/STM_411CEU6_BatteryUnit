/*
 * program.cpp
 *
 *  Created on: Apr 4, 2024
 *      Author: Dmitriy.Gyr
 */

#include "program.h"
#include "button.h"



class Program
{
public:
	Program(const ProgramDescriptor& desc)
		: brightnessHandle(*desc.brightnessHandle)
		, btnHandler(Button(desc.btnGPIOx, desc.btnPin))
	{}
    void init();
    void onTick();
    void handleEvents();

    volatile uint32_t& brightnessHandle;
    ButtonEventProvider btnHandler;

    uint32_t brightness{500};
    bool screenOn {true};
    ButtonEvent lastEvent {ButtonEvent::NoEvent};
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
	brightnessHandle = screenOn ? brightness : 0;
}

void Program::onTick()
{
	btnHandler.onTick();
}

void Program::handleEvents()
{
	switch (btnHandler.getLastEvent())
	{
		case ButtonEvent::Release:
		{
			if (lastEvent == ButtonEvent::Press) {
				screenOn = !screenOn;
			}
			lastEvent = btnHandler.getLastEvent();

		} break;
		case ButtonEvent::Press:
			lastEvent = btnHandler.getLastEvent();
			break;
		case ButtonEvent::Hold:
		{
			brightness = (brightness + 100) % 1000;
			lastEvent = btnHandler.getLastEvent();
		} break;
		default:
			break;
	}

	brightnessHandle = screenOn ? brightness : 0;
}
