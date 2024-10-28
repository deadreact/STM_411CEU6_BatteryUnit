#ifndef TEMPERATURECONTAINER_HPP
#define TEMPERATURECONTAINER_HPP

#include <gui_generated/containers/TemperatureContainerBase.hpp>

class TemperatureContainer : public TemperatureContainerBase
{
public:
    TemperatureContainer();
    virtual ~TemperatureContainer() {}

    virtual void initialize();

    void setValue(int16_t value);
    void setTitle(TypedTextId textId);
protected:
    int16_t m_value{99};
    static const int16_t m_min{-20};
    static const int16_t m_max{50};
};

#endif // TEMPERATURECONTAINER_HPP
