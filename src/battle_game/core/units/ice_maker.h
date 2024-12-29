#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class IceMaker : public Unit {
 public:
  IceMaker(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  [[nodiscard]] float Speed() const override;
  [[nodiscard]] float AngularSpeed() const override;
  [[nodiscard]] float FireInterval() const override;
  [[nodiscard]] float BasicMaxHealth() const override;

 protected:
  void IceMakerMove();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

};
}  // namespace battle_game::unit
