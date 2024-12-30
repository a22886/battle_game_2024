#include "ice_maker.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t ice_maker_model_index = 0xffffffffu;
}  // namespace

IceMaker::IceMaker(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~ice_maker_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      ice_maker_model_index = mgr->RegisterModel(
          {
              {{0.7f, -0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.7f, -0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.0f, 1.2f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.7f, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.7f, 0.5f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.0f, -1.2f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.2f, 0.6f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
              {{-0.2f, 0.6f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
              {{0.0f, 0.8f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
              {{0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
          },
          {0, 1, 2, 3, 4, 5, 6, 8, 9, 7, 8, 9});
    }
  }
}

void IceMaker::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(ice_maker_model_index);
}

void IceMaker::Update() {
  IceMakerMove();
  Fire();
  CountDown();
}

void IceMaker::IceMakerMove() {
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

void IceMaker::Fire() {
  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        GenerateBullet<bullet::Ice>(position_ + Rotate({0.0f, 1.2f}, rotation_),
                                    rotation_, GetDamageScale());
        fire_count_down_ =
            FireInterval() * GetFireIntervalScale() * kTickPerSecond;
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
}

bool IceMaker::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return (17.0f * position.x - 7.0f * position.y < 8.4f &&
          17.0f * position.x + 7.0f * position.y > -8.4f &&
          position.y < 0.5f) ||
         (17.0f * position.x + 7.0f * position.y < 8.4f &&
          17.0f * position.x - 7.0f * position.y > -8.4f && position.y > -0.5f);
}

float IceMaker::Speed() const {
  return 2.5f;
}

float IceMaker::AngularSpeed() const {
  return glm::radians<float>(150.0f);
}

float IceMaker::BasicMaxHealth() const {
  return 80.0f;
}

float IceMaker::FireInterval() const {
  return 0.9f;
}

const char *IceMaker::UnitName() const {
  return "Ice Maker";
}

const char *IceMaker::Author() const {
  return "a22886";
}

const char *IceMaker::Brief() const {
  return "Can only fire bullets to the front.\nMove: WASD\nFire: click\nBullet "
         "default damage: 8.0\nBullet effect: slow down enemies 0.8x for "
         "2.0s\nFire interval: 0.9s";
}
}  // namespace battle_game::unit
