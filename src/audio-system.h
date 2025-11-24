#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <optional>
#include <queue>
#include <unordered_map>

#include "asset-manager.h"

/// Unique identifier for tracking individual sounds
using SoundHandle = uint64_t;

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
   * @param assetManager Reference to the asset manager for loading sounds
   */
  explicit AudioSystem(AssetManager &assetManager);

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
   * @return std::pair<SoundHandle, std::future<void>> Handle for cancellation and future for completion
   */
  auto PlaySound(Sounds sound, std::optional<int> loop) -> std::pair<SoundHandle, std::future<void>>;

  /**
   * @brief Cancels a specific playing sound by its handle.
   *
   * @param handle The unique identifier of the sound to cancel
   */
  auto CancelSound(SoundHandle handle) -> void;

  /**
   * @brief Cancels all currently playing sounds.
   */
  auto CancelAllSounds() -> void;

private:
  /**
   * @brief Internal structure representing a sound playback request.
   */
  struct AudioRequest {
    Sounds sound;                                    ///< Sound to play
    std::optional<int> loop;                        ///< Loop count
    SoundHandle handle;                             ///< Unique handle for this sound
    std::shared_ptr<std::promise<void>> completion; ///< Completion notification
  };

  /**
   * @brief Internal structure tracking an actively playing sound.
   */
  struct ActiveSound {
    int channel;                                    ///< SDL_mixer channel number
    Sounds sound;                                    ///< Sound type
    SoundHandle handle;                             ///< Unique handle
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

  /**
   * @brief Cleans up finished sounds from the active sound tracking.
   *
   * Checks all active sounds and removes those that have finished playing.
   */
  auto cleanupFinishedSounds() -> void;

  /**
   * @brief SDL_mixer callback when a channel finishes playing.
   */
  static auto channelFinishedCallback(int channel) -> void;

  /**
   * @brief Removes an active sound from all tracking maps and signals completion.
   *
   * Must be called while holding activeSoundsMutex_.
   * @param handle The sound handle to remove
   */
  auto removeActiveSoundLocked(SoundHandle handle) -> void;

  bool initialized_{false};                   ///< Audio system initialization state
  bool running_{true};                        ///< Audio thread running state
  std::thread audioThread_;                   ///< Audio processing thread
  std::queue<AudioRequest> audioQueue_;       ///< Pending audio requests
  std::mutex queueMutex_;                     ///< Queue access protection
  std::condition_variable conditionVariable_; ///< New request notification
  AssetManager &assetManager_;                ///< Reference to asset manager

  // Sound tracking for cancellation support
  std::atomic<SoundHandle> nextHandle_{1};                       ///< Counter for generating unique handles
  std::unordered_map<SoundHandle, ActiveSound> activeSounds_;    ///< Map of handle to active sound info
  std::unordered_map<int, SoundHandle> channelToHandle_;         ///< Map of channel to sound handle
  std::unordered_map<Sounds, SoundHandle> soundTypeToHandle_;     ///< Map of sound type to active handle (for exclusive sounds)
  std::mutex activeSoundsMutex_;                                 ///< Protection for active sounds maps

  static AudioSystem* instance_;                                 ///< Singleton instance for callbacks
};

#endif