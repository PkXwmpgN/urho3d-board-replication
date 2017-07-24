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

#pragma once

#include <Urho3D/Engine/Application.h>

namespace Urho3D
{
    class Button;
    class Connection;
    class Scene;
    class Node;
    class Text;
    class UIElement;
    class Drawable;
    class Material;
}

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;

class Board : public Application
{
    URHO3D_OBJECT(Board, Application);

public:

    Board(Context* context);

    /// Setup before engine initialization. Modifies the engine parameters.
    virtual void Setup();
    /// Setup after engine initialization. Creates the logo, console & debug HUD.
    virtual void Start();

private:

    /// Construct the scene content.
    void CreateScene();
    /// Construct instruction text and the login / start server UI.
    void CreateUI();
    /// Construct the board
    void CreateBoard();
    /// Set up viewport.
    void SetupViewport();
    /// Subscribe to update, UI and network events.
    void SubscribeToEvents();
    /// Create a button to the button container.
    Button* CreateButton(const String& text, int width);
    /// Update visibility of buttons according to connection and server status.
    void UpdateButtons();

    /// Handle pressing the connect button.
    void HandleConnect(StringHash eventType, VariantMap& eventData);
    /// Handle pressing the disconnect button.
    void HandleDisconnect(StringHash eventType, VariantMap& eventData);
    /// Handle pressing the start server button.
    void HandleStartServer(StringHash eventType, VariantMap& eventData);
    /// Handle connection status change (just update the buttons that should be shown.)
    void HandleConnectionStatus(StringHash eventType, VariantMap& eventData);
    /// Handle a client connecting to the server.
    void HandleClientConnected(StringHash eventType, VariantMap& eventData);
    /// Handle a client disconnecting from the server.
    void HandleClientDisconnected(StringHash eventType, VariantMap& eventData);
    /// Handle key up event to process key controls
    void HandleKeyUp(StringHash eventType, VariantMap& eventData);
    /// Handle a client touch
    void HandleTouchReaction(StringHash eventType, VariantMap& eventData);

    /// Button container element.
    SharedPtr<UIElement> buttonContainer_;
    /// Server address line editor element.
    SharedPtr<LineEdit> textEdit_;
    /// Connect button.
    SharedPtr<Button> connectButton_;
    /// Disconnect button.
    SharedPtr<Button> disconnectButton_;
    /// Start server button.
    SharedPtr<Button> startServerButton_;
    /// Scene.
    SharedPtr<Scene> scene_;
    /// Camera scene node.
    SharedPtr<Node> cameraNode_;

    struct ClientResources : public RefCounted
    {
        explicit ClientResources(Material * material)
            : material_(material)
        {}

        WeakPtr<Material> material_;
    };
    using ClientResourcesPtr = SharedPtr<ClientResources>;

    List<ClientResourcesPtr> freeResources_;
    HashMap<Connection*, ClientResourcesPtr> connectionResources_;

};
