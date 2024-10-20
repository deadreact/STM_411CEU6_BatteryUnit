#ifndef MODEL_HPP
#define MODEL_HPP

#include <cinttypes>

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();

    int64_t m_time{0};
protected:
    ModelListener* modelListener;
};

#endif // MODEL_HPP
