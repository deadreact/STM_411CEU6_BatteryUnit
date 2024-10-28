#include <gui/containers/CircleContainer.hpp>

CircleContainer::CircleContainer()
{

}

void CircleContainer::initialize()
{
    CircleContainerBase::initialize();
}

void CircleContainer::setColor(uint32_t color)
{
	circle.setAlpha(color >> 24);
	circlePainter.setColor(color);
	invalidate();
}
