#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Vial : public Unit {
 public:
  Vial(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  [[nodiscard]] float Speed() const override;
  [[nodiscard]] float FireInterval() const override;
  [[nodiscard]] float BasicMaxHealth() const override;
  [[nodiscard]] float AddPoisonInterval() const;

 protected:
  void VialMove();
  // uses Fire() to represent the action of instant moving.
  void Fire();
  void Shoot();
  void AddPoison();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;
  [[nodiscard]] const char *Brief() const override;

  int poison_{20};
  bool CtrlX_{0};
  int add_poison_count_down_;
};
}  // namespace battle_game::unit
