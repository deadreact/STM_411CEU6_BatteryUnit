#ifndef FRONTENDAPPLICATION_HPP
#define FRONTENDAPPLICATION_HPP

#include <gui_generated/common/FrontendApplicationBase.hpp>
#include <shared_data.h>

class FrontendHeap;

using namespace touchgfx; // @suppress("Using directive in header")

class FrontendApplication : public FrontendApplicationBase
{
public:
    FrontendApplication(Model& m, FrontendHeap& heap);
    virtual ~FrontendApplication() { }

    virtual void handleTickEvent() override;
    void showScreen(ScreenId id);
private:
    ScreenId m_screenId{ScreenId::DefaultScreen};
};

#endif // FRONTENDAPPLICATION_HPP
