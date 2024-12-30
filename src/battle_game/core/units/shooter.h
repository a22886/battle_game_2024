#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Shooter : public Unit {
 public:
  Shooter(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  [[nodiscard]] float BasicMaxHealth() const override;
  [[nodiscard]] float FireInterval() const override;
  [[nodiscard]] float Speed() const override;
  [[nodiscard]] float AngularSpeed() const override;

 protected:
  void ShooterMove();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  [[nodiscard]] const char *Brief() const override;
};
}  // namespace battle_game::unit
