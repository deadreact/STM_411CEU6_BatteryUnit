#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

extern int gCurrentProcess;
extern int gTimeLeftToSleep;

Model::Model() : modelListener(0)
{

}

void Model::tick()
{
	if (gCurrentProcess == 1)
	{
		m_timer = gTimeLeftToSleep / 1000;
	}
	else
	{
		m_timer = -1;
	}
}
