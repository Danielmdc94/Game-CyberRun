#include "EnemyBullet.h"
#include "../MathUtils.h"
#include "World.h"
#include "Player.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>

EnemyBullet::EnemyBullet()
{
    m_shape.setSize({16.f, 16.f});
    m_shape.setFillColor(sf::Color::Red);
    m_shape.setOutlineColor(sf::Color::Black);
    m_shape.setOutlineThickness(2.f);
    m_shape.setOrigin(m_shape.getSize() * 0.5f);
}

bool EnemyBullet::init()
{
    return true;
}

void EnemyBullet::update(float dt)
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

        Player* player = m_world->GetPlayer();
        if (player && !player->IsDead())
        {
            if (player->CheckCollision(GetBounds()))
            {
                player->TakeDamage(1);
                MarkForDeletion();
            }
        }
    }
}

bool EnemyBullet::CheckCollision(const sf::FloatRect& rect)
{
    if (m_shape.getGlobalBounds().findIntersection(rect))
        return true;
    else
        return false;
}

void EnemyBullet::render(sf::RenderTarget& target) const
{
    target.draw(m_shape);
}

void EnemyBullet::SetPosition(const sf::Vector2f& l_pos)
{
    m_shape.setPosition(l_pos);
}
