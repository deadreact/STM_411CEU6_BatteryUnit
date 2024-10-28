#ifndef SETTINGENTRY_HPP
#define SETTINGENTRY_HPP

#include <gui_generated/containers/SettingEntryBase.hpp>
#include <utils/stack_vector.h>

class SettingEntry : public SettingEntryBase
{
public:
    SettingEntry();
    virtual ~SettingEntry() {}

    virtual void initialize();

    void setTitle(TypedTextId textId);
    void setValues(const utils::stack_vector<TypedTextId, 5>& values);
    void selectValue(uint16_t index);
    void setSelected(bool selected);
protected:
    utils::stack_vector<TypedTextId, 5> m_values;
    uint16_t selectedIndex{0};
};

#endif // SETTINGENTRY_HPP
