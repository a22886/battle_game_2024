#include "battle_game/core/obstacles/trampoline.h"

#include "battle_game/core/game_core.h"

namespace battle_game::obstacle {

Trampoline::Trampoline(GameCore *game_core,
                       uint32_t id,
                       glm::vec2 position,
                       float rotation,
                       glm::vec2 scale,
                       float duration)
    : Obstacle(game_core, id, position, rotation),
      duration_(duration * kTickPerSecond) {
}

bool Trampoline::IsBlocked(glm::vec2 p) const {
  p = WorldToLocal(p);
  return p.x <= scale_.x && p.x >= -scale_.x && p.y <= scale_.y &&
         p.y >= -scale_.y;
}

std::pair<glm::vec2, glm::vec2> Trampoline::GetSurfaceNormal(
    glm::vec2 origin,
    glm::vec2 terminus) {
  origin = WorldToLocal(origin), terminus = WorldToLocal(terminus);
  glm::vec2 intersection{0.0f}, normal_vector{0.0f};
  if (terminus.x == 0) {
    if (terminus.y > 0) {
      intersection.x = origin.x, intersection.y = -scale_.y;
      normal_vector.y = -1.0f;
    } else if (terminus.y < 0) {
      intersection.x = origin.x, intersection.y = scale_.y;
      normal_vector.y = 1.0f;
    }
  } else if (terminus.y == 0) {
    if (terminus.x > 0) {
      intersection.x = -scale_.x, intersection.y = origin.y;
      normal_vector.x = -1.0f;
    } else if (terminus.x < 0) {
      intersection.x = scale_.x, intersection.y = origin.y;
      normal_vector.x = 1.0f;
    }
  } else {
    float sgnx = terminus.x < 0 ? -1.0f : 1.0f;
    float sgny = terminus.y < 0 ? -1.0f : 1.0f;
    float lx = (origin.x + sgnx * scale_.x) / terminus.x;
    float ly = (origin.y + sgny * scale_.y) / terminus.y;
    if (lx < ly) {
      intersection = origin - lx * terminus;
      normal_vector.x = -sgnx;
    } else if (ly < lx) {
      intersection = origin - ly * terminus;
      normal_vector.y = -sgny;
    } else {
      intersection = origin - lx * terminus;
      normal_vector = glm::normalize(scale_) * glm::vec2{-sgnx, -sgny};
    }
  }
  auto endp = LocalToWorld(intersection + normal_vector);
  intersection = LocalToWorld(intersection);
  normal_vector = endp - intersection;
  return std::pair<glm::vec2, glm::vec2>(intersection, normal_vector);
}

void Trampoline::Update() {
  if (!duration_)
    game_core_->PushEventRemoveObstacle(id_);
  else if (duration_ > 0)
    duration_--;
}

void Trampoline::Render() {
  battle_game::SetColor(glm::vec4{0.0f, 0.0f, 0.0f, 0.1f});
  battle_game::SetTexture(0);
  battle_game::SetTransformation(position_, rotation_, scale_);
  battle_game::DrawModel(0);
}
}  // namespace battle_game::obstacle
