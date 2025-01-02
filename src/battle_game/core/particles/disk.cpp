#include "battle_game/core/particles/disk.h"

#include "battle_game/core/game_core.h"

namespace battle_game::particle {
Disk::Disk(GameCore *game_core,
           uint32_t id,
           glm::vec2 position,
           float rotation,
           float v,
           float radius,
           glm::vec4 color,
           float decay_scale)
    : Particle(game_core, id, position, rotation),
      v_(v),
      radius_(radius),
      color_(color),
      decay_scale_(decay_scale) {
}

void Disk::Render() {
  SetTransformation(position_, rotation_, glm::vec2{radius_});
  SetColor(glm::vec4{glm::vec3{1.0f}, strength_} * color_);
  SetTexture(BATTLE_GAME_ASSETS_DIR "textures/particle3.png");
  DrawModel();
}

void Disk::Update() {
  radius_ += v_ * kSecondPerTick;
  strength_ -= kSecondPerTick * decay_scale_;
  if (strength_ < 0.0f) {
    game_core_->PushEventRemoveParticle(id_);
  }
}
}  // namespace battle_game::particle
