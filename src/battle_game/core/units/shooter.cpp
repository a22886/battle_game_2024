#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "tiny_tank.h"

namespace battle_game::unit {

namespace {
uint32_t shooter_model_index = 0xffffffffu;
}  // namespace

Shooter::Shooter(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~shooter_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Shooter Body */
      shooter_model_index = mgr->RegisterModel(
          {
              {{0.6f, -0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.6f, -0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.0f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
          },
          {0, 1, 2});
    }
  }
}

void Shooter::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(shooter_model_index);
}

void Shooter::Update() {
  ShooterMove();
  Fire();
  CountDown();
}

void Shooter::ShooterMove() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.y -= 1.0f;
    }
    float rotation_offset = 0.0f;
    if (input_data.key_down[GLFW_KEY_A]) {
      rotation_offset += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      rotation_offset -= 1.0f;
    }
    MoveAndRotate(offset, rotation_offset);
  }
}

void Shooter::Fire() {
  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        GenerateBullet<bullet::Laser>(
            position_ + Rotate({0.0f, 0.8f}, rotation_), rotation_,
            GetDamageScale());
        fire_count_down_ =
            FireInterval() * GetFireIntervalScale() * kTickPerSecond;
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
}

bool Shooter::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.y > -0.8f && 8.0f * position.x - 3.0f * position.y > -2.4f &&
         8.0f * position.x + 3.0f * position.y < 2.4f;
}

float Shooter::BasicMaxHealth() const {
  return 80.0f;
}

float Shooter::FireInterval() const {
  return 0.7f;
}

float Shooter::Speed() const {
  return 4.0f;
}
float Shooter::AngularSpeed() const {
  return glm::radians<float>(90.0f);
}

const char *Shooter::UnitName() const {
  return "Shooter";
}

const char *Shooter::Author() const {
  return "a22886";
}
}  // namespace battle_game::unit
