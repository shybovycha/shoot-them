#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "ActionHandler.h"
#include "QueueAction.h"

template<class State>
class StateManager
{
public:
    static StateManager* getInstance();

    void dispatch(QueueAction* action);

    std::shared_ptr<State> getState() const;

    void subscribe(ActionHandler* handler);

    void unsubscribe(ActionHandler* handler);

    void processActionQueue();

private:
    static StateManager* instance;

    std::shared_ptr<State> state;

    std::queue<QueueAction*> actionQueue;
    std::vector<ActionHandler*> subscribers;
};
