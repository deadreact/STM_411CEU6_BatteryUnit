#ifndef MODEL_HPP
#define MODEL_HPP

class Settings;

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

    Settings* settingsPopUp{nullptr};
protected:
    ModelListener* modelListener;

};

#endif // MODEL_HPP
