#ifndef SETTINGENTRY_HPP
#define SETTINGENTRY_HPP

#include <gui_generated/containers/SettingEntryBase.hpp>

class SettingEntry : public SettingEntryBase
{
public:
    SettingEntry();
    virtual ~SettingEntry() {}

    virtual void initialize();

    void setIndex(int index);
    void setValue(int value);
    void setValueTextId(TypedTextId id);

    void setSelected(bool selected);
protected:
};

#endif // SETTINGENTRY_HPP
