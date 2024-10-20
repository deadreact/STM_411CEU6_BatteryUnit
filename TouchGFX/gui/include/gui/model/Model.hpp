#ifndef MODEL_HPP
#define MODEL_HPP

#include <settings_data.h>

class Settings;
class ModelListener;

class Model
{
public:
    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();

    Settings* settingsPopUp{nullptr};
protected:
    ModelListener* modelListener{nullptr};
private:
    SettingsData m_settingsData;

};

#endif // MODEL_HPP
