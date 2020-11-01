#include "State.h"

void State::enqueue(QueueAction* action)
{
    actionQueue.push(action);
}

bool State::hasActions() const
{
    return !actionQueue.empty();
}

QueueAction* State::nextAction()
{
    if (actionQueue.empty())
    {
        return nullptr;
    }

    auto action = actionQueue.front();

    actionQueue.pop();

    return action;
}
