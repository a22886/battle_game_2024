#include "vial.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t vial_model_index = 0xffffffffu;
uint32_t poison_model_index[10] = {
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu};
}  // namespace

Vial::Vial(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~vial_model_index) {
    std::vector<ObjectVertex> vertices = {
        {{0.4f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 0.5f}},
        {{-0.4f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 0.5f}},
        {{0.4f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 0.5f}},
        {{-0.4f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 0.5f}},
        {{0.6f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 0.5f}},
        {{-0.6f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 0.5f}},
        {{0.6f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 0.5f}},
        {{-0.6f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 0.5f}},
    };
    std::vector<uint32_t> indices = {0, 1, 2, 1, 2, 3, 4, 5, 6, 5, 6, 7};

    std::vector<ObjectVertex> poison_vertices[10];
    std::vector<uint32_t> poison_indices = {0, 1, 2, 1, 2, 3};
    for (int i = 0; i < 10; i++) {
      poison_vertices[i] = {
          {{0.6f, -1.0f + 0.18f * i}, {0.0f, 0.0f}, {0.5f, 1.0f, 0.5f, 1.0f}},
          {{-0.6f, -1.0f + 0.18f * i}, {0.0f, 0.0f}, {0.5f, 1.0f, 0.5f, 1.0f}},
          {{0.6f, -0.82f + 0.18f * i}, {0.0f, 0.0f}, {0.5f, 1.0f, 0.5f, 1.0f}},
          {{-0.6f, -0.82f + 0.18f * i}, {0.0f, 0.0f}, {0.5f, 1.0f, 0.5f, 1.0f}},
      };
    }
    auto mgr = AssetsManager::GetInstance();
    {
      vial_model_index = mgr->RegisterModel(vertices, indices);
      for (int i = 0; i < 10; i++)
        poison_model_index[i] =
            mgr->RegisterModel(poison_vertices[i], poison_indices);
    }
  }
}

void Vial::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(vial_model_index);
  for (int i = 0; 2 * i < poison_; i++)
    battle_game::DrawModel(poison_model_index[i]);
}

void Vial::Update() {
  VialMove();
  Fire();
  Shoot();
  CountDown();
  AddPoison();
}

void Vial::VialMove() {
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

void Vial::Fire() {
  if (fire_count_down_ == 0 && CtrlX_) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      bool Ctrl = input_data.key_down[GLFW_KEY_LEFT_CONTROL] ||
                  input_data.key_down[GLFW_KEY_RIGHT_CONTROL];
      if (Ctrl && input_data.key_down[GLFW_KEY_V] && poison_ >= 10) {
        if (!game_core_->IsOutOfRange(input_data.mouse_cursor_position)) {
          game_core_->PushEventMoveUnit(id_, input_data.mouse_cursor_position);
          fire_count_down_ =
              FireInterval() * GetFireIntervalScale() * kTickPerSecond;
          CtrlX_ = 0;
          poison_ -= 10;
        }
      }
    }
  }
  if (fire_count_down_)
    fire_count_down_--;
  auto player = game_core_->GetPlayer(player_id_);
  if (player && !CtrlX_) {
    auto &input_data = player->GetInputData();
    bool Ctrl = input_data.key_down[GLFW_KEY_LEFT_CONTROL] ||
                input_data.key_down[GLFW_KEY_RIGHT_CONTROL];
    if (Ctrl && input_data.key_down[GLFW_KEY_X]) {
      CtrlX_ = 1;
    }
  }
}

void Vial::Shoot() {
  if (poison_) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_clicked[GLFW_MOUSE_BUTTON_LEFT]) {
        GenerateBullet<bullet::Poison>(
            position_ + Rotate({0.0f, 1.0f}, rotation_), rotation_,
            GetDamageScale());
        poison_--;
      }
    }
  }
}

void Vial::AddPoison() {
  if (poison_ == 20)
    add_poison_count_down_ =
        AddPoisonInterval() * GetFireIntervalScale() * kTickPerSecond;
  if (add_poison_count_down_ == 0) {
    if (poison_ < 20) {
      poison_++;
      add_poison_count_down_ =
          AddPoisonInterval() * GetFireIntervalScale() * kTickPerSecond;
    }
  }
  if (add_poison_count_down_)
    add_poison_count_down_--;
}

bool Vial::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  if (position.y > -1.0f && position.y < 0.8f)
    return std::abs(position.x) < 0.6f;
  else if (position.y >= 0.8f && position.y < 1.0f)
    return std::abs(position.x) < 0.4f;
  else
    return false;
}

float Vial::Speed() const {
  return 2.0f;
}

float Vial::BasicMaxHealth() const {
  return 70.0f;
}

float Vial::FireInterval() const {
  return 5.0f;
}

float Vial::AddPoisonInterval() const {
  return 1.0f;
}

const char *Vial::UnitName() const {
  return "Vial";
}

const char *Vial::Author() const {
  return "a22886";
}

const char *Vial::Brief() const {
  return "A unit that can make and store bullets.\nMove: WASD\nFire: "
         "click\nBullet default damage: 7.0\nBullet effect: lower all dagames "
         "to 0.9x for 2.0s\nCapacity: 20 bullets\nInstant Move: Ctrl+X, "
         "Ctrl+V\nInstant move interval: 5.0s\nInstant moving requires 10 "
         "bullets.";
}
}  // namespace battle_game::unit
