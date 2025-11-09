#include "Enemy.h"
#include "ResourceManager.h"
#include "Player.h"
#include "World.h"
#include <cmath>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

Enemy::Enemy()
{

}

bool Enemy::init()
{
    const sf::Texture* texture = ResourceManager::getOrLoadTexture("Enemy.png");
    if (texture == nullptr)
    {
        return false;
    }
    m_sprite = std::make_unique<sf::Sprite>(*texture);
    m_sprite->setOrigin({27.f, 26.f});
    m_sprite->setPosition({0.f, 0.f});
    m_sprite->setScale({1.5f, 1.5f});
    
    m_shape.setSize({34.f, 40.f});
    m_shape.setFillColor({255, 0, 0, 50});
    m_shape.setOutlineColor(sf::Color::Red);
    m_shape.setOutlineThickness(1.f);
    m_shape.setOrigin(m_shape.getSize() * 0.5f);
    m_shape.setScale({1.5f, 1.5f});

    m_speedMultiplier = 0.75f + static_cast<float>(rand()) / RAND_MAX * (2.0f - 0.75f);
    
    m_shootCooldown = 2.0f + static_cast<float>(rand()) / RAND_MAX * 3.0f;
    m_shootTimer = m_shootCooldown;

    return true;
}

void Enemy::update(float dt)
{
    if (m_world)
    {
        Player* player = m_world->GetPlayer();
        if (player)
        {
            if (!IsMarkedForDeletion() && player->CheckCollision(m_shape.getGlobalBounds()))
            {
                player->TakeDamage(1);
                MarkForDeletion();
                return;
            }
            float speed = m_world->GetWorldSpeed() * m_speedMultiplier;
            sf::Vector2f velocity = {speed * -1.f, 0.f};
            sf::Vector2f newPosition = GetPosition() + velocity * dt;
            SetPosition(newPosition);
            
            m_shootTimer -= dt;
            if (m_shootTimer <= 0.f)
            {
                Shoot();
                m_shootCooldown = 2.0f + static_cast<float>(rand()) / RAND_MAX * 3.0f;
                m_shootTimer = m_shootCooldown;
            }
        }
    }
}

void Enemy::Shoot()
{
    if (m_world)
    {
        Player* player = m_world->GetPlayer();
        if (player)
        {
            sf::Vector2f enemyPos = GetPosition();
            sf::Vector2f playerPos = player->GetPosition();
            
            sf::Vector2f direction = playerPos - enemyPos;
            
            m_world->SpawnEnemyBullet(enemyPos, direction);
        }
    }
}

void Enemy::render(sf::RenderTarget& target) const
{
    if (m_sprite)
    {
        target.draw(*m_sprite);
    }
}

void Enemy::SetPosition(const sf::Vector2f& l_pos)
{
    m_shape.setPosition(l_pos);
    if (m_sprite)
    {
        m_sprite->setPosition(l_pos);
    }
}

bool Enemy::CheckCollision(const sf::FloatRect& rect)
{
    if (m_shape.getGlobalBounds().findIntersection(rect))
    {
        return true;
    }
    return false;
}
