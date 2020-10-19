#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "stdafx.h"

#include "Level.h"
#include "ResourceManager.h"
#include "Settings.h"

class ModernResourceManager : public ResourceManager {
public:
    ModernResourceManager();

    virtual std::shared_ptr<Settings> loadSettings();

    virtual std::vector<std::shared_ptr<Level>> loadLevels();
};
