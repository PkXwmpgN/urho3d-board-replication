#ifndef _BOARD_STATE_H_INCLUDED__
#define _BOARD_STATE_H_INCLUDED__

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

namespace Urho3D
{
    class Context;
}

class BoardState : public LogicComponent
{
    URHO3D_OBJECT(BoardState, LogicComponent);

public:

    explicit BoardState(Context * context);
    static void RegisterObject(Context * context);

    void SetBusy(bool busy);
    bool GetBusy() const;

private:

    bool busy_;
};

#endif // _BOARD_STATE_H_INCLUDED__
