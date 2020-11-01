#include "ActionDispatcher.h"

ActionDispatcher::ActionDispatcher(std::shared_ptr<State> _state) : state(std::move(_state))
{
}

void ActionDispatcher::dispatch(QueueAction* action)
{
    state->enqueue(action);
}
