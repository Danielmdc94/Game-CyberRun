#pragma once

#include "Entity.h"
#include <memory>
#include <SFML/System/Angle.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf { class Sprite; class RenderWindow; }
class World;

class Boss final : public Entity
{
public:
    static constexpr float collisionRadius = 48.0f;
    static constexpr int maxHealth = 50;

    Boss();
    virtual ~Boss() = default;
    
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
    
    void TakeDamage(int damage);
    int GetHealth() const { return m_health; }
    
    void Shoot();

private:
    World* m_world = nullptr;
    sf::RectangleShape m_shape;
    std::unique_ptr<sf::Sprite> m_sprite;
    bool m_markedForDeletion = false;
    int m_health = maxHealth;
    
    sf::Vector2f m_targetPosition;
    float m_moveSpeed = 100.f;
    float m_moveTimer = 0.f;
    float m_moveChangeCooldown = 2.0f;
    bool m_hasEnteredScreen = false;
    float m_entryX = 1600.f;

    float m_shootTimer = 0.f;
    float m_shootCooldown = 1.5f;
    int m_burstCount = 0;
    int m_maxBurstShots = 3;
    float m_burstDelay = 0.3f;
    float m_burstTimer = 0.f;
};
