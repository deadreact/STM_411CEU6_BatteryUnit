#ifndef IOVALUE_HPP
#define IOVALUE_HPP

#include <gui_generated/containers/IOValueBase.hpp>

class IOValue : public IOValueBase
{
public:
	enum IO {In, Out};
    IOValue();
    virtual ~IOValue() {}

    virtual void initialize();
    void setValue(int val);
    void setIO(IO io);
protected:
    int m_value{-1};
};

#endif // IOVALUE_HPP
