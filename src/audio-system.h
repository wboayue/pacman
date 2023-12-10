#include <memory>

enum class Sound { kIntro };

class AudioSystem {
public:
  AudioSystem();
  void PlaySync(Sound sound);
  void PlayAsync(Sound sound);

private:
  bool enabled;
};