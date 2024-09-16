#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <gui_generated/containers/SettingsBase.hpp>

class Settings : public SettingsBase
{
public:
    Settings();
    virtual ~Settings() {}

    virtual void initialize();
protected:
};

#endif // SETTINGS_HPP
