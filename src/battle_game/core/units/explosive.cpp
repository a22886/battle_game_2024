#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "explosive.h"

namespace battle_game::unit {

namespace {
uint32_t explosive_model_index = 0xffffffffu;
}  // namespace

Explosive::Explosive(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~explosive_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      std::vector<ObjectVertex> vertices;
      std::vector<uint32_t> indices;
      const int precision = 60;
      const float inv_precision = 1.0f / float(precision);
      for (int i = 0; i < precision; i++) {
        auto theta = (float(i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f;
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        vertices.push_back({{sin_theta * 0.7f, cos_theta * 0.7f},
                            {0.0f, 0.0f},
                            {0.7f, 0.7f, 0.7f, 1.0f}});
        indices.push_back(i);
        indices.push_back((i + 1) % precision);
        indices.push_back(precision);
      }
      vertices.push_back(
          {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      for (int i = 0; i < 12; i++) {
        auto theta = (float(5 * i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f;
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        vertices.push_back({{sin_theta * 1.0f, cos_theta * 1.0f},
                            {0.0f, 0.0f},
                            {0.7f, 0.7f, 0.7f, 1.0f}});
        indices.push_back(5 * i + 1);
        indices.push_back((5 * i + precision - 1) % precision);
        indices.push_back(precision + 1 + i);
      }
      explosive_model_index = mgr->RegisterModel(vertices, indices);
    }
  }
  life_count_down_ = MaximumLife() * kTickPerSecond;
}

void Explosive::Render() {
  battle_game::SetTransformation(position_, 0);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(explosive_model_index);
}

void Explosive::Update() {
  game_core_->PushEventRotateUnit(id_, 0.0f);
  ExplosiveMove();
  CheckExplode();
  CountDown();
}

void Explosive::ExplosiveMove() {
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
    if (input_data.key_down[GLFW_KEY_A]) {
      offset.x -= 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      offset.x += 1.0f;
    }
    MoveAndRotate(offset, 0.0f);
  }
}

void Explosive::Explode() {
  game_core_->PushEventDealDamage(id_, id_, BasicMaxHealth());
  game_core_->PushEventGenerateParticle<particle::Disk>(
      position_, 0.0f, 4.5f, 0.5f, glm::vec4{1.0f, 0.0f, 0.0f, 0.5f}, 3.0f);
  auto &units = game_core_->GetUnits();
  for (auto &unit : units) {
    if (unit.first == id_)
      continue;
    float d = glm::length(unit.second->GetPosition() - position_);
    if (d < damage_radius_) {
      float scale = 1.0f;
      if (d > 0.5f)
        scale = 1.2f - 0.4f * d;
      game_core_->PushEventDealDamage(unit.first, id_, MaximumDamage() * scale);
    }
  }
}

void Explosive::CheckExplode() {
  if (life_count_down_ == 0) {
    Explode();
  } else {
    life_count_down_--;
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        Explode();
      }
    }
  }
}

bool Explosive::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}

float Explosive::Speed() const {
  return 5.0f;
}

float Explosive::BasicMaxHealth() const {
  return 50.0f;
}

float Explosive::MaximumLife() const {
  return 5.0f;
}

float Explosive::MaximumDamage() const {
  return 25.0f;
}

const char *Explosive::UnitName() const {
  return "Explosive";
}

const char *Explosive::Author() const {
  return "a22886";
}

const char *Explosive::Brief() const {
  return "A suicidal unit that explodes upon click or natural death.\nMove: "
         "WASD\nMake explode: click\nMaximum lifespan: 5.0s\nDamage radius: "
         "3.0\nMaximum damage: 25.0\nIt won't explode if enemies killed it "
         "before 5.0s.";
}
}  // namespace battle_game::unit
