#pragma once

#include <memory>
#include <utility>

#include "QueueAction.h"
#include "State.h"

class ActionDispatcher
{
public:
    ActionDispatcher(std::shared_ptr<State> state);

protected:
    void dispatch(QueueAction* action);

    std::shared_ptr<State> state;
};
