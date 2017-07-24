#ifndef _TOUCH_DISPATCHER_H_INCLUDED__
#define _TOUCH_DISPATCHER_H_INCLUDED__

#include <Urho3D/Scene/Component.h>

using namespace Urho3D;

namespace Urho3D
{
    class Context;
    class Node;
    class Scene;
}

class TouchDispatcher : public Component
{
    URHO3D_OBJECT(TouchDispatcher, Component);

public:

    explicit TouchDispatcher(Context * context);
    static void RegisterObject(Context * context);

    void SetDistance(float distance);

protected:

    void OnSceneSet(Scene * scene);

private:

    Node * Raycast(float distance, IntVector2 origin);

private:

    WeakPtr<Scene> scene_;
    float distance_;
};

#endif // _TOUCH_DISPATCHER_H_INCLUDED__
