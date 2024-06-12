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
    void showScreen(int id);
private:
    int m_screenId{2};
};

#endif // FRONTENDAPPLICATION_HPP
