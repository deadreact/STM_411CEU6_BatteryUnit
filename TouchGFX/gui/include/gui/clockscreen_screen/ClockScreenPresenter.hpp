#ifndef CLOCKSCREENPRESENTER_HPP
#define CLOCKSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ClockScreenView;

class ClockScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ClockScreenPresenter(ClockScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~ClockScreenPresenter() {}

private:
    ClockScreenPresenter();

    ClockScreenView& view;
};

#endif // CLOCKSCREENPRESENTER_HPP
