#ifndef _TOUCH_SERVER_H_INCLUDED__
#define _TOUCH_SERVER_H_INCLUDED__

#include <Urho3D/Scene/Component.h>

using namespace Urho3D;

namespace Urho3D
{
    class Context;
    class Node;
    class Scene;
    class connection;
}

class TouchServer : public Component
{
    URHO3D_OBJECT(TouchServer, Component);

public:

    explicit TouchServer(Context * context);
    static void RegisterObject(Context * context);

protected:

    void OnSceneSet(Scene * scene);

private:

    void ProcessConnection(Connection * connection);

private:

    WeakPtr<Scene> scene_;
};

#endif // _TOUCH_SERVER_H_INCLUDED__
