#include "Sound.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <AudioToolbox/AudioServices.h>
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace {

void ring_terminal_bell(int count)
{
    for (int i = 0; i < count; ++i) {
        std::cout << '\a' << std::flush;
    }
}

#ifdef _WIN32
bool play_windows_tone(DWORD frequency, DWORD duration)
{
    return Beep(frequency, duration) != 0;
}
#endif

#ifdef __APPLE__
SystemSoundID load_system_sound(const char* absolute_path)
{
    CFStringRef path_ref =
        CFStringCreateWithCString(nullptr, absolute_path, kCFStringEncodingUTF8);
    if (!path_ref) {
        return 0;
    }

    CFURLRef url_ref =
        CFURLCreateWithFileSystemPath(nullptr, path_ref, kCFURLPOSIXPathStyle, false);
    CFRelease(path_ref);

    if (!url_ref) {
        return 0;
    }

    SystemSoundID sound_id = 0;
    const OSStatus status = AudioServicesCreateSystemSoundID(url_ref, &sound_id);
    CFRelease(url_ref);

    if (status != noErr) {
        return 0;
    }

    return sound_id;
}

SystemSoundID mac_sound_id(SoundCue cue)
{
    static const SystemSoundID error_sound =
        load_system_sound("/System/Library/Sounds/Basso.aiff");
    static const SystemSoundID magic_sound =
        load_system_sound("/System/Library/Sounds/Glass.aiff");
    static const SystemSoundID menu_sound =
        load_system_sound("/System/Library/Sounds/Pop.aiff");
    static const SystemSoundID attack_sound =
        load_system_sound("/System/Library/Sounds/Tink.aiff");
    static const SystemSoundID end_sound =
        load_system_sound("/System/Library/Sounds/Hero.aiff");

    switch (cue) {
    case SoundCue::Error:
        return error_sound;
    case SoundCue::Magic:
        return magic_sound;
    case SoundCue::Menu:
        return menu_sound;
    case SoundCue::Attack:
        return attack_sound;
    case SoundCue::End:
        return end_sound;
    }

    return 0;
}
#endif

} // namespace

void PlaySoundCue(SoundCue cue)
{
#ifdef _WIN32
    switch (cue) {
    case SoundCue::Error:
        if (play_windows_tone(180, 120) && play_windows_tone(120, 180)) {
            return;
        }
        ring_terminal_bell(2);
        return;
    case SoundCue::Magic:
        if (play_windows_tone(523, 60) &&
            play_windows_tone(659, 60) &&
            play_windows_tone(784, 90)) {
            return;
        }
        ring_terminal_bell(1);
        return;
    case SoundCue::Menu:
        if (play_windows_tone(784, 35)) {
            return;
        }
        ring_terminal_bell(1);
        return;
    case SoundCue::Attack:
        if (play_windows_tone(220, 40) && play_windows_tone(180, 40)) {
            return;
        }
        ring_terminal_bell(1);
        return;
    case SoundCue::End:
        if (play_windows_tone(784, 60) &&
            play_windows_tone(523, 80) &&
            play_windows_tone(392, 140)) {
            return;
        }
        ring_terminal_bell(1);
        return;
    }
#elif defined(__APPLE__)
    const SystemSoundID sound_id = mac_sound_id(cue);
    if (sound_id != 0) {
        AudioServicesPlaySystemSound(sound_id);
        return;
    }

    ring_terminal_bell(1);
#else
    switch (cue) {
    case SoundCue::Error:
        ring_terminal_bell(2);
        return;
    case SoundCue::Magic:
    case SoundCue::Menu:
    case SoundCue::Attack:
    case SoundCue::End:
        ring_terminal_bell(1);
        return;
    }
#endif
}
