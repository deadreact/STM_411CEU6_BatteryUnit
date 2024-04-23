#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <gui/common/BlinkingAnimation.h>
#include <utils/revision_data.h>

enum class InverterState : uint8_t;

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
    void showLoading(bool show);
protected:
    BatteryData m_bmsData;
    InverterState m_invState;
    bool m_usbState{false};
    int m_chargeTimeSec{-1};
    bool m_isBMSError{false};
    bool m_isChargError{false};
    CTimeout m_loadingAnimTimeout{50};

    BlinkingAnimation<touchgfx::Image> m_invAnimation{icon_inv, 800};
    BlinkingAnimation<touchgfx::Image> m_warnAnimation{icon_warn, 800};

    touchgfx::Callback<Screen1View, const touchgfx::AnimationTextureMapper&> textureMapperAnimationEndedCallback;
    void textureMapperAnimationEndedCallbackHandler(const touchgfx::AnimationTextureMapper& src);

};

#endif // SCREEN1VIEW_HPP
