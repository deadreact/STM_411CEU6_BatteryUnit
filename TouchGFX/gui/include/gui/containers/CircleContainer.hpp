#ifndef CIRCLECONTAINER_HPP
#define CIRCLECONTAINER_HPP

#include <gui_generated/containers/CircleContainerBase.hpp>

class CircleContainer : public CircleContainerBase
{
public:
    CircleContainer();
    virtual ~CircleContainer() {}

    virtual void initialize();
    void setColor(uint32_t color);
protected:
};

#endif // CIRCLECONTAINER_HPP
