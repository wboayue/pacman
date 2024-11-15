#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>

enum class Sound { kIntro, kMunch1, kMunch2, kPowerPellet, kDeath };

class AudioSystem {
public:
  AudioSystem();
  ~AudioSystem();

  AudioSystem(const AudioSystem &) = delete;
  AudioSystem(const AudioSystem &&) = delete;
  AudioSystem &operator=(const AudioSystem &) = delete;
  AudioSystem &operator=(const AudioSystem &&) = delete;

  auto PlaySound(Sound sound, std::optional<int> loop) -> std::future<void>;

private:
  struct AudioRequest {
    Sound sound;
    std::optional<int> loop;
    std::shared_ptr<std::promise<void>> completion;
  };

  auto processAudioQueue() -> void;

  bool initialized_{false};
  bool running_{true};
  std::thread audioThread_;
  std::queue<AudioRequest> audioQueue_;
  std::mutex queueMutex_;
  std::condition_variable conditionVariable_;
};

#endif