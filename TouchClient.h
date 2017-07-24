#ifndef _TOUCH_CLIENT_H_INCLUDED__
#define _TOUCH_CLIENT_H_INCLUDED__

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

namespace Urho3D
{
    class Context;
    class Node;
}

class TouchClient : public LogicComponent
{
    URHO3D_OBJECT(TouchClient, LogicComponent);

public:

    explicit TouchClient(Context * context);
    static void RegisterObject(Context * context);

protected:

    void OnNodeSet(Node * node);
};

#endif // _TOUCH_CLIENT_H_INCLUDED__
