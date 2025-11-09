#include "PowerUp.h"
#include "ResourceManager.h"
#include "Player.h"
#include "World.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

PowerUp::PowerUp()
{
    
}

bool PowerUp::init()
{
    const sf::Texture* texture = ResourceManager::getOrLoadTexture("Health.png");
    if (texture == nullptr)
    {
        return false;
    }
    m_sprite = std::make_unique<sf::Sprite>(*texture);
    if (!m_sprite)
    {
        return false;
    }
    unsigned frameWidth = 67;
    unsigned frameHeight = 60;
    m_sprite->setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(frameWidth, frameHeight)));
    m_sprite->setOrigin({frameWidth / 2.f, frameHeight / 2.f});
    m_sprite->setPosition({0.f, 0.f});
    
    m_shape.setSize({static_cast<float>(frameWidth), static_cast<float>(frameHeight)});
    m_shape.setFillColor({0, 255, 0, 50});
    m_shape.setOutlineColor(sf::Color::Green);
    m_shape.setOutlineThickness(1.f);
    m_shape.setOrigin(m_shape.getSize() * 0.5f);

    return true;
}

void PowerUp::update(float dt)
{
    if (m_world)
    {
        Player* player = m_world->GetPlayer();
        if (player)
        {
            if (!IsMarkedForDeletion() && player->CheckCollision(m_shape.getGlobalBounds()))
            {
                if (m_type == PowerUpType::Health)
                {
                    player->Heal(1);
                }
                MarkForDeletion();
                return;
            }
            
            float speed = m_world->GetWorldSpeed();
            sf::Vector2f velocity = {speed * -1.f, 0.f};
            sf::Vector2f newPosition = GetPosition() + velocity * dt;
            SetPosition(newPosition);
        }
    }
}

void PowerUp::render(sf::RenderTarget& target) const
{
    if (m_sprite)
    {
        target.draw(*m_sprite);
    }
}

void PowerUp::SetPosition(const sf::Vector2f& l_pos)
{
    m_shape.setPosition(l_pos);
    if (m_sprite)
    {
        m_sprite->setPosition(l_pos);
    }
}

bool PowerUp::CheckCollision(const sf::FloatRect& rect)
{
    if (m_shape.getGlobalBounds().findIntersection(rect))
    {
        return true;
    }
    return false;
}
