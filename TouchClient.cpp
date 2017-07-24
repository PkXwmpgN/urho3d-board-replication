#include "TouchClient.h"
#include "TouchEvent.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/IO/Log.h>

TouchClient::TouchClient(Context * context)
    : LogicComponent(context)
{}

void TouchClient::RegisterObject(Context * context)
{
    context->RegisterFactory<TouchClient>();
}

void TouchClient::OnNodeSet(Node * node)
{
    if(node)
    {
        SubscribeToEvent(node, E_TOUCHOBJECT, [this](StringHash, VariantMap &)
        {
            if(auto connection = GetSubsystem<Network>()->GetServerConnection())
            {
                Controls controls;
                controls.Set(GetNode()->GetID(), true);
                connection->SetControls(controls);
            }
        });
    }
}
