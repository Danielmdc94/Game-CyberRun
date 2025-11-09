#pragma once

#include "Entity.h"
#include <memory>
#include <SFML/Graphics/RectangleShape.hpp>

namespace sf { class RenderWindow; class RenderTarget; }
class World;

class Bullet final : public Entity
{
public:
    Bullet();
    virtual ~Bullet() = default;
    
    bool init() override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;
    
    void SetWorld(World* l_world) { m_world = l_world; }
    World* GetWorld() const { return m_world; }
    
    sf::Vector2f GetPosition() const { return m_shape.getPosition(); }
    void SetPosition(const sf::Vector2f& l_pos);
    
    void SetVelocity(const sf::Vector2f& l_velocity) { m_velocity = l_velocity; }
    sf::Vector2f GetVelocity() const { return m_velocity; }
    
    sf::FloatRect GetBounds() const { return m_shape.getGlobalBounds(); }
    bool CheckCollision(const sf::FloatRect& rect);
    
    bool IsMarkedForDeletion() const { return m_markedForDeletion; }
    void MarkForDeletion() { m_markedForDeletion = true; }

private:
    World* m_world = nullptr;
    sf::RectangleShape m_shape;
    sf::Vector2f m_velocity;
    bool m_markedForDeletion = false;
};
