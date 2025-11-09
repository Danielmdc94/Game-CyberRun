#pragma once
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include <memory>
#include <vector>

#include "gamestates/IState.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "entities/Bullet.h"
#include "entities/EnemyBullet.h"
#include "entities/PowerUp.h"

class ParallaxLayer;

class World
{
public:
	World(IState* l_owner) : m_owner(l_owner) {}
	~World() = default;

	void OnCreate();
	void OnDestroy();

	void Draw(sf::RenderTarget* l_window);
	void Update(sf::Time l_deltaTime);

	IState* GetOwner() const { return m_owner; }
	float GetWorldSpeed() const { return m_worldSpeed; }
	
	void SetWorldSpeed(float l_worldSpeed) { 
		m_worldSpeed = l_worldSpeed; 
		UpdateParallaxSpeeds(); 
	}

	Player* GetPlayer() const { return m_player.get(); }
	const std::vector<sf::RectangleShape>& GetPlatforms() const { return m_platforms; }
	const std::vector<sf::RectangleShape>& GetFloorSegments() const { return m_floorSegments; }
	std::vector<std::unique_ptr<Enemy>>& GetEnemies() { return m_enemies; }
	std::vector<std::unique_ptr<Bullet>>& GetBullets() { return m_bullets; }

	void SpawnPlayer();
	void SpawnBullet(const sf::Vector2f& l_position, const sf::Vector2f& l_direction);
	void SpawnEnemyBullet(const sf::Vector2f& l_position, const sf::Vector2f& l_direction);
	void GeneratePlatforms();
	void UpdatePlatforms(float l_deltaTime);
	void SpawnEnemies(float l_deltaTime);
	void UpdateParallaxSpeeds();
	
	void UpdateEnemies(float l_deltaTime);
	void UpdateBullets(float l_deltaTime);
	void UpdateEnemyBullets(float l_deltaTime);
	void DrawEnemies(sf::RenderTarget& l_target);
	void DrawBullets(sf::RenderTarget& l_target);
	void DrawEnemyBullets(sf::RenderTarget& l_target);
	
	void SpawnPowerUps(float l_deltaTime);
	void UpdatePowerUps(float l_deltaTime);
	void DrawPowerUps(sf::RenderTarget& l_target);


protected:
	IState* m_owner;
	
private:
	ParallaxLayer* m_backgroundLayer = nullptr;
	ParallaxLayer* m_buildingsLayer = nullptr;

	float m_zoomDelta = 1.f;
	float m_zoom = 1.f;
	float m_spawnTimer = 0.f;
	float m_worldSpeed = 150.f;

	float m_platformTimer = 0.f;
	float m_platformSpawnRate = 2.0f;
	std::vector<sf::RectangleShape> m_platforms;
	std::vector<sf::RectangleShape> m_floorSegments;

	float m_enemyTimer = 0.f;
	float m_enemySpawnRate = 4.0f;
	int m_enemySpawnCounter = 0;

	float m_powerUpTimer = 0.f;
	float m_powerUpSpawnRate = 15.0f;

	std::unique_ptr<Player> m_player;
	std::vector<std::unique_ptr<Enemy>> m_enemies;
	std::vector<std::unique_ptr<Bullet>> m_bullets;
	std::vector<std::unique_ptr<EnemyBullet>> m_enemyBullets;
	std::vector<std::unique_ptr<PowerUp>> m_powerUps;
};
