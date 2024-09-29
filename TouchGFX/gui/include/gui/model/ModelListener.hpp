#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

class Settings;

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    virtual void onSettingsCreated(Settings* settings) {}
    virtual void onSettingsWillBeDestroyed() {}
protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
