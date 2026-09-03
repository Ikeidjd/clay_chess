#include "sounds.h"

struct Sounds sounds;

// I was going to use .ogg files, but they would inconsistently load with weird artifacts
// This does not seem to be the case for .mp3
#define LOAD(name) LoadSound("res/sounds/" name ".mp3")

void sounds_init(void) {
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(4096);

    sounds.move = LOAD("move");
    sounds.capture = LOAD("capture");
    sounds.castle = LOAD("castle");
    sounds.check = LOAD("check");
}

void sounds_destroy(void) {
    UnloadSound(sounds.move);
    UnloadSound(sounds.capture);
    UnloadSound(sounds.castle);
    UnloadSound(sounds.check);

    CloseAudioDevice();
}
