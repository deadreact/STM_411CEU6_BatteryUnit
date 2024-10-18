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
    int m_selectedEntry{-1};
    utils::stack_vector<uint16_t, 3> m_selectedSettings = {0, 0, 0};
//    SettingsData m_data{nullptr};
};

#endif // SETTINGS_HPP
