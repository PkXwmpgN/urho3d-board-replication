//
// Copyright (c) 2008-2017 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>

#include "Board.h"
#include "BoardState.h"
#include "BoardCamera.h"

#include "TouchDispatcher.h"
#include "TouchClient.h"
#include "TouchServer.h"
#include "TouchEvent.h"


// UDP port we will use
static const unsigned short SERVER_PORT = 2345;

URHO3D_DEFINE_APPLICATION_MAIN(Board)

Board::Board(Context* context)
    : Application(context)
{
    TouchDispatcher::RegisterObject(context);
    TouchClient::RegisterObject(context);
    TouchServer::RegisterObject(context);
    BoardState::RegisterObject(context);
    BoardCamera::RegisterObject(context);
}

void Board::Setup()
{
    engineParameters_[EP_FULL_SCREEN]  = false;
}

void Board::Start()
{
    // Create the scene content
    CreateScene();

    // Create the UI content
    CreateUI();

    // Setup the viewport for displaying the scene
    SetupViewport();

    // Hook up to necessary events
    SubscribeToEvents();
}

void Board::CreateScene()
{
    scene_ = MakeShared<Scene>(context_);

    // Create octree and physics world with default settings.
    // Create them as local so that they are not needlessly replicated
    // when a client connects
    scene_->CreateComponent<Octree>(LOCAL);
    scene_->CreateComponent<PhysicsWorld>(LOCAL);

    // All static scene content and the camera are also created as local, so that they are unaffected by scene replication and are
    // not removed from the client upon connection. Create a Zone component first for ambient lighting & fog control.
    auto zoneNode = scene_->CreateChild("Zone", LOCAL);
    auto zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
    zone->SetAmbientColor(Color(0.5f, 0.5f, 0.5f));
    zone->SetFogColor(Color(0.8f, 0.8f, 0.8f));
    zone->SetFogStart(100.0f);
    zone->SetFogEnd(300.0f);

    // Create a directional light without shadows
    auto lightNode = scene_->CreateChild("DirectionalLight", LOCAL);
    lightNode->SetDirection(Vector3(0.5f, -1.0f, 0.5f));
    auto light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetColor(Color(0.2f, 0.2f, 0.2f));
    light->SetSpecularIntensity(1.0f);

    // Create the camera. Limit far clip distance to match the fog
    // The camera needs to be created into a local node so that each client can retain its own camera, that is unaffected by
    // network messages. Furthermore, because the client removes all replicated scene nodes when connecting to a server scene,
    // the screen would become blank if the camera node was replicated (as only the locally created camera is assigned to a
    // viewport in SetupViewports() below)
    cameraNode_ = scene_->CreateChild("Camera", LOCAL);
    Camera* camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFarClip(100.0f);

    cameraNode_->CreateComponent<BoardCamera>();
}

void Board::CreateUI()
{
    auto cache = GetSubsystem<ResourceCache>();
    auto ui = GetSubsystem<UI>();

    auto root = ui->GetRoot();
    auto uiStyle = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    // Set style to the UI root so that elements will inherit it
    root->SetDefaultStyle(uiStyle);

    // Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will
    // control the camera, and when visible, it can interact with the login UI
    auto cursor = MakeShared<Cursor>(context_);
    cursor->SetStyleAuto(uiStyle);
    ui->SetCursor(cursor);

    // Set starting position of the cursor at the rendering window center
    auto graphics = GetSubsystem<Graphics>();
    cursor->SetPosition(graphics->GetWidth() / 2, graphics->GetHeight() / 2);

    buttonContainer_ = root->CreateChild<UIElement>();
    buttonContainer_->SetFixedSize(500, 20);
    buttonContainer_->SetPosition(20, 20);
    buttonContainer_->SetLayoutMode(LM_HORIZONTAL);

    textEdit_ = buttonContainer_->CreateChild<LineEdit>();
    textEdit_->SetStyleAuto();

    connectButton_ = CreateButton("Connect", 90);
    disconnectButton_ = CreateButton("Disconnect", 100);
    startServerButton_ = CreateButton("Start Server", 110);

    UpdateButtons();
}

void Board::SetupViewport()
{
    auto renderer = GetSubsystem<Renderer>();

    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    auto viewport = MakeShared<Viewport>(context_, scene_, cameraNode_->GetComponent<Camera>());
    renderer->SetViewport(0, viewport);
}

void Board::SubscribeToEvents()
{
    // Subscribe key doupwn event
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(Board, HandleKeyUp));

    // Subscribe to button actions
    SubscribeToEvent(connectButton_, E_RELEASED, URHO3D_HANDLER(Board, HandleConnect));
    SubscribeToEvent(disconnectButton_, E_RELEASED, URHO3D_HANDLER(Board, HandleDisconnect));
    SubscribeToEvent(startServerButton_, E_RELEASED, URHO3D_HANDLER(Board, HandleStartServer));

    // Subscribe to network events
    SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(Board, HandleConnectionStatus));
    SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(Board, HandleConnectionStatus));
    SubscribeToEvent(E_CONNECTFAILED, URHO3D_HANDLER(Board, HandleConnectionStatus));
    SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(Board, HandleClientConnected));
    SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(Board, HandleClientDisconnected));
}

