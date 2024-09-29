#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <gui_generated/containers/SettingsBase.hpp>
#include <settings_data.h>

class Settings : public SettingsBase
{
public:
    Settings();
    virtual ~Settings() {}

    virtual void initialize();

    void setData(const SettingsData& data);
protected:
    SettingsData m_data{nullptr};
};

#endif // SETTINGS_HPP
