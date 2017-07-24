#include "BoardCamera.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/IO/Log.h>

BoardCamera::BoardCamera(Context * context)
    : LogicComponent(context)
    , yaw_(-40.0f)
    , pitch_(30.0f)
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(BoardCamera, HandleUpdate));
}

void BoardCamera::RegisterObject(Context * context)
{
    context->RegisterFactory<BoardCamera>();
}

void BoardCamera::OnNodeSet(Node * node)
{
    if(node)
    {
        cameraNode_ = node;
        cameraNode_->SetPosition(Vector3(20.0f, 15.0f, -25.0f));
        cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
    }
}

void BoardCamera::HandleUpdate(StringHash eventType,
                               VariantMap & eventData)
{
    if(!cameraNode_)
        return;

    // Do not move if the UI has a focused element (the console)
    auto ui = GetSubsystem<UI>();
    if(ui->GetFocusElement())
        return;

    auto input = GetSubsystem<Input>();
    ui->GetCursor()->SetVisible(!input->GetMouseButtonDown(MOUSEB_RIGHT));

    // Movement speed as world units per second
    const float MOVE_SPEED = 20.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1f;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[Update::P_TIMESTEP].GetFloat();

    // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
    // Only move the camera when the cursor is hidden
    if(!ui->GetCursor()->IsVisible())
    {
        IntVector2 mouseMove = input->GetMouseMove();
        yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
        pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
        pitch_ = Clamp(pitch_, -90.0f, 90.0f);

        // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
        cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
    }

    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    if (input->GetKeyDown(KEY_W))
        cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_S))
        cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_A))
        cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_D))
        cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
}
