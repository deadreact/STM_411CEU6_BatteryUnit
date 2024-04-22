#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>
#include <application_utils.h>

class Screen2View : public Screen2ViewBase
{
public:
    Screen2View();
    virtual ~Screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;

    void updateTime();
protected:
    int m_screenBrightness{0};
    int m_analog1{0};
    int m_analog2{0};

    Timeout m_timer{500};
};

#endif // SCREEN2VIEW_HPP
