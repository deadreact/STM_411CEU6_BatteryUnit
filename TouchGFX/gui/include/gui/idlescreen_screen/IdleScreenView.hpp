#ifndef IDLESCREENVIEW_HPP
#define IDLESCREENVIEW_HPP

#include <gui_generated/idlescreen_screen/IdleScreenViewBase.hpp>
#include <gui/idlescreen_screen/IdleScreenPresenter.hpp>

class IdleScreenView : public IdleScreenViewBase
{
public:
    IdleScreenView();
    virtual ~IdleScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;
protected:
    int capacity {0};
    float current {0.f};
    bool error{false};
};

#endif // IDLESCREENVIEW_HPP
