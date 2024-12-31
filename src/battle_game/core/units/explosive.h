#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Explosive : public Unit {
 public:
  Explosive(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  [[nodiscard]] float Speed() const override;
  [[nodiscard]] float BasicMaxHealth() const override;
  [[nodiscard]] float MaximumLife() const;
  [[nodiscard]] float MaximumDamage() const;

 protected:
  void ExplosiveMove();
  void Explode();
  void CheckExplode();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  [[nodiscard]] const char *Brief() const override;

  float damage_radius_{3.0f};
  int life_count_down_{1};
};
}  // namespace battle_game::unit
