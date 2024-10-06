#ifndef CLOCKSCREENVIEW_HPP
#define CLOCKSCREENVIEW_HPP

#include <gui_generated/clockscreen_screen/ClockScreenViewBase.hpp>
#include <gui/clockscreen_screen/ClockScreenPresenter.hpp>
#include <gui/common/CommonScreenView.h>

class ClockScreenView : public CommonScreenView<ClockScreenViewBase>
{
public:
    virtual ~ClockScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;

    void updateTime();
protected:
    Timeout m_timer{500};
};

#endif // CLOCKSCREENVIEW_HPP
