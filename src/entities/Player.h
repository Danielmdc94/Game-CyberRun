#pragma once

#include "Entity.h"
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Audio.hpp>

namespace sf { class Sprite; };
class World;

class Player final : public Entity
{
public:
    static constexpr float collisionRadius = 42.0f;

    Player();
    virtual ~Player() = default;

    bool init() override;
    void SetupAnimations(unsigned l_frameWidth, unsigned l_frameHeight);

    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

	sf::Vector2f GetPosition() const { return m_shape.getPosition(); }
	sf::Vector2f GetSpawnPoint() const { return m_spawnPosition; }
	void SetSpawnPosition(const sf::Vector2f& l_pos) { m_spawnPosition = l_pos; }

	bool CheckCollision(const sf::FloatRect& rect);
	void Die();
	bool IsDead() const { return m_isDead; }
	void SetPosition(const sf::Vector2f& l_pos) { m_shape.setPosition(l_pos); m_sprite->setPosition(l_pos); }

	void SetWorld(World* l_world) { m_world = l_world; }
	World* GetWorld() const { return m_world; }

	void OnMoveUpPressed() { m_moveUp = true; }
	void OnMoveDownPressed() { m_moveDown = true; }
	void OnMoveLeftPressed() { m_moveLeft = true; }
	void OnMoveRightPressed() { m_moveRight = true; }
	void OnJumpPressed() { m_jump = true; }
	void OnRunPressed() { m_running = true; }
	void OnDashPressed();
	
	void OnMoveUpReleased() { m_moveUp = false; }
	void OnMoveDownReleased() { m_moveDown = false; }
	void OnMoveLeftReleased() { m_moveLeft = false; }
	void OnMoveRightReleased() { m_moveRight = false; }
	void OnJumpReleased() { m_jump = false; }
	void OnRunReleased() { m_running = false; }

	void ResetKeys();
	void Shoot(const sf::Vector2f& l_mousePosition);
	void LoadSounds();
	void EnemyKill();
	
	void SetHealth(int l_health);
	void Heal(int l_amount) { SetHealth(m_currentHealth + l_amount); }
	void TakeDamage(int l_amount) { SetHealth(m_currentHealth - l_amount); if (m_hurtSound) m_hurtSound->play(); }
	int GetHealth() const { return m_currentHealth; }
	int GetMaxHealth() const { return m_maxHealth; }
	int GetScore() const { return m_score; }
	
private:
	void ResolveWorldCollisions(sf::Vector2f& l_velocity, float l_deltaTime);
	void HandleDash(float l_deltaTime);
	void HandleMove(float l_deltaTime);
	void HandleJump(float l_deltaTime);
	void HandleFall(float l_deltaTime);
	void HandleCollisions(float l_deltaTime);
	void HandleAnimations(float l_deltaTime);

	World* m_world = nullptr;
	int m_score = 0;
	
	sf::Vector2f m_spawnPosition;
	sf::RectangleShape m_shape;
	float m_speed;

	bool m_isDead = false;

	bool m_moveUp = false;
	bool m_moveDown = false;
	bool m_moveLeft = false;
	bool m_moveRight = false;
	bool m_jump = false;
	bool m_prevJump = false;

    std::unique_ptr<sf::Sprite>  m_sprite;
    std::vector<sf::IntRect> m_framesRun;
    std::vector<sf::IntRect> m_framesJump;
    std::size_t m_currentFrame = 0;
    float m_frameTime = 0.12f;
    float m_frameTimer = 0.f;
    bool m_isAnimRun = true;
    bool m_isAnimJump = false;

	int m_currentHealth = 3;
	static constexpr int m_maxHealth = 3;

	sf::Vector2f m_velocity{0.f, 0.f};
	bool m_onGround = false;
	float m_maxSpeedX{300.f};
	float m_accelGround{2500.f};
	float m_decelGround{3000.f};
	float m_accelAir{3500.f};

	bool m_running = false;
	float m_runSpeedMultiplier = 1.7f;
	
	float m_gravity = 1900.f;
	float m_fallGravityMultiplier = 1.5f;
	float m_jumpCutMultiplier = 3.0f;
	float m_jumpSpeed = 900.f;
	unsigned m_jumpCount = 0;
	unsigned m_maxJumps = 2;

	bool m_canDash = true;
	bool m_isDashing = false;
	float m_dashSpeed = 1000.f;
	float m_dashDuration = 0.2f;
	float m_dashCooldown = 0.5f;
	float m_dashRechargeTimer = 0.f;

	float m_coyoteTime = 0.10f;
	float m_jumpBufferTime = 0.12f;
	float m_shootCooldown = 0.25f;

	float m_coyoteTimer = 0.f;
	float m_jumpBufferTimer = 0.f;
	float m_dashTimer = 0.f;
	float m_shootTimer = 0.f;
	
	sf::SoundBuffer m_shootSoundBuffer;
	std::unique_ptr<sf::Sound> m_shootSound;
	
	sf::SoundBuffer m_jumpSoundBuffer;
	std::unique_ptr<sf::Sound> m_jumpSound;
	
	sf::SoundBuffer m_explosionSoundBuffer;
	std::unique_ptr<sf::Sound> m_explosionSound;

	sf::SoundBuffer m_hurtSoundBuffer;
	std::unique_ptr<sf::Sound> m_hurtSound;

	sf::SoundBuffer m_dashSoundBuffer;
	std::unique_ptr<sf::Sound> m_dashSound;
};
