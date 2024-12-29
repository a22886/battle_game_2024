#include "battle_game/core/unit.h"

#include "battle_game/core/game_core.h"

namespace battle_game {

namespace {
uint32_t life_bar_model_index = 0xffffffffu;
}  // namespace

Unit::Unit(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Object(game_core, id) {
  player_id_ = player_id;
  lifebar_offset_ = {0.0f, 1.0f};
  background_lifebar_color_ = {1.0f, 0.0f, 0.0f, 0.9f};
  front_lifebar_color_ = {0.0f, 1.0f, 0.0f, 0.9f};
  fadeout_lifebar_color_ = {1.0f, 1.0f, 1.0f, 0.5f};
  fadeout_health_ = 1;
  if (!~life_bar_model_index) {
    auto mgr = AssetsManager::GetInstance();
    life_bar_model_index = mgr->RegisterModel(
        {{{-0.5f, 0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
         {{-0.5f, -0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
         {{0.5f, 0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
         {{0.5f, -0.08f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}},
        {0, 1, 2, 1, 2, 3});
  }
}

void Unit::SetPosition(glm::vec2 position) {
  if (!position_change_count_down_)
    position_ = position;
}

void Unit::SetRotation(float rotation) {
  if (!position_change_count_down_)
    rotation_ = rotation;
}

float Unit::Speed() const {
  return 3.0f;
}

float Unit::AngularSpeed() const {
  return glm::radians<float>(180.0f);
}

void Unit::ChangeSpeed(float speed_scale, float seconds) {
  if (GetSpeedScale() < 2.0f && GetSpeedScale() > 0.5f)
    GetSpeedScale() *= speed_scale;
  speed_change_count_down_ = seconds * kTickPerSecond;
}

float &Unit::GetSpeedScale() {
  return speed_scale_;
}

void Unit::ChangeDamage(float damage_scale, float seconds) {
  if (GetDamageScale() < 2.0f && GetDamageScale() > 0.5f)
    GetDamageScale() *= damage_scale;
  damage_change_count_down_ = seconds * kTickPerSecond;
}

float &Unit::GetDamageScale() {
  return damage_scale_;
}

float Unit::FireInterval() const {
  return 0.1f;
}

void Unit::ChangeFireInterval(float fire_interval_scale, float seconds) {
  if (GetFireIntervalScale() < 2.0f && GetFireIntervalScale() > 0.5f) {
    GetFireIntervalScale() *= fire_interval_scale;
    fire_count_down_ *= GetFireIntervalScale();
  }
  fire_interval_change_count_down_ = seconds * kTickPerSecond;
}

float &Unit::GetFireIntervalScale() {
  return fire_interval_scale_;
}

float Unit::BasicMaxHealth() const {
  return 100.0f;
}

float &Unit::GetHealthScale() {
  return health_scale_;
}

void Unit::ChangeHealth(float health_scale, float seconds) {
  if (GetHealthScale() < 2.0f && GetHealthScale() > 0.5f)
    GetHealthScale() *= health_scale;
  health_change_count_down_ = seconds * kTickPerSecond;
}

void Unit::CountDown() {
  if (speed_change_count_down_ == 0)
    speed_scale_ = 1.0f;
  else if (speed_change_count_down_ > 0)
    speed_change_count_down_--;

  if (damage_change_count_down_ == 0)
    damage_scale_ = 1.0f;
  else if (damage_change_count_down_ > 0)
    damage_change_count_down_--;

  if (health_change_count_down_ == 0)
    health_scale_ = 1.0f;
  else if (health_change_count_down_ > 0)
    health_change_count_down_--;

  if (fire_interval_change_count_down_ == 0) {
    fire_count_down_ /= fire_interval_scale_;
    fire_interval_scale_ = 1.0f;
  } else if (fire_interval_change_count_down_ > 0)
    fire_interval_change_count_down_--;

  if (position_change_count_down_ > 0)
    position_change_count_down_--;
}

void Unit::SetLifeBarLength(float new_length) {
  lifebar_length_ = std::min(new_length, 0.0f);
}
void Unit::SetLifeBarOffset(glm::vec2 new_offset) {
  lifebar_offset_ = new_offset;
}
void Unit::SetLifeBarFrontColor(glm::vec4 new_color) {
  front_lifebar_color_ = new_color;
}
void Unit::SetLifeBarBackgroundColor(glm::vec4 new_color) {
  background_lifebar_color_ = new_color;
}
void Unit::SetLifeBarFadeoutColor(glm::vec4 new_color) {
  fadeout_lifebar_color_ = new_color;
}
float Unit::GetLifeBarLength() {
  return lifebar_length_;
}
glm::vec2 Unit::GetLifeBarOffset() {
  return lifebar_offset_;
}
glm::vec4 Unit::GetLifeBarFrontColor() {
  return front_lifebar_color_;
}
glm::vec4 Unit::GetLifeBarBackgroundColor() {
  return background_lifebar_color_;
}
glm::vec4 Unit::GetLifeBarFadeoutColor() {
  return fadeout_lifebar_color_;
}

void Unit::ShowLifeBar() {
  lifebar_display_ = true;
}
void Unit::HideLifeBar() {
  lifebar_display_ = false;
}

void Unit::RenderLifeBar() {
  if (lifebar_display_) {
    auto parent_unit = game_core_->GetUnit(id_);
    auto pos = parent_unit->GetPosition() + lifebar_offset_;
    auto health = parent_unit->GetHealth();
    SetTransformation(pos, 0.0f, {lifebar_length_, 1.0f});
    SetColor(background_lifebar_color_);
    SetTexture(0);
    DrawModel(life_bar_model_index);
    glm::vec2 shift = {(float)lifebar_length_ * (1 - health) / 2, 0.0f};
    SetTransformation(pos - shift, 0.0f, {lifebar_length_ * health, 1.0f});
    SetColor(front_lifebar_color_);
    DrawModel(life_bar_model_index);
    if (std::fabs(health - fadeout_health_) >= 0.01f) {
      fadeout_health_ = health + (fadeout_health_ - health) * 0.93;
      shift = {lifebar_length_ * (health + fadeout_health_ - 1) / 2, 0.0f};
      SetTransformation(pos + shift, 0.0f,
                        {lifebar_length_ * (health - fadeout_health_), 1.0f});
      SetColor(fadeout_lifebar_color_);
      DrawModel(life_bar_model_index);
    } else {
      fadeout_health_ = health;
    }
  }
}

void Unit::RenderHelper() {
}

void Unit::MoveAndRotate(glm::vec2 offset, float rotation_offset) {
  if (offset != glm::vec2{0.0f, 0.0f}) {
    offset *= kSecondPerTick * Speed() * GetSpeedScale();

    auto new_position =
        position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                          glm::vec3{0.0f, 0.0f, 1.0f}) *
                              glm::vec4{offset, 0.0f, 0.0f}};
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }
  }
  if (rotation_offset != 0.0f) {
    rotation_offset *= kSecondPerTick * AngularSpeed() * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }
}

const char *Unit::UnitName() const {
  return "Unknown Unit";
}
const char *Unit::Author() const {
  return "Unknown Author";
}
const char *Unit::Brief() const {
  return "Unknown Features";
}
}  // namespace battle_game
