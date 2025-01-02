#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class Poison : public Bullet {
 public:
  Poison(GameCore *core,
         uint32_t id,
         uint32_t unit_id,
         uint32_t player_id,
         glm::vec2 position,
         float rotation,
         float damage_scale);
  float BasicDamage() const override;
  ~Poison() override;
  void Render() override;
  void Update() override;

 protected:
  float other_damage_scale_{0.9f};
  float duration_{2.0f};
};
}  // namespace battle_game::bullet
