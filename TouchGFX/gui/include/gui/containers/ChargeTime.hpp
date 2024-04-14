#ifndef CHARGETIME_HPP
#define CHARGETIME_HPP

#include <gui_generated/containers/ChargeTimeBase.hpp>

class ChargeTime : public ChargeTimeBase
{
public:
    ChargeTime();
    virtual ~ChargeTime() {}

    virtual void initialize();
    void setValue(int val);
protected:
    int m_value{0};
    constexpr static const int kYellowTreshold = 30;
    constexpr static const int kRedTreshold = 10;
};

#endif // CHARGETIME_HPP
