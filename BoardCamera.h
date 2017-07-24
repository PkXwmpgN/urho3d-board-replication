#ifndef _BOARD_CAMERA_H_INCLUDED__
#define _BOARD_CAMERA_H_INCLUDED__

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

namespace Urho3D
{
    class Context;
    class Node;
}

class BoardCamera : public LogicComponent
{
    URHO3D_OBJECT(BoardCamera, LogicComponent);

public:

    explicit BoardCamera(Context * context);
    static void RegisterObject(Context * context);

protected:

    void OnNodeSet(Node * node);
    
private:

    void HandleUpdate(StringHash eventType, VariantMap & eventData);

private:

    WeakPtr<Node> cameraNode_;

    /// Camera yaw angle.
    float yaw_;
    /// Camera pitch angle.
    float pitch_;
};

#endif // _BOARD_CAMERA_H_INCLUDED__
