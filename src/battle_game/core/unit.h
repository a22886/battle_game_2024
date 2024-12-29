#pragma once
#include "battle_game/core/object.h"
#include "glm/glm.hpp"

namespace battle_game {

class Bullet;

class Unit : public Object {
 public:
  Unit(GameCore *game_core, uint32_t id, uint32_t player_id);

  uint32_t &GetPlayerId() {
    return player_id_;
  }
  [[nodiscard]] uint32_t GetPlayerId() const {
    return player_id_;
  }
  void SetPosition(glm::vec2 position);
  void SetRotation(float rotation);

  [[nodiscard]] float &GetDamageScale();
  void ChangeDamage(float damage_scale, float seconds);

  [[nodiscard]] virtual float Speed() const;
  [[nodiscard]] virtual float AngularSpeed() const;
  [[nodiscard]] float &GetSpeedScale();
  void ChangeSpeed(float speed_scale, float seconds);

  [[nodiscard]] virtual float FireInterval() const;
  [[nodiscard]] float &GetFireIntervalScale();
  void ChangeFireInterval(float fire_interval_scale, float seconds);

  [[nodiscard]] virtual float BasicMaxHealth() const;
  [[nodiscard]] float &GetHealthScale();
  void ChangeHealth(float health_scale, float seconds);
  [[nodiscard]] float GetMaxHealth() {
    return std::max(GetHealthScale() * BasicMaxHealth(), 1.0f);
  }

  /*
   * Health value is in range [0, 1], represents the remaining health in ratio
   * form. GetHealth() * GetMaxHealth() represent true remaining health of the
   * unit.
   * */
  [[nodiscard]] float GetHealth() const {
    return health_;
  }

  /*
   * The value of new_health will be clamped to [0, 1]
   * */
  void SetHealth(float new_health) {
    health_ = std::clamp(new_health, 0.0f, 1.0f);
  }

  void CountDown();

  void SetLifeBarLength(float new_length);
  void SetLifeBarOffset(glm::vec2 new_offset);
  void SetLifeBarFrontColor(glm::vec4 new_color);
  void SetLifeBarBackgroundColor(glm::vec4 new_color);
  void SetLifeBarFadeoutColor(glm::vec4 new_color);
  [[nodiscard]] float GetLifeBarLength();
  [[nodiscard]] glm::vec2 GetLifeBarOffset();
  [[nodiscard]] glm::vec4 GetLifeBarFrontColor();
  [[nodiscard]] glm::vec4 GetLifeBarBackgroundColor();
  [[nodiscard]] glm::vec4 GetLifeBarFadeoutColor();

  void ShowLifeBar();
  void HideLifeBar();
  virtual void RenderLifeBar();

  /*
   * This virtual function is used to render some extra helpers, such as
   * predicted trajectory of the bullet the unit will shoot, and etc., only
   * in the first-person perspective.
   * */
  virtual void RenderHelper();

  /*
   * This virtual function is used to check whether a bullet at the position
   * have hit the unit. If the position is inside the unit area, then return
   * true, otherwise return false.
   * */
  [[nodiscard]] virtual bool IsHit(glm::vec2 position) const = 0;

  /*
   * This function wraps PushEventMoveUnit andPushEventRotateUnit. The
   * offsets are usually 0 or 1, because we multiplied it with speed inside the
   * implementation.
   * */
  void MoveAndRotate(glm::vec2 offset, float rotation_offset);

  template <class BulletType, class... Args>
  void GenerateBullet(glm::vec2 position,
                      float rotation,
                      float damage_scale = 1.0f,
                      Args... args);

  [[nodiscard]] virtual const char *UnitName() const;
  [[nodiscard]] virtual const char *Author() const;
  [[nodiscard]] virtual const char *Brief() const;

 protected:
  uint32_t player_id_{};
  float health_{1.0f};
  bool lifebar_display_{true};
  glm::vec2 lifebar_offset_{};
  float lifebar_length_{2.4f};
  glm::vec4 front_lifebar_color_{};
  glm::vec4 background_lifebar_color_{};
  glm::vec4 fadeout_lifebar_color_{};

 private:
  float fadeout_health_;

 protected:
  float speed_scale_{1.0f};
  int speed_change_count_down_{0};

  float damage_scale_{1.0f};
  int damage_change_count_down_{0};

  float health_scale_{1.0f};
  float health_change_count_down_{0};

  int fire_count_down_{0};
  float fire_interval_scale_{1.0f};
  int fire_interval_change_count_down_{0};

  int position_change_count_down_{0};
};

}  // namespace battle_game

// add something to pull
