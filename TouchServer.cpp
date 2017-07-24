#include "TouchServer.h"
#include "TouchEvent.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Network/Network.h>

TouchServer::TouchServer(Context * context)
    : Component(context)
{}

void TouchServer::RegisterObject(Context * context)
{
    context->RegisterFactory<TouchServer>();
}

void TouchServer::OnSceneSet(Scene * scene)
{
    if(scene)
    {
        scene_ = GetScene();
        SubscribeToEvent(E_SCENEUPDATE, [this](StringHash, VariantMap &)
        {
            auto network = GetSubsystem<Network>();
            if(network->IsServerRunning())
            {
                for(auto & connection : network->GetClientConnections())
                    ProcessConnection(connection);
            }
        });
    }
}

void TouchServer::ProcessConnection(Connection * connection)
{
    if(auto objectID = connection->GetControls().buttons_)
    {
        if(auto object = scene_->GetNode(objectID))
        {
            using namespace TouchReaction;
            SendEvent(E_TOUCHREACTION,
                P_NODE, object,
                P_CONNECTION, static_cast<Connection*>(connection));
        }
    }
}
