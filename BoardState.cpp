#include "BoardState.h"
#include <Urho3D/Core/Context.h>

BoardState::BoardState(Context * context)
    : LogicComponent(context)
    , busy_(false)
{}

void BoardState::RegisterObject(Context * context)
{
    context->RegisterFactory<BoardState>();
}

void BoardState::SetBusy(bool busy)
{
    busy_ = busy;
}

bool BoardState::GetBusy() const
{
    return busy_;
}
