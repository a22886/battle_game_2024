#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class Ice : public Bullet {
 public:
  Ice(GameCore *core,
      uint32_t id,
      uint32_t unit_id,
      uint32_t player_id,
      glm::vec2 position,
      float rotation,
      float damage_scale);
  float BasicDamage() const override;
  float Speed() const override;
  ~Ice() override;
  void Render() override;
  void Update() override;

 protected:
  float speed_scale_{0.8f};
  float duration_{2.0f};
};
}  // namespace battle_game::bullet
