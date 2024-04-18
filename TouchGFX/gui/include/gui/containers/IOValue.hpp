#ifndef IOVALUE_HPP
#define IOVALUE_HPP

#include <gui_generated/containers/IOValueBase.hpp>
#include <gui/common/common.h>

class IOValue : public IOValueBase
{
public:
    IOValue();
    virtual ~IOValue() {}

    virtual void initialize();
    void setValue(int val);

protected:
    void setState(ChargeState state);
protected:
    int m_value{0};
    ChargeState m_state{ChargeState::Idle};
};

#endif // IOVALUE_HPP
