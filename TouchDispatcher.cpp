#include "TouchDispatcher.h"
#include "TouchEvent.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/UI.h>

TouchDispatcher::TouchDispatcher(Context * context)
    : Component(context)
    , distance_(100.0f)
{}

void TouchDispatcher::RegisterObject(Context * context)
{
    context->RegisterFactory<TouchDispatcher>();
}

void TouchDispatcher::SetDistance(float distance)
{
    distance_ = distance;
}

void TouchDispatcher::OnSceneSet(Scene * scene)
{
    if(scene)
    {
        scene_ = GetScene();
        SubscribeToEvent(E_SCENEUPDATE, [this](StringHash, VariantMap &)
        {
            auto input = GetSubsystem<Input>();
            if(input->GetMouseButtonPress(MOUSEB_LEFT))
            {
                if(auto node = Raycast(distance_, GetSubsystem<UI>()->GetCursorPosition()))
                    node->SendEvent(E_TOUCHOBJECT);
            }
        });
    }
}

Node * TouchDispatcher::Raycast(float distance, IntVector2 origin)
{
    if(auto viewport = GetSubsystem<Renderer>()->GetViewport(0))
    {
        auto ray = viewport->GetScreenRay(origin.x_, origin.y_);

        PODVector<RayQueryResult> results;
        RayOctreeQuery query(results, ray, RAY_TRIANGLE, distance, DRAWABLE_GEOMETRY);
        scene_->GetComponent<Octree>()->RaycastSingle(query);

        return results.Size() ? results[0].node_ : nullptr;
    }

    return nullptr;
}
