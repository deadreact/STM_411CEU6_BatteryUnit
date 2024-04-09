#include <gui/idlescreen_screen/IdleScreenView.hpp>
#include <cstring>

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

        if (data.batCapacity != capacity)
        {
        	capacity = data.batCapacity;
            capacityValue.setValue(capacity);

            capacityTextValue.setWildcard1(capacityTextValueBuffer);
            Unicode::snprintf(capacityTextValueBuffer, CAPACITYTEXTVALUE_SIZE, "%d", capacity);
            capacityTextValue.resizeToCurrentText();
            capacityTextValue.invalidate();
        }

        if (data.batCurrent != current)
        {
            current = data.batCurrent;
//            currentValue.setValue(current * 100 + 50);

			currentTextValue.setWildcard1(currentTextValueBuffer);
			Unicode::snprintfFloat(currentTextValueBuffer, CURRENTTEXTVALUE_SIZE, "%.2f", current);
			currentTextValue.resizeToCurrentText();
			currentTextValue.invalidate();
        }

        if (data.batVoltage != voltage)
		{
        	voltage = data.batVoltage;
//			voltageValue.setValue(voltage);

			voltageTextValue.setWildcard1(voltageTextValueBuffer);
			Unicode::snprintfFloat(voltageTextValueBuffer, VOLTAGETEXTVALUE_SIZE, "%.2f", (float)voltage * 0.01f);
			voltageTextValue.resizeToCurrentText();
			voltageTextValue.invalidate();
		}

        if (data.errMsg != errMsg)
        {
        	errMsg = data.errMsg ? data.errMsg : "no errors";
        	errorLabel.setWildcard1(errorLabelBuffer);

        	memset(errorLabelBuffer, ' ', ERRORLABEL_SIZE * 2);

        	if (errMsg)
        	{
        		for (int i = 0; errMsg[i] != '\0'; i++) {
					errorLabelBuffer[i] = errMsg[i];
				}
        	}

        	errorLabel.resizeToCurrentTextWithAlignment();
        	errorLabel.invalidate();
        }
    }
}
