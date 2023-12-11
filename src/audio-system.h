#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <memory>
#include <string_view>

enum class Sound { kIntro, kMunch1, kMunch2, kPowerPellet };

class AudioSystem {
public:
  AudioSystem();
  ~AudioSystem();

  AudioSystem(const AudioSystem &) = delete;
  AudioSystem &operator=(const AudioSystem &) = delete;
  AudioSystem(const AudioSystem &&) = delete;
  AudioSystem &operator=(const AudioSystem &&) = delete;

  void PlaySync(Sound sound);
  void PlayAsync(Sound sound);
  void PlayAsync(Sound sound, int loop);

private:
  bool enabled_{false};
};

auto playSound(const std::string &sound) -> int;

#endif