#pragma once

#include "Entity.h"
#include <memory>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf { class Sprite; class RenderWindow; }
class World;

enum class PowerUpType
{
    Health
};

class PowerUp final : public Entity
{
public:
    PowerUp();
    virtual ~PowerUp() = default;
    
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
    
    void SetType(PowerUpType type) { m_type = type; }
    PowerUpType GetType() const { return m_type; }

private:
    World* m_world = nullptr;
    sf::RectangleShape m_shape;
    std::unique_ptr<sf::Sprite> m_sprite;
    PowerUpType m_type = PowerUpType::Health;
    bool m_markedForDeletion = false;
};
