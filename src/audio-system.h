#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>

/**
 * @brief Enumeration of available sound effects in the game.
 */
enum class Sound {
  kIntro,       ///< Game intro/start sound
  kMunch1,      ///< Pacman eating pellet sound
  kMunch2,      ///< Alternative pellet eating sound
  kPowerPellet, ///< Power pellet consumed sound
  kDeath        ///< Pacman death sound
};

/**
 * @brief Manages the game's audio system including sound effect playback.
 *
 * The AudioSystem class handles asynchronous sound playback using a dedicated audio thread
 * and a queue-based system for processing sound requests. It ensures thread-safe operation
 * and proper resource management.
 */
class AudioSystem {
public:
  /**
   * @brief Initializes the audio system.
   *
   * Sets up SDL audio subsystem and SDL_mixer with default parameters.
   * Starts the audio processing thread.
   */
  AudioSystem();

  /**
   * @brief Cleans up audio system resources.
   *
   * Ensures proper shutdown of the audio thread, SDL_mixer, and SDL audio subsystem.
   */
  ~AudioSystem();

  // Delete copy and move operations to ensure single instance
  AudioSystem(const AudioSystem &) = delete;
  AudioSystem(const AudioSystem &&) = delete;
  AudioSystem &operator=(const AudioSystem &) = delete;
  AudioSystem &operator=(const AudioSystem &&) = delete;

  /**
   * @brief Queues a sound for playback.
   *
   * @param sound The sound effect to play
   * @param loop Optional number of times to loop the sound (-1 for infinite)
   * @return std::future<void> Future object for tracking sound completion
   */
  auto PlaySound(Sound sound, std::optional<int> loop) -> std::future<void>;

private:
  /**
   * @brief Internal structure representing a sound playback request.
   */
  struct AudioRequest {
    Sound sound;                                    ///< Sound to play
    std::optional<int> loop;                        ///< Loop count
    std::shared_ptr<std::promise<void>> completion; ///< Completion notification
  };

  /**
   * @brief Processes the audio request queue.
   *
   * Main function for the audio thread. Continuously monitors the queue
   * for new requests and plays sounds as they arrive.
   */
  auto processAudioQueue() -> void;

  bool initialized_{false};                   ///< Audio system initialization state
  bool running_{true};                        ///< Audio thread running state
  std::thread audioThread_;                   ///< Audio processing thread
  std::queue<AudioRequest> audioQueue_;       ///< Pending audio requests
  std::mutex queueMutex_;                     ///< Queue access protection
  std::condition_variable conditionVariable_; ///< New request notification
};

#endif