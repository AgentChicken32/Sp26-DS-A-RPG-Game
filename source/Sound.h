#pragma once

enum class SoundCue {
    Error,
    Magic,
    Menu,
    Attack,
    End
};

void PlaySoundCue(SoundCue cue);
