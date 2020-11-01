#include "ActionHandler.h"

ActionHandler::ActionHandler(std::shared_ptr<ActionDispatcher> actionDispatcher)
        : actionDispatcher(std::move(actionDispatcher))
{
}
