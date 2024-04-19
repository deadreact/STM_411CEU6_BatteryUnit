#include <gui/screen2_screen/Screen2View.hpp>
#include <touchgfx/Color.hpp>

Screen2View::Screen2View()
{

}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

void Screen2View::handleTickEvent()
{
    if (SharedData::getProcessId() == ProcessId::Idle)
    {
        const auto& data = SharedData::getData<ProcessId::Idle>();
//        if (sleepingLabel.isVisible() != data.sleepingMode)
//        {
//            sleepingLabel.setVisible(data.sleepingMode);
//            sleepingLabel.invalidate();
//            container1.setVisible(!data.sleepingMode);
//            container1.invalidate();
//        }
//
//        if (data.sleepingMode) {
//            return;
//        }

        if (data.screenBrightness != m_screenBrightness)
        {
            static const float k = 1000.f/255;
            brightnessValue.setValue(data.screenBrightness/10);
            brightnessValue.setColor(touchgfx::Color::getColorFromRGB((1000 - data.screenBrightness)/k, data.screenBrightness/k, 0));
            m_screenBrightness = data.screenBrightness;
        }

        if (data.analog1 != m_analog1 || data.analog2 != m_analog2)
        {
            m_analog1 = data.analog1;
            m_analog2 = data.analog2;
            lineProgress1.setValue(m_analog1);
            lineProgress2.setValue(m_analog2);
        }
    }
}
