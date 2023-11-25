#include "power-pellet.h"

PowerPellet::PowerPellet(SDL_Renderer *renderer)
    :sprite{renderer, "../assets/pacman.png", 4, 16}
{
  sprite = Sprite>(renderer, "../assets/pacman.png", 4, 16);
  sprite->SetFrames({1, 2});
}
