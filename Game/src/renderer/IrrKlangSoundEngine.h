#pragma once

#include <irrKlang.h>

#include "../core/SoundEngine.h"

class IrrKlangSoundEngine : public SoundEngine
{
public:
    IrrKlangSoundEngine();

    virtual void playSound(const std::string &soundFilename) override;

private:
    irrklang::ISoundEngine* soundEngine = 0;
};
