#ifndef SETTINGENTRY_HPP
#define SETTINGENTRY_HPP

#include <gui_generated/containers/SettingEntryBase.hpp>

class SettingEntry : public SettingEntryBase
{
public:
    SettingEntry();
    virtual ~SettingEntry() {}

    virtual void initialize();
protected:
};

#endif // SETTINGENTRY_HPP
