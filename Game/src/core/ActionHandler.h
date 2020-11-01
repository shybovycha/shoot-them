#pragma once

#include <memory>

#include "ActionDispatcher.h"
#include "QueueAction.h"
#include "State.h"

class ActionHandler
{
public:
    ActionHandler(std::shared_ptr<ActionDispatcher> actionDispatcher);

    virtual void handle(QueueAction* action, std::shared_ptr<State> state) = 0;

protected:
    std::shared_ptr<ActionDispatcher> actionDispatcher;
};
