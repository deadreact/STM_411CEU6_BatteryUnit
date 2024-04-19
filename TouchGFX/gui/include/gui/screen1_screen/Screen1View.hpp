#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

enum class InverterState;

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
    void updateBatteryData(const BatteryData& data, int16_t smoothedCurr);
    void setIconFanVisible(bool visible);
    void updateInvState();
    void showContent(bool show);
protected:
    BatteryData m_bmsData;
    InverterState m_invState;
    int m_chargeTimeMins{-1};
    bool m_isBMSError{false};
    uint32_t m_loadingAnimTimeout{0};

    touchgfx::Callback<Screen1View, const touchgfx::AnimationTextureMapper&> textureMapperAnimationEndedCallback;
    void textureMapperAnimationEndedCallbackHandler(const touchgfx::AnimationTextureMapper& src);

};

#endif // SCREEN1VIEW_HPP
