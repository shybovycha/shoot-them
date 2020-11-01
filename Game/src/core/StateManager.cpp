#include "StateManager.h"

template<class State>
StateManager<State>* StateManager<State>::getInstance()
{
    if (instance == nullptr)
    {
        instance = new StateManager();
    }

    return instance;
}

template<class State>
std::shared_ptr<State> StateManager<State>::getState() const
{
    return state;
}

template<class State>
void StateManager<State>::dispatch(QueueAction* action)
{
    actionQueue.push(action);
}

template<class State>
void StateManager<State>::subscribe(ActionHandler* handler)
{
    subscribers.push_back(handler);
}

template<class State>
void StateManager<State>::unsubscribe(ActionHandler* handler)
{
    auto existingSubscriber = std::find(subscribers.begin(), subscribers.end(), handler);

    if (existingSubscriber == subscribers.end())
    {
        return;
    }

    subscribers.erase(existingSubscriber);
}

template<class State>
void StateManager<State>::processActionQueue()
{
    while (!actionQueue.empty())
    {
        auto action = actionQueue.front();

        for (auto subscriber : subscribers)
        {
            subscriber->handleAction(action);
        }

        actionQueue.pop();
    }
}
