#include "battle_game/core/bullets/ice.h"
#include "battle_game/core/game_core.h"

namespace battle_game::bullet {
Ice::Ice(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale) {
  velocity_ = Rotate(glm::vec2{0.0f, Speed()}, rotation);
}

void Ice::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.1f, 0.2f});
  SetColor(0.5f * game_core_->GetPlayerColor(player_id_) +
           glm::vec4{0.0f, 0.0f, 0.3f, 0.5f});
  SetTexture(BATTLE_GAME_ASSETS_DIR "textures/particle5.png");
  DrawModel(0);
}

void Ice::Update() {
  position_ += velocity_ * kSecondPerTick;
  bool should_die = false;
  if (game_core_->IsBlockedByObstacles(position_)) {
    auto obstacle = game_core_->GetBlockedObstacle(position_);
    if (obstacle) {
      auto normal_vector = obstacle->GetSurfaceNormal(position_, velocity_);
      if (normal_vector != std::pair<glm::vec2, glm::vec2>{
                               glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f}})
        game_core_->PushEventBounceBullet(id_, normal_vector);
      else
        should_die = true;
    }
    else
      should_die = true;
  }

  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == unit_id_) {
      continue;
    }
    if (unit.second->IsHit(position_)) {
      game_core_->PushEventDealDamage(unit.first, id_,
                                      damage_scale_ * BasicDamage());
      game_core_->PushEventChangeSpeedScale(unit.first, speed_scale_, duration_);
      game_core_->PushEventChangeFireIntervalScale(
          unit.first, 1.0f / speed_scale_, duration_);
      should_die = true;
    }
  }

  if (should_die) {
    game_core_->PushEventRemoveBullet(id_);
  }
}

float Ice::BasicDamage() const {
  return 8.0f;
}

float Ice::Speed() const {
  return 18.0f;
}

Ice::~Ice() {
}
}  // namespace battle_game::bullet
