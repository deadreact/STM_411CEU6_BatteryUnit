#include <gui/idlescreen_screen/IdleScreenView.hpp>

IdleScreenView::IdleScreenView()
{

}

void IdleScreenView::setupScreen()
{
    IdleScreenViewBase::setupScreen();
}

void IdleScreenView::tearDownScreen()
{
    IdleScreenViewBase::tearDownScreen();
}

void IdleScreenView::handleTickEvent()
{
    if (SharedData::getProcessId() == ProcessId::Idle)
    {
        const auto& data = SharedData::getData<ProcessId::Idle>();

        if (data.bmsError != error)
        {
        	error = data.bmsError;
        	errorLabel.setVisible(error);
        	errorLabel.invalidate();
        	containerCurrent.setVisible(!error);
        	containerCurrent.invalidate();
        	containerCapacity.setVisible(!error);
        	containerCapacity.invalidate();
        }

        if (error) {
        	return;
        }

        if (data.batCapacity != capacity)
        {
        	capacity = data.batCapacity;
            capacityValue.setValue(capacity);

            capacityTextValue.setWildcard1(capacityTextValueBuffer);
            Unicode::snprintf(capacityTextValueBuffer, CAPACITYTEXTVALUE_SIZE, "%d", capacity);
            capacityTextValue.invalidate();
        }

        if (data.batCurrent != current)
        {
            current = data.batCurrent;
            currentValue.setValue(current * 100 + 50);

			currentTextValue.setWildcard1(currentTextValueBuffer);
			Unicode::snprintf(currentTextValueBuffer, CURRENTTEXTVALUE_SIZE, "%f", current);
			currentTextValue.invalidate();
        }
    }
}
