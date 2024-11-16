#include "board-manager.h"
#include "constants.h"


BoardManager::BoardManager(SDL_Renderer *renderer)
    : maze{renderer, "../assets/maze.png"}, pacman{renderer, "../assets/pacman.png"},
      fruits{renderer, "../assets/fruits.png"}, text{renderer, "../assets/white-text.png"}
      {}

void BoardManager::Update(const float deltaTime, GameContext &context) {
  maze.Update(deltaTime);

  score = std::to_string(context.score);
  extraLives = context.extraLives;
  level = context.level;
}

void BoardManager::Render(SDL_Renderer *renderer) {
  // Maze
  maze.Render(renderer, {0, 0});

  RenderExtraLives(renderer);

  RenderFruits(renderer);

  // Display score
  static constexpr auto kHighScoreCell = Vec2{9, 0};
  static constexpr auto k1UpCell = Vec2{3, 0};
  static constexpr auto kScoreCell = Vec2{4, 1};

  WriteText(renderer, kHighScoreCell, "HIGH SCORE");
  WriteText(renderer, k1UpCell, "1UP");
  WriteText(renderer, kScoreCell, score);
}

void BoardManager::RenderExtraLives(SDL_Renderer *renderer) {
  static constexpr int kLifeSize = 16;
  static constexpr int kSpriteFrame = 4;
  static constexpr Vec2 kLifeCell{4, 34};

  SDL_Rect source;
  source.w = kLifeSize;
  source.h = kLifeSize;
  source.x = kSpriteFrame * kLifeSize;
  source.y = 0;

  for (auto i = 0; i < extraLives; ++i) {
    SDL_Rect destination;

    destination.w = kLifeSize;
    destination.h = kLifeSize;
    destination.x = static_cast<int>(kLifeCell.x) * kCellSize - (i * kLifeSize);
    destination.y = static_cast<int>(kLifeCell.y) * kCellSize;

    pacman.Render(renderer, source, destination);
  }
}

void BoardManager::RenderFruits(SDL_Renderer *renderer) {
  static constexpr int kFruitSize = 16;
  static constexpr Vec2 kFruitCell{24, 34};

  SDL_Rect source;
  source.w = kFruitSize;
  source.h = kFruitSize;
  source.x = level * kFruitSize;
  source.y = 0;

  SDL_Rect destination;

  destination.w = kFruitSize;
  destination.h = kFruitSize;
  destination.x = kFruitCell.x * kCellSize;
  destination.y = kFruitCell.y * kCellSize;

  fruits.Render(renderer, source, destination);
}

void BoardManager::WriteText(SDL_Renderer *renderer, Vec2 position, const std::string &text) {
  static constexpr int kLetterOffset = 1;
  static constexpr int kDigitOffset = 31;

  SDL_Rect source{0, 0, kCellSize, kCellSize};
  SDL_Rect destination{0 , 0, kCellSize, kCellSize};

  for (size_t i = 0; i < text.size(); ++i) {
    int ch = std::toupper(text[i]);

    if (std::isalpha(ch)) {
      source.x = (kLetterOffset + ch - 'A') * kCellSize;
      source.y = 0;
    } else if (std::isdigit(ch)) {
      source.x = (kDigitOffset + ch - '0') * kCellSize;
      source.y = 0;
    } else {
      source.x = 0;
      source.y = 0;
    }

    destination.x = static_cast<int>(position.x) * kCellSize + static_cast<int>(i) * kCellSize;
    destination.y = static_cast<int>(position.y) * kCellSize;

    this->text.Render(renderer, source, destination);
  }
}
