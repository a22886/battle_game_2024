#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "ice_maker.h"

namespace battle_game::unit {

namespace {
uint32_t shield_model_index = 0xffffffffu;
}  // namespace

Shield::Shield(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~shield_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      shield_model_index = mgr->RegisterModel(
          {
              {{0.5f, 0.7f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.5f, 0.7f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.6f, -0.3f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.6f, -0.3f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.0f, -0.7f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
          },
          {0, 1, 2, 0, 2, 3, 2, 3, 4});
    }
  }
}

void Shield::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(shield_model_index);
}

void Shield::Update() {
  Fire();
  ShieldMove();
  CountDown();
}

void Shield::ShieldMove() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player && fire_count_down_ < (FireInterval() * GetFireIntervalScale() -
                                    ShieldDuration()) *
                                       kTickPerSecond) {
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

void Shield::Fire() {
  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        game_core_->PushEventGenerateObstacle<obstacle::Trampoline>(
            position_, rotation_, glm::vec2{0.6f, 0.7f}, ShieldDuration());
        fire_count_down_ =
            FireInterval() * GetFireIntervalScale() * kTickPerSecond;
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
}

bool Shield::IsHit(glm::vec2 position) const {
  if (fire_count_down_ >=
      (FireInterval() * fire_interval_scale_ - ShieldDuration()) *
          kTickPerSecond)
    return false;
  position = WorldToLocal(position);
  return position.y < 0.7f && 10.0f * position.x - position.y > -5.7f &&
         10.0f * position.x + position.y < 5.7f &&
         2.0f * position.x + 3.0f * position.y > -2.1f &&
         2.0f * position.x - 3.0f * position.y < 2.1f;
}

float Shield::Speed() const {
  return 3.5f;
}

float Shield::AngularSpeed() const {
  return glm::radians<float>(210.0f);
}

float Shield::BasicMaxHealth() const {
  return 12500.0f;
}

float Shield::FireInterval() const {
  return 6.0f;
}

float Shield::ShieldDuration() const {
  return 3.0f;
}

const char *Shield::UnitName() const {
  return "Shield";
}

const char *Shield::Author() const {
  return "a22886";
}

const char *Shield::Brief() const {
  return "A defensive unit that doen't attack but makes shields that bounces "
         "bullets.\nMove: WASD\nMake shield: click\nShield duration: "
         "3.0s\nShield interval: 6.0s";
}
}  // namespace battle_game::unit
