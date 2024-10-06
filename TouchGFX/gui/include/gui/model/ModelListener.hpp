#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

class Settings;

class ModelListener
{
public:
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    virtual void onSettingsCreated() {}
    virtual void onSettingsWillBeDestroyed() {}
protected:
    Model* model{nullptr};
};

#endif // MODELLISTENER_HPP
