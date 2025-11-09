#pragma once

#include "Entity.h"
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf { class Sprite; class RenderWindow; }
class World;

class Enemy final : public Entity
{
public:
    static constexpr float collisionRadius = 24.0f;

    Enemy();
    virtual ~Enemy() = default;
    
    bool init() override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;
    
    void SetWorld(World* l_world) { m_world = l_world; }
    World* GetWorld() const { return m_world; }
    
    sf::Vector2f GetPosition() const { return m_shape.getPosition(); }
    void SetPosition(const sf::Vector2f& l_pos);
    
    bool CheckCollision(const sf::FloatRect& rect);
    
    bool IsMarkedForDeletion() const { return m_markedForDeletion; }
    void MarkForDeletion() { m_markedForDeletion = true; }
    
    void Shoot();

private:
    World* m_world = nullptr;
    sf::RectangleShape m_shape;
    std::unique_ptr<sf::Sprite> m_sprite;
    float m_speedMultiplier = 1.0f;
    bool m_markedForDeletion = false;
    
    float m_shootTimer = 0.f;
    float m_shootCooldown = 3.0f;
};
