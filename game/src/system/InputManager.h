#pragma once

#include <fmt/core.h>
#include <iostream>

#include "../math/Vector2.h"

class InputManager
{
public:
    Vector2 getMousePosition() const
    {
        std::cout << fmt::format("got mouse position\n");
        return Vector2();
    }

    bool isMouseButtonPressed(int button) const
    {
        std::cout << fmt::format("checking mouse button {}\n", button);
        return false;
    }
};
