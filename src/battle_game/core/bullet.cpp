#include "battle_game/core/bullet.h"

#include "battle_game/core/game_core.h"
namespace battle_game {
Bullet::Bullet(GameCore *core,
               uint32_t id,
               uint32_t unit_id,
               uint32_t player_id,
               glm::vec2 position,
               float rotation,
               float damage_scale)
    : Object(core, id, position, rotation),
      unit_id_(unit_id),
      player_id_(player_id),
      damage_scale_(damage_scale) {
  velocity_ = Rotate(glm::vec2{0.0f, Speed()}, rotation);
};

void Bullet::Bounce(std::pair<glm::vec2, glm::vec2> normal_vector) {
  auto x = normal_vector.first, n = normal_vector.second;
  velocity_ -= 2.0f * (glm::dot<2, float>(velocity_, n)) * n;
  position_ -= 2.0f * (glm::dot<2, float>(position_ - x, n)) * n;
  if (velocity_.x == 0.0f)
    rotation_ = glm::pi<float>();
  else if (velocity_.x > 0.0f) {
    rotation_ = -glm::acos(glm::normalize(velocity_).y);
  } else {
    rotation_ = glm::acos(glm::normalize(velocity_).y);
  }
}

float Bullet::BasicDamage() const {
  return 10.0f;
}
float Bullet::Speed() const {
  return 20.0f;
}

Bullet::~Bullet() = default;
}  // namespace battle_game
