#ifndef IOVALUE_HPP
#define IOVALUE_HPP

#include <gui_generated/containers/IOValueBase.hpp>

class IOValue : public IOValueBase
{
public:
    IOValue();
    virtual ~IOValue() {}

    virtual void initialize();
protected:
};

#endif // IOVALUE_HPP
