#ifndef MAINSCREENVIEW_HPP
#define MAINSCREENVIEW_HPP

#include <gui_generated/mainscreen_screen/MainScreenViewBase.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>
#include <gui/common/BlinkingAnimation.h>
#include <utils/revision_data.h>
#include <gui/containers/Settings.hpp>

enum class InverterState : uint8_t;
enum class PowerModeState: uint8_t;

class MainScreenView : public MainScreenViewBase
{
public:
    MainScreenView();
    MainScreenView(const MainScreenView&) = delete;
    MainScreenView& operator=(const MainScreenView&) = delete;
    MainScreenView(MainScreenView&&) = default;
    MainScreenView& operator=(MainScreenView&&) = default;

    virtual ~MainScreenView();
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;
    void setWatts(int val);

    Settings* takeSettings();
	void giveSettings(Settings* settings);
protected:
    void updateBatteryData(const BatteryData& data);
    void setIconFanVisible(bool visible);
    void updateInvState();
    void showLoading(bool show);
protected:
    Settings* m_settingsPopUp{nullptr};

    PowerModeState m_powerModeState;
    BatteryData m_bmsData;
    InverterState m_invState;
    bool m_usbState{false};
    int m_chargeTimeSec{-1};
    bool m_isBMSError{false};
    bool m_isChargError{false};

    CTimeout m_loadingAnimTimeout{50};

    BlinkingAnimation<touchgfx::Image> m_invAnimation{icon_inv, 800};
    BlinkingAnimation<touchgfx::Image> m_warnAnimation{icon_warn, 800};

    touchgfx::Callback<MainScreenView, const touchgfx::AnimationTextureMapper&> textureMapperAnimationEndedCallback;
    void textureMapperAnimationEndedCallbackHandler(const touchgfx::AnimationTextureMapper& src);

};

#endif // MAINSCREENVIEW_HPP
