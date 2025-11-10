#include "Boss.h"
#include "ResourceManager.h"
#include "Player.h"
#include "World.h"
#include "MathUtils.h"
#include <cmath>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

Boss::Boss()
{

}

bool Boss::init()
{
    const sf::Texture* texture = ResourceManager::getOrLoadTexture("Boss.png");
    if (texture == nullptr)
    {
        return false;
    }
    m_sprite = std::make_unique<sf::Sprite>(*texture);
    m_sprite->setOrigin({27.f, 26.f});
    m_sprite->setPosition({0.f, 0.f});
    m_sprite->setScale({3.0f, 3.0f});
    
    m_shape.setSize({34.f, 40.f});
    m_shape.setFillColor({255, 0, 255, 50});
    m_shape.setOutlineColor(sf::Color::Magenta);
    m_shape.setOutlineThickness(2.f);
    m_shape.setOrigin(m_shape.getSize() * 0.5f);
    m_shape.setScale({3.0f, 3.0f});

    m_health = maxHealth;
    
    m_shootCooldown = 1.5f;
    m_shootTimer = m_shootCooldown;

    return true;
}

void Boss::update(float dt)
{
    if (m_world)
    {
        Player* player = m_world->GetPlayer();
        if (player)
        {
            if (!IsMarkedForDeletion() && player->CheckCollision(m_shape.getGlobalBounds()))
            {
                player->TakeDamage(1);
            }
            
            sf::Vector2f currentPos = GetPosition();
            
            if (!m_hasEnteredScreen)
            {
                if (currentPos.x > m_entryX)
                {
                    sf::Vector2f entryVelocity = {-150.f, 0.f};
                    SetPosition(currentPos + entryVelocity * dt);
                }
                else
                {
                    m_hasEnteredScreen = true;
                    m_targetPosition = {m_entryX, 400.f};
                }
            }
            else
            {
                float distanceToTarget = VecLength(m_targetPosition - currentPos);
                
                if (distanceToTarget > 5.f)
                {
                    sf::Vector2f direction = VecNormalized(m_targetPosition - currentPos);
                    sf::Vector2f velocity = direction * m_moveSpeed;
                    SetPosition(currentPos + velocity * dt);
                }
                
                m_moveTimer += dt;
                if (m_moveTimer >= m_moveChangeCooldown)
                {
                    m_moveTimer = 0.f;
                    
                    float minY = 150.f;
                    float maxY = 900.f;
                    float randomY = minY + static_cast<float>(rand()) / RAND_MAX * (maxY - minY);
                    m_targetPosition = {m_entryX, randomY};
                }
            }
            
            // Shooting pattern - burst fire
            if (m_hasEnteredScreen)
            {
                m_shootTimer -= dt;
                
                if (m_burstCount > 0)
                {
                    m_burstTimer -= dt;
                    if (m_burstTimer <= 0.f)
                    {
                        Shoot();
                        m_burstCount--;
                        m_burstTimer = m_burstDelay;
                    }
                }
                else if (m_shootTimer <= 0.f)
                {
                    // Start a new burst
                    m_burstCount = m_maxBurstShots;
                    Shoot();
                    m_burstCount--;
                    m_burstTimer = m_burstDelay;
                    m_shootTimer = m_shootCooldown;
                }
            }
        }
    }
}

void Boss::Shoot()
{
    if (m_world)
    {
        Player* player = m_world->GetPlayer();
        if (player)
        {
            sf::Vector2f bossPos = GetPosition();
            sf::Vector2f playerPos = player->GetPosition();
            
            // Shoot at player
            sf::Vector2f direction = playerPos - bossPos;
            m_world->SpawnEnemyBullet(bossPos, direction);
            
            // Also shoot two additional bullets at slight angles (spread pattern)
            float spreadAngle = 15.f; // degrees
            sf::Vector2f normalizedDir = VecNormalized(direction);
            
            // Calculate angle of main direction
            float angle = std::atan2(normalizedDir.y, normalizedDir.x);
            
            // Left spread bullet
            float leftAngle = angle + spreadAngle * 3.14159f / 180.f;
            sf::Vector2f leftDir = {std::cos(leftAngle), std::sin(leftAngle)};
            m_world->SpawnEnemyBullet(bossPos, leftDir);
            
            // Right spread bullet
            float rightAngle = angle - spreadAngle * 3.14159f / 180.f;
            sf::Vector2f rightDir = {std::cos(rightAngle), std::sin(rightAngle)};
            m_world->SpawnEnemyBullet(bossPos, rightDir);
        }
    }
}

void Boss::TakeDamage(int damage)
{
    m_health -= damage;
    if (m_health <= 0)
    {
        m_health = 0;
        MarkForDeletion();
    }
}

void Boss::render(sf::RenderTarget& target) const
{
    if (m_sprite)
    {
        target.draw(*m_sprite);
    }
    
    if (m_health > 0 && m_hasEnteredScreen)
    {
        sf::Vector2f bossPos = GetPosition();
        float healthBarWidth = 200.f;
        float healthBarHeight = 20.f;
        float healthPercent = static_cast<float>(m_health) / static_cast<float>(maxHealth);
        
        sf::RectangleShape healthBarBg;
        healthBarBg.setSize({healthBarWidth, healthBarHeight});
        healthBarBg.setPosition({bossPos.x - healthBarWidth / 2.f, bossPos.y - 120.f});
        healthBarBg.setFillColor(sf::Color::Red);
        healthBarBg.setOutlineColor(sf::Color::White);
        healthBarBg.setOutlineThickness(2.f);
        target.draw(healthBarBg);
        
        sf::RectangleShape healthBarFg;
        healthBarFg.setSize({healthBarWidth * healthPercent, healthBarHeight});
        healthBarFg.setPosition({bossPos.x - healthBarWidth / 2.f, bossPos.y - 120.f});
        healthBarFg.setFillColor(sf::Color::Green);
        target.draw(healthBarFg);
    }
}

void Boss::SetPosition(const sf::Vector2f& l_pos)
{
    m_shape.setPosition(l_pos);
    if (m_sprite)
    {
        m_sprite->setPosition(l_pos);
    }
}

bool Boss::CheckCollision(const sf::FloatRect& rect)
{
    if (m_shape.getGlobalBounds().findIntersection(rect))
    {
        return true;
    }
    return false;
}
