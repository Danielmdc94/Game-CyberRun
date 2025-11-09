#include "Bullet.h"
#include "World.h"
#include "Enemy.h"
#include "Player.h"
#include "../MathUtils.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>

Bullet::Bullet()
{
    m_shape.setSize({8.f, 8.f});
    m_shape.setFillColor(sf::Color::Green);
    m_shape.setOutlineColor(sf::Color::Black);
    m_shape.setOutlineThickness(1.f);
    m_shape.setOrigin(m_shape.getSize() * 0.5f);
}

bool Bullet::init()
{
    return true;
}

void Bullet::update(float dt)
{
    if (m_world)
    {
        sf::Vector2f newPosition = GetPosition() + m_velocity * dt;
        SetPosition(newPosition);

        sf::Vector2u windowSize(1920, 1080);
        
        float leftBound = -static_cast<float>(windowSize.x) * 0.1f;
        float rightBound = static_cast<float>(windowSize.x) * 1.1f;
        float topBound = -static_cast<float>(windowSize.y) * 0.1f;
        float bottomBound = static_cast<float>(windowSize.y) * 1.1f;
        
        sf::Vector2f bulletPos = GetPosition();
        if (bulletPos.x < leftBound || bulletPos.x > rightBound ||
            bulletPos.y < topBound || bulletPos.y > bottomBound)
        {
            MarkForDeletion();
            return;
        }

        auto& enemies = m_world->GetEnemies();
        for (auto& enemy : enemies)
        {
            if (enemy && !enemy->IsMarkedForDeletion())
            {
                if (enemy->CheckCollision(GetBounds()))
                {
                    Player* player = m_world->GetPlayer();
                    if (player)
                    {
                        player->EnemyKill();
                    }
                    
                    enemy->MarkForDeletion();
                    MarkForDeletion();
                    break;
                }
            }
        }
    }
}

bool Bullet::CheckCollision(const sf::FloatRect& rect)
{
    if (m_shape.getGlobalBounds().findIntersection(rect))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Bullet::render(sf::RenderTarget& target) const
{
    target.draw(m_shape);
}

void Bullet::SetPosition(const sf::Vector2f& l_pos)
{
    m_shape.setPosition(l_pos);
}
