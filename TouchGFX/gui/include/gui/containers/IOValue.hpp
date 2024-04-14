#ifndef IOVALUE_HPP
#define IOVALUE_HPP

#include <gui_generated/containers/IOValueBase.hpp>

class IOValue : public IOValueBase
{
public:
    IOValue();
    virtual ~IOValue() {}

    virtual void initialize();
    void setValue(float val);

protected:
    enum class State { Idle, Charge, Uncharge };

    void setState(State state);
protected:
    float m_value{0.f};
    State m_state{State::Idle};
};

#endif // IOVALUE_HPP
