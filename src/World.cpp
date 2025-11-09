#include "World.h"
#include "ParallaxLayer.h"
#include "MathUtils.h"

#include <iostream>
#include <algorithm>

void World::OnCreate()
{
	m_backgroundLayer = new ParallaxLayer(this);
	m_backgroundLayer->Init("Background.png", m_worldSpeed * 0.5f, {1920, 1080}, 1.2f, -100.f);

	m_buildingsLayer = new ParallaxLayer(this);
	m_buildingsLayer->Init("Buildings.png", m_worldSpeed * 1.0f, {1920, 1080}, 0.8f, 300.f);

	GeneratePlatforms();

	m_player = std::make_unique<Player>();
    if (!m_player || !m_player->init())
	{
		return;
	}
	
	m_player->SetWorld(this);
}

void World::OnDestroy()
{
	delete m_backgroundLayer;
	m_backgroundLayer = nullptr;
	delete m_buildingsLayer;
	m_buildingsLayer = nullptr;

	m_player.reset();
	m_enemies.clear();
	m_bullets.clear();
	m_enemyBullets.clear();
	m_powerUps.clear();
	m_platforms.clear();
	m_floorSegments.clear();
}

void World::Draw(sf::RenderTarget* l_window)
{
	if (m_backgroundLayer)
		m_backgroundLayer->Draw(*l_window);
	if (m_buildingsLayer)
		m_buildingsLayer->Draw(*l_window);

	for (const auto& segment : m_floorSegments) 
	{
		l_window->draw(segment);
	}
	for (const auto& platform : m_platforms) 
	{
		l_window->draw(platform);
	}

	DrawEnemies(*l_window);
	DrawBullets(*l_window);
	DrawEnemyBullets(*l_window);
	DrawPowerUps(*l_window);
	
	if (m_player)
		m_player->render(*l_window);
}

void World::Update(sf::Time l_deltaTime)
{
	static float timeAccumulator = 0.f;
	timeAccumulator += l_deltaTime.asSeconds();
	
	if (timeAccumulator >= 2.f && m_worldSpeed < 300.f) 
	{
		timeAccumulator = 0.f;
		SetWorldSpeed(m_worldSpeed + 1.f);
	}
	
	if (m_backgroundLayer)
		m_backgroundLayer->Update(l_deltaTime.asSeconds());

	if (m_buildingsLayer)
	{
		const float windowHeight = 1080.f;
		const float groundTopY = windowHeight - 50.f;
		float extraYOffset = 0.f;

		if (m_player)
		{
			sf::Vector2f p = m_player->GetPosition();
			float halfHeight = 50.f;
			float playerBottom = p.y + halfHeight;
			float heightOverGround = std::max(0.f, groundTopY - playerBottom);

			const float parallaxFactor = 0.25f;
			extraYOffset = heightOverGround * parallaxFactor;
		}

		m_buildingsLayer->SetExtraYOffset(extraYOffset);
		m_buildingsLayer->Update(l_deltaTime.asSeconds());
	}
	

	UpdatePlatforms(l_deltaTime.asSeconds());
	SpawnEnemies(l_deltaTime.asSeconds());
	SpawnPowerUps(l_deltaTime.asSeconds());

	UpdateEnemies(l_deltaTime.asSeconds());
	UpdateBullets(l_deltaTime.asSeconds());
	UpdateEnemyBullets(l_deltaTime.asSeconds());
	UpdatePowerUps(l_deltaTime.asSeconds());
	
	if (m_player)
		m_player->update(l_deltaTime.asSeconds());
}

void World::GeneratePlatforms()
{
	sf::Vector2u windowSize(1920, 1080);

	m_floorSegments.clear();
	m_platforms.clear();

	float floorY = static_cast<float>(windowSize.y) - 50.f;
	
	sf::RectangleShape initialFloor;
	initialFloor.setSize({static_cast<float>(windowSize.x), 50.f});
	initialFloor.setPosition({0.f, floorY});
	initialFloor.setFillColor(sf::Color(209, 134, 59));
	initialFloor.setOutlineColor(sf::Color(10, 10, 10));
	initialFloor.setOutlineThickness(6.f);
	m_floorSegments.push_back(initialFloor);
}

