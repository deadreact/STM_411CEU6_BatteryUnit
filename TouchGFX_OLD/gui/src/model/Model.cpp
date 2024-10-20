#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <main.h>

Model::Model() : modelListener(0)
{

}

void Model::tick()
{
	const auto time = HAL_GetTick() / 1000;

	if (m_time != time)
	{
		m_time = time;
		modelListener->OnTimeChanged();
	}
}
