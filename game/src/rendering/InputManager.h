#pragma once

#include <format>
#include <iostream>

#include "../math/Vector2.h"

class InputManager
{
public:
    Vector2 getMousePosition() const
    {
        std::cout << std::format("got mouse position\n");
        return Vector2();
    }

    bool isMouseButtonPressed(int button) const
    {
        std::cout << std::format("checking mouse button {}\n", button);
        return false;
    }
};