void World::UpdatePlatforms(float l_deltaTime)
{
	sf::Vector2u windowSize(1920, 1080);
	
	for (auto& platform : m_platforms) 
	{
		sf::Vector2f pos = platform.getPosition();
		pos.x -= m_worldSpeed * l_deltaTime;
		platform.setPosition(pos);
	}
	
	for (auto& segment : m_floorSegments) 
	{
		sf::Vector2f pos = segment.getPosition();
		pos.x -= m_worldSpeed * l_deltaTime;
		segment.setPosition(pos);
	}
	
	m_platforms.erase(std::remove_if(m_platforms.begin(), m_platforms.end(), [](const sf::RectangleShape& platform) 
		{
			return platform.getPosition().x + platform.getSize().x < -100.f;
		}), m_platforms.end());
	
	m_floorSegments.erase(std::remove_if(m_floorSegments.begin(), m_floorSegments.end(), [](const sf::RectangleShape& segment) 
		{
			return segment.getPosition().x + segment.getSize().x < -100.f;
		}), m_floorSegments.end());
	
	m_platformTimer += l_deltaTime;
	if (m_platformTimer >= m_platformSpawnRate) 
	{
		m_platformTimer = 0.f;
		
		sf::RectangleShape newPlatform;
		newPlatform.setSize({120.f, 20.f});
		
		float minY = 200.f;
		float maxY = static_cast<float>(windowSize.y) - 150.f;
		float randomY = minY + static_cast<float>(rand()) / RAND_MAX * (maxY - minY);

		newPlatform.setPosition({static_cast<float>(windowSize.x) + 100.f, randomY});
		newPlatform.setFillColor(sf::Color(209, 134, 59));
		newPlatform.setOutlineColor(sf::Color(10, 10, 10));
		newPlatform.setOutlineThickness(6.f);
		m_platforms.push_back(newPlatform);
	}
	
	if (m_floorSegments.empty() || m_floorSegments.back().getPosition().x < windowSize.x + 200.f) 
	{
		float floorY = static_cast<float>(windowSize.y) - 50.f;
		float segmentWidth = 500.f;
		
		float baseGapWidth = 250.f;
		float speedProgress = (m_worldSpeed - 150.f) / (300.f - 150.f);
		speedProgress = std::max(0.f, std::min(1.f, speedProgress));
		float gapWidth = baseGapWidth + speedProgress * (1000.f - baseGapWidth);
		
		float lastX = windowSize.x + 200.f;
		if (!m_floorSegments.empty()) 
		{
			auto& lastSegment = m_floorSegments.back();
			lastX = lastSegment.getPosition().x + lastSegment.getSize().x + gapWidth;
		}

		for (int i = 0; i < 3; ++i) 
		{
			sf::RectangleShape floorSegment;
			floorSegment.setSize({segmentWidth, 50.f});
			floorSegment.setPosition({lastX + i * (segmentWidth + gapWidth), floorY});
			floorSegment.setFillColor(sf::Color(209, 134, 59));
			floorSegment.setOutlineColor(sf::Color(10, 10, 10));
			floorSegment.setOutlineThickness(6.f);
			m_floorSegments.push_back(floorSegment);
		}
	}
}

void World::SpawnEnemies(float l_deltaTime)
{
	sf::Vector2u windowSize(1920, 1080);

	float baseSpeed = 150.f;
	float dynamicSpawnRate = m_enemySpawnRate * (baseSpeed / m_worldSpeed);
	
	float minSpawnRate = 0.5f;
	dynamicSpawnRate = std::max(dynamicSpawnRate, minSpawnRate);
	
	m_enemyTimer += l_deltaTime;
	if (m_enemyTimer >= dynamicSpawnRate) 
	{
		m_enemyTimer = 0.f;
		m_enemySpawnCounter++;
		
		int enemiesToSpawn = (m_enemySpawnCounter % 10 == 0) ? 5 : 2;
		
		for (int i = 0; i < enemiesToSpawn; ++i)
		{
			auto enemy = std::make_unique<Enemy>();
			if (enemy)
			{
				sf::Vector2f spawnPos;
				
				float groundY = static_cast<float>(windowSize.y) - 50.f;
				float minY = 100.f;
				float maxY = groundY;
				float randomY = minY + static_cast<float>(rand()) / RAND_MAX * (maxY - minY);
				
				spawnPos.x = static_cast<float>(windowSize.x) + 100.f + i * 150.f;
				spawnPos.y = randomY - 50.f;
				
				enemy->SetWorld(this);
				enemy->SetPosition(spawnPos);
				if (enemy->init())
				{
					m_enemies.push_back(std::move(enemy));
				}
			}
		}
	}
}

void World::UpdateEnemies(float l_deltaTime)
{
	for (auto& enemy : m_enemies)
	{
		if (enemy)
		{
			enemy->update(l_deltaTime);
		}
	}
	
	m_enemies.erase(std::remove_if(m_enemies.begin(), m_enemies.end(),[](const std::unique_ptr<Enemy>& enemy)
		{
			return !enemy || enemy->IsMarkedForDeletion() || enemy->GetPosition().x < -200.f;
		}), m_enemies.end());
}

