#pragma once

#include <queue>

#include "ActionDispatcher.h"
#include "QueueAction.h"

class State
{
public:
    State() = default;

    bool hasActions() const;

    QueueAction* nextAction();

private:
    friend class ActionDispatcher;

    void enqueue(QueueAction* action);

    std::queue<QueueAction*> actionQueue;
};
