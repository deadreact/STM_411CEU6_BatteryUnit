#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;
    void setWatts(int val);
protected:
    void updateBatteryData(const BatteryData& data);
    void setIconFanVisible(bool visible);
    void setIconInvVisible(bool visible);
protected:
    BatteryData m_bmsData;
    int m_chargeTimeMins{-1};
    bool m_isBMSError{false};

    touchgfx::Callback<Screen1View, const touchgfx::AnimationTextureMapper&> textureMapperAnimationEndedCallback;
    void textureMapperAnimationEndedCallbackHandler(const touchgfx::AnimationTextureMapper& src);

};

#endif // SCREEN1VIEW_HPP
