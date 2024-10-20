#ifndef SETTINGENTRY_HPP
#define SETTINGENTRY_HPP

#include <gui_generated/containers/SettingEntryBase.hpp>

class SettingEntry : public SettingEntryBase
{
public:
    SettingEntry();
    virtual ~SettingEntry() {}

    virtual void initialize();

    void setTitle(TypedTextId textId);
    void setValues(const std::array<TypedTextId, 5>& values);
    void selectValue(uint16_t index);
    void setSelected(bool selected);
protected:
    std::array<TypedTextId, 5> m_values;
};

#endif // SETTINGENTRY_HPP