void World::DrawEnemies(sf::RenderTarget& l_target)
{
	for (const auto& enemy : m_enemies)
	{
		if (enemy)
		{
			enemy->render(l_target);
		}
	}
}

void World::SpawnBullet(const sf::Vector2f& l_position, const sf::Vector2f& l_direction)
{
	auto bullet = std::make_unique<Bullet>();
	if (bullet)
	{
		bullet->SetWorld(this);
		bullet->SetPosition(l_position);
		
		sf::Vector2f normalizedDir = VecNormalized(l_direction);
		
		bullet->SetVelocity(normalizedDir * 750.f);
		
		if (bullet->init())
		{
			m_bullets.push_back(std::move(bullet));
		}
	}
}

void World::UpdateBullets(float l_deltaTime)
{
	for (auto& bullet : m_bullets)
	{
		if (bullet)
		{
			bullet->update(l_deltaTime);
		}
	}
	
	m_bullets.erase(std::remove_if(m_bullets.begin(), m_bullets.end(), [](const std::unique_ptr<Bullet>& bullet) 
		{
			return !bullet || bullet->IsMarkedForDeletion();
		}), m_bullets.end());
}

void World::DrawBullets(sf::RenderTarget& l_target)
{
	for (const auto& bullet : m_bullets)
	{
		if (bullet)
		{
			bullet->render(l_target);
		}
	}
}

void World::SpawnEnemyBullet(const sf::Vector2f& l_position, const sf::Vector2f& l_direction)
{
	auto bullet = std::make_unique<EnemyBullet>();
	if (bullet)
	{
		bullet->SetWorld(this);
		bullet->SetPosition(l_position);
		
		sf::Vector2f normalizedDir = VecNormalized(l_direction);
		
		bullet->SetVelocity(normalizedDir * 500.f);
		
		if (bullet->init())
		{
			m_enemyBullets.push_back(std::move(bullet));
		}
	}
}

void World::UpdateEnemyBullets(float l_deltaTime)
{
	for (auto& bullet : m_enemyBullets)
	{
		if (bullet)
		{
			bullet->update(l_deltaTime);
		}
	}
	
	m_enemyBullets.erase(std::remove_if(m_enemyBullets.begin(), m_enemyBullets.end(), [](const std::unique_ptr<EnemyBullet>& bullet) 
		{
			return !bullet || bullet->IsMarkedForDeletion();
		}), m_enemyBullets.end());
}

void World::DrawEnemyBullets(sf::RenderTarget& l_target)
{
	for (const auto& bullet : m_enemyBullets)
	{
		if (bullet)
			bullet->render(l_target);
	}
}

void World::UpdateParallaxSpeeds()
{
	if (m_backgroundLayer) {
		m_backgroundLayer->SetScrollSpeed(m_worldSpeed * 0.5f);
	}
	if (m_buildingsLayer) {
		m_buildingsLayer->SetScrollSpeed(m_worldSpeed * 1.0f);
	}
}

void World::SpawnPowerUps(float l_deltaTime)
{
	sf::Vector2u windowSize(1920, 1080);
	
	m_powerUpTimer += l_deltaTime;
	if (m_powerUpTimer >= m_powerUpSpawnRate)
	{
		m_powerUpTimer = 0.f;
		
		auto powerUp = std::make_unique<PowerUp>();
		if (powerUp)
		{
			sf::Vector2f spawnPos;
			
			float minY = 300.f;
			float maxY = 700.f;
			float randomY = minY + static_cast<float>(rand()) / RAND_MAX * (maxY - minY);
			
			spawnPos.x = static_cast<float>(windowSize.x) + 100.f;
			spawnPos.y = randomY;
			
			powerUp->SetWorld(this);
			powerUp->SetType(PowerUpType::Health);
			powerUp->SetPosition(spawnPos);
			
			if (powerUp->init())
			{
				m_powerUps.push_back(std::move(powerUp));
			}
		}
	}
}

void World::UpdatePowerUps(float l_deltaTime)
{
	for (auto& powerUp : m_powerUps)
	{
		if (powerUp)
		{
			powerUp->update(l_deltaTime);
		}
	}
	
	m_powerUps.erase(std::remove_if(m_powerUps.begin(), m_powerUps.end(), [](const std::unique_ptr<PowerUp>& powerUp) 
		{
			return !powerUp || powerUp->IsMarkedForDeletion() || powerUp->GetPosition().x < -200.f;
		}), m_powerUps.end());
}

void World::DrawPowerUps(sf::RenderTarget& l_target)
{
	for (const auto& powerUp : m_powerUps)
	{
		if (powerUp)
		{
			powerUp->render(l_target);
		}
	}
}