Button* Board::CreateButton(const String& text, int width)
{
    auto cache = GetSubsystem<ResourceCache>();
    auto font = cache->GetResource<Font>("Fonts/Anonymous Pro.ttf");

    auto button = buttonContainer_->CreateChild<Button>();
    button->SetStyleAuto();
    button->SetFixedWidth(width);

    auto buttonText = button->CreateChild<Text>();
    buttonText->SetFont(font, 12);
    buttonText->SetAlignment(HA_CENTER, VA_CENTER);
    buttonText->SetText(text);

    return button;
}

void Board::UpdateButtons()
{
    auto network = GetSubsystem<Network>();
    auto serverConnection = network->GetServerConnection();
    bool serverRunning = network->IsServerRunning();

    // Show and hide buttons so that eg. Connect and Disconnect are never shown at the same time
    connectButton_->SetVisible(!serverConnection && !serverRunning);
    disconnectButton_->SetVisible(serverConnection || serverRunning);
    startServerButton_->SetVisible(!serverConnection && !serverRunning);
    textEdit_->SetVisible(!serverConnection && !serverRunning);
}

void Board::CreateBoard()
{
    auto cache = GetSubsystem<ResourceCache>();

    const int BOARD_SIZE = 10;
    for(int y = -BOARD_SIZE; y < BOARD_SIZE; ++y)
    {
        for(int x = -BOARD_SIZE; x < BOARD_SIZE; ++x)
        {
            auto node = scene_->CreateChild("Cell");
            node->SetPosition(Vector3(x * 1.6f, 0.0f, y * 1.6f));
            node->SetScale(Vector3(1.5f, 0.5f, 1.5f));

            auto model = node->CreateComponent<StaticModel>();
            model->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
            model->SetMaterial(cache->GetResource<Material>("Materials/Empty.xml"));

            node->CreateComponent<TouchClient>();
            node->CreateComponent<BoardState>(LOCAL);
        }
    }
}

void Board::HandleKeyUp(StringHash , VariantMap& eventData)
{
    using namespace KeyUp;

    // Close console (if open) or exit when ESC is pressed
    if(eventData[P_KEY].GetInt() == KEY_ESCAPE)
        engine_->Exit();
}

void Board::HandleTouchReaction(StringHash eventType, VariantMap& eventData)
{
    using namespace TouchReaction;

    auto connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    auto node = static_cast<Node*>(eventData[P_NODE].GetPtr());
    auto state = node->GetComponent<BoardState>();
    if(!state->GetBusy())
    {
        state->SetBusy(true);
        auto model = node->GetComponent<StaticModel>();
        model->SetMaterial(connectionResources_[connection]->material_);
    }
}

void Board::HandleConnect(StringHash eventType, VariantMap& eventData)
{
    auto network = GetSubsystem<Network>();
    String address = textEdit_->GetText().Trimmed();
    if(address.Empty())
        address = "localhost";

    network->Connect(address, SERVER_PORT, scene_);
    scene_->CreateComponent<TouchDispatcher>(LOCAL);

    UpdateButtons();
}

void Board::HandleDisconnect(StringHash eventType, VariantMap& eventData)
{
    auto network = GetSubsystem<Network>();
    if(auto serverConnection = network->GetServerConnection())
    {
        serverConnection->Disconnect();
        scene_->Clear(true, false);
    }
    else if(network->IsServerRunning())
    {
        network->StopServer();
        scene_->Clear(true, false);
    }

    UpdateButtons();
}

void Board::HandleStartServer(StringHash eventType, VariantMap& eventData)
{
    GetSubsystem<Network>()->StartServer(SERVER_PORT);

    CreateBoard();

    scene_->CreateComponent<TouchServer>(LOCAL);
    SubscribeToEvent(E_TOUCHREACTION, URHO3D_HANDLER(Board, HandleTouchReaction));

    static String resources[] =
    {
        "Materials/Red.xml",
        "Materials/Blue.xml",
        "Materials/Green.xml",
        "Materials/Yellow.xml"
    };

    auto cache = GetSubsystem<ResourceCache>();

    freeResources_.Clear();
    for(auto & resource : resources)
    {
        auto material = cache->GetResource<Material>(resource);
        freeResources_.Push(MakeShared<ClientResources>(material));
    }

    UpdateButtons();
}

void Board::HandleConnectionStatus(StringHash eventType, VariantMap& eventData)
{
    UpdateButtons();
}

void Board::HandleClientConnected(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientConnected;

    auto connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    if(freeResources_.Empty())
    {
        connection->Disconnect();
        return;
    }

    connection->SetScene(scene_);
    connectionResources_[connection] = freeResources_.Front();
    freeResources_.PopFront();
}

void Board::HandleClientDisconnected(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientConnected;

    auto connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    if(auto resource = connectionResources_[connection])
        freeResources_.Push(resource);
}
