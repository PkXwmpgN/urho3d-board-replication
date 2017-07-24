#ifndef _TOUCH_EVENT_H_INCLUDED__
#define _TOUCH_EVENT_H_INCLUDED__

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_TOUCHOBJECT, TouchObject)
{}

URHO3D_EVENT(E_TOUCHREACTION, TouchReaction)
{
    URHO3D_PARAM(P_NODE, Node);
    URHO3D_PARAM(P_CONNECTION, Connection);
}

#endif // _TOUCH_EVENT_H_INCLUDED__
