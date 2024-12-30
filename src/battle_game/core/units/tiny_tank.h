#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Tank : public Unit {
 public:
  Tank(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void TankMove();
  void TurretRotate();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  [[nodiscard]] const char *Brief() const override;

  float turret_rotation_{0.0f};
};
}  // namespace battle_game::unit
