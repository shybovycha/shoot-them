#include "IrrKlangSoundEngine.h"

IrrKlangSoundEngine::IrrKlangSoundEngine() : SoundEngine()
{
    soundEngine = irrklang::createIrrKlangDevice();
}

void IrrKlangSoundEngine::playSound(const std::string& soundFilename)
{
    soundEngine->play2D(soundFilename.c_str());
}
