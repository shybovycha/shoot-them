#pragma once

#include "QueueAction.h"

class ActionHandler
{
public:
    virtual void handleAction(QueueAction* action) = 0;
};
