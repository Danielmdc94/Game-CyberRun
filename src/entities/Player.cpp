#include "Player.h"
#include "ResourceManager.h"
#include "../World.h"
#include "../MathUtils.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cmath>

Player::Player() : m_speed(200.f)
{

}

bool Player::init()
{
    const sf::Texture* texture = ResourceManager::getOrLoadTexture("Player.png");
    if (texture == nullptr)
    {
        return false;
    }

    m_sprite = std::make_unique<sf::Sprite>(*texture);
    if (!m_sprite)
    {
        return false;
    }

    m_sprite->setOrigin({35.f, 33.f});
    m_sprite->setPosition({200.f, 600.f});
    m_sprite->setScale({2.f, 2.f});
    
    SetupAnimations(71, 67);
    LoadSounds();

    m_shape.setSize({71.f, 100.f});
    m_shape.setFillColor({0, 255, 0, 40});
    m_shape.setOutlineColor(sf::Color::Green);
    m_shape.setOutlineThickness(1.f);
    m_shape.setOrigin(m_shape.getSize() * 0.5f);
    m_shape.setPosition({m_sprite->getPosition().x, m_sprite->getPosition().y + 16.f});
    
    m_position = m_shape.getPosition();
    m_collisionRadius = collisionRadius;

    return true;
}

void Player::SetupAnimations(unsigned l_frameWidth, unsigned l_frameHeight)
{
    m_framesRun.clear();
    for (unsigned i = 0; i < 8; ++i)
    {
        m_framesRun.push_back(sf::IntRect(sf::Vector2i(static_cast<int>(i * l_frameWidth), 0), sf::Vector2i(static_cast<int>(l_frameWidth), static_cast<int>(l_frameHeight))));
    }

    m_framesJump.clear();
    for (unsigned i = 0; i < 4; ++i)
    {
        m_framesJump.push_back(sf::IntRect(sf::Vector2i(static_cast<int>(i * l_frameWidth), static_cast<int>(l_frameHeight)), sf::Vector2i(static_cast<int>(l_frameWidth), static_cast<int>(l_frameHeight))));
    }

    if (!m_framesRun.empty() && m_sprite)
    {
        m_sprite->setTextureRect(m_framesRun[0]);
    }
}

void Player::update(float dt)
{
    if (m_currentHealth <= 0)
    {
        Die();
        return;
    }

    if (dt <= 0.f)
    {
        return;
    }
    
    if (m_shootTimer > 0.f)
    {
        m_shootTimer -= dt;
    }
    
    HandleDash(dt);
    HandleMove(dt);
    HandleJump(dt);
    HandleFall(dt);
    HandleCollisions(dt);
    HandleAnimations(dt);
}

void Player::render(sf::RenderTarget& target) const
{
    if (m_sprite)
    {
        target.draw(*m_sprite);
    }
}

void Player::HandleMove(float dt)
{
    if (m_isDashing)
    {
        return;
    }
    
    int axisX = (m_moveRight ? 1 : 0) - (m_moveLeft ? 1 : 0);

    float targetSpeed = m_maxSpeedX;
    if (m_running)
    {
        targetSpeed *= m_runSpeedMultiplier;
    }

    float targetVelocity = static_cast<float>(axisX) * targetSpeed;
    float acceleration;
    if (m_onGround)
    {
        acceleration = (axisX != 0) ? m_accelGround : m_decelGround;
    }
    else
    {
        acceleration = m_accelAir;
        if (axisX != 0)
        {
            if ((axisX > 0 && m_velocity.x > 0.f) || (axisX < 0 && m_velocity.x < 0.f))
            {
                acceleration *= 0.4f;
            }
        }
        else
        {
            acceleration *= 0.2f;
        }
    }
    
    m_velocity.x = Approach(m_velocity.x, targetVelocity, acceleration * dt);
}

void Player::HandleJump(float dt)
{
    if (m_isDashing) 
    {
        return;
    }

    const bool jumpPressedThisFrame = (m_jump && !m_prevJump);
    
    if (m_onGround)
    {
        m_coyoteTimer = m_coyoteTime;
        m_jumpCount = 0;
    }
    else
    {
        float previousCoyoteTimer = m_coyoteTimer;
        m_coyoteTimer = std::max(0.f, m_coyoteTimer - dt);
        
        if (previousCoyoteTimer > 0.f && m_coyoteTimer <= 0.f)
        {
            m_jumpCount++;
        }
    }
    
    if (jumpPressedThisFrame)
    {
        m_jumpBufferTimer = m_jumpBufferTime;
    }
    else
    {
        m_jumpBufferTimer = std::max(0.f, m_jumpBufferTimer - dt);
    }
    
    if (m_jumpBufferTimer > 0.f && (m_coyoteTimer > 0.f || m_jumpCount < m_maxJumps))
    {
        m_onGround = false;
        m_coyoteTimer = 0.f;
        m_jumpBufferTimer = 0.f;
        
        m_velocity.y = -m_jumpSpeed;

        if (m_jumpSound)
        {
            m_jumpSound->play();
        }
        
        m_isAnimJump = true;
        m_currentFrame = 0;
        m_frameTimer = 0.f;

        m_jumpCount++;
        m_canDash = true;
    }
}

void Player::HandleFall(float dt)
{
    if (m_isDashing) return;
    
    const bool jumpReleasedThisFrame = (!m_jump && m_prevJump);
    
    float gravity = m_gravity;
    if (m_velocity.y < 0.f)
    {
        if (!m_jump)
        {
            gravity *= m_jumpCutMultiplier;
        }
    }
    else
    {
        gravity *= m_fallGravityMultiplier;
    }
    m_velocity.y += gravity * dt;
    
    if (jumpReleasedThisFrame && m_velocity.y < 0.f)
    {
        m_velocity.y *= 0.6f;
    }
}

void Player::HandleDash(float dt)
{
    if (m_isDashing)
    {
        m_dashTimer -= dt;
        if (m_dashTimer <= 0.f)
        {
            m_isDashing = false;
            m_velocity = sf::Vector2f(m_velocity.x * 0.5f, 0.f);
            if (m_sprite)
            {
                m_sprite->setColor(sf::Color::White);
            }
            m_isAnimRun = true;
            m_dashRechargeTimer = m_dashCooldown;
        }
        else
        {
            if (m_sprite)
            {
                m_sprite->setColor(sf::Color::Yellow);
            }
            m_isAnimRun = false;

            sf::Vector2f velToMove = m_velocity;
            ResolveWorldCollisions(velToMove, dt);
            m_velocity = velToMove;
            
            m_jump = false;
            m_prevJump = false;
            m_coyoteTimer = 0.f;
            m_jumpBufferTimer = 0.f;
        }
    }
    else
    {
        if (m_dashRechargeTimer > 0.f)
        {
            m_dashRechargeTimer = std::max(0.f, m_dashRechargeTimer - dt);
        }
        if (m_onGround && m_dashRechargeTimer <= 0.f)
        {
            m_canDash = true;
        }
    }
}

void Player::HandleCollisions(float dt)
{
    sf::Vector2f velToMove = m_velocity;
    ResolveWorldCollisions(velToMove, dt);
    
    m_velocity = velToMove;
    m_prevJump = m_jump;
}

void Player::HandleAnimations(float dt)
{
    if (m_isAnimJump && !m_framesJump.empty())
    {
        m_frameTimer += dt;
        if (m_frameTimer >= m_frameTime)
        {
            m_frameTimer -= m_frameTime;
            if (m_currentFrame < 3)
            {
                m_currentFrame++;
            }
            if (m_sprite)
            {
                m_sprite->setTextureRect(m_framesJump[m_currentFrame]);
            }
        }
    }
    else if (m_isAnimRun && !m_framesRun.empty())
    {
        m_frameTimer += dt;
        if (m_frameTimer >= m_frameTime)
        {
            m_frameTimer -= m_frameTime;
            m_currentFrame = (m_currentFrame + 1) % m_framesRun.size();
            if (m_sprite)
            {
                m_sprite->setTextureRect(m_framesRun[m_currentFrame]);
            }
        }
    }
}

void Player::OnDashPressed()
{
    if (m_canDash && !m_isDashing)
    {
        m_isDashing = true;
        m_canDash = false;
        m_dashTimer = m_dashDuration;
        if (m_dashSound)
        {
            m_dashSound->stop();
            m_dashSound->setPlayingOffset(sf::seconds(0.15f));
            m_dashSound->play();
        }

        sf::Vector2f direction = {1.f, 0.f};
        
        if (m_moveLeft && !m_moveRight)
            direction = {-1.f, 0.f};
        else if (m_moveRight && !m_moveLeft)
            direction = {1.f, 0.f};

        m_velocity = direction * m_dashSpeed;
    }
}

void Player::ResetKeys()
{
    m_moveUp = false;
    m_moveDown = false;
    m_moveLeft = false;
    m_moveRight = false;
    m_jump = false;
    m_prevJump = false;
    m_running = false;
}

void Player::Shoot(const sf::Vector2f& l_mousePosition)
{
    if (m_shootTimer > 0.f)
    {
        return;
    }
    
    if (m_world)
    {
        sf::Vector2f playerPosition = GetPosition();
        sf::Vector2f gunPosition = {playerPosition.x + 45.f, playerPosition.y - 20.f};
        sf::Vector2f direction = l_mousePosition - gunPosition;
        
        m_world->SpawnBullet(gunPosition, direction);
    }
    
    if (m_shootSound)
    {
        m_shootSound->play();
    }
    
    m_shootTimer = m_shootCooldown;
}

void Player::LoadSounds()
{
    if (const sf::SoundBuffer* buffer = ResourceManager::getOrLoadSoundBuffer("Shoot.ogg"))
    {
        m_shootSound = std::make_unique<sf::Sound>(*buffer);
        m_shootSound->setVolume(30.f);
    }
    
    if (const sf::SoundBuffer* buffer = ResourceManager::getOrLoadSoundBuffer("Jump.ogg"))
    {
        m_jumpSound = std::make_unique<sf::Sound>(*buffer);
        m_jumpSound->setVolume(50.f);
    }
    
    if (const sf::SoundBuffer* buffer = ResourceManager::getOrLoadSoundBuffer("Explosion.ogg"))
    {
        m_explosionSound = std::make_unique<sf::Sound>(*buffer);
        m_explosionSound->setVolume(70.f);
    }

    if (const sf::SoundBuffer* buffer = ResourceManager::getOrLoadSoundBuffer("Hurt.ogg"))
    {
        m_hurtSound = std::make_unique<sf::Sound>(*buffer);
        m_hurtSound->setVolume(70.f);
        m_hurtSound->setPitch(1.2f);
    }
    if (const sf::SoundBuffer* buffer = ResourceManager::getOrLoadSoundBuffer("Dash.ogg"))
    {
        m_dashSound = std::make_unique<sf::Sound>(*buffer);
        m_dashSound->setVolume(50.f);
    }
}

void Player::EnemyKill()
{
    m_score += (m_world ? static_cast<int>(m_world->GetWorldSpeed()) : 100);
    if (m_explosionSound)
    {
        m_explosionSound->play();
    }
}

void Player::SetHealth(int l_health)
{
    m_currentHealth = std::clamp(l_health, 0, m_maxHealth);
    if (m_currentHealth <= 0)
    {
        Die();
    }
}

void Player::Die()
{
    m_isDead = true;
}

bool Player::CheckCollision(const sf::FloatRect& l_rect)
{
    return m_shape.getGlobalBounds().findIntersection(l_rect).has_value();
}

void Player::ResolveWorldCollisions(sf::Vector2f& l_velocity, float l_deltaTime)
{
    sf::Vector2f currentPos = m_shape.getPosition();
    sf::Vector2f newPos = currentPos + l_velocity * l_deltaTime;
    
    if (m_world)
    {
        float leftBound = 0.f;
        float rightBound = 1920.f;
        float topBound = -200.f;

        sf::FloatRect playerBounds = m_shape.getGlobalBounds();
        float halfWidth = playerBounds.size.x * 0.5f;
        float halfHeight = playerBounds.size.y * 0.5f;
        
        if (newPos.x - halfWidth < leftBound)
        {
            newPos.x = leftBound + halfWidth;
            l_velocity.x = 0.f;
        }
        else if (newPos.x + halfWidth > rightBound)
        {
            newPos.x = rightBound - halfWidth;
            l_velocity.x = 0.f;
        }
        
        if (newPos.y - halfHeight < topBound)
        {
            newPos.y = topBound + halfHeight;
            l_velocity.y = 0.f;
        }
        
        sf::FloatRect newPlayerBounds = playerBounds;
        newPlayerBounds.position = {newPos.x - halfWidth, newPos.y - halfHeight};
        
        const auto& platforms = m_world->GetPlatforms();
        const auto& floorSegments = m_world->GetFloorSegments();
        
        bool foundGroundCollision = false;
        
        for (const auto& segment : floorSegments) 
        {
            sf::FloatRect segmentBounds = segment.getGlobalBounds();
            
            if (newPlayerBounds.findIntersection(segmentBounds))
            {
                float playerBottom = currentPos.y + halfHeight;
                float segmentTop = segmentBounds.position.y;
                
                if (l_velocity.y >= 0.f && playerBottom <= segmentTop + 20.f)
                {
                    newPos.y = segmentTop - halfHeight;
                    l_velocity.y = 0.f;
                    m_onGround = true;
                    foundGroundCollision = true;
                    
                    if (m_isAnimJump) 
                    {
                        m_isAnimJump = false;
                        m_currentFrame = 0;
                        m_frameTimer = 0.f;
                    }
                    break;
                }
            }
        }
        
        if (!foundGroundCollision) 
        {
            for (const auto& platform : platforms) 
            {
                sf::FloatRect platformBounds = platform.getGlobalBounds();
                
                if (newPlayerBounds.findIntersection(platformBounds))
                {
                    float playerBottom = currentPos.y + halfHeight;
                    float platformTop = platformBounds.position.y;
                    
                    if (l_velocity.y >= 0.f && playerBottom <= platformTop + 20.f)
                    {
                        newPos.y = platformTop - halfHeight;
                        l_velocity.y = 0.f;
                        m_onGround = true;
                        foundGroundCollision = true;
                        
                        if (m_isAnimJump) 
                        {
                            m_isAnimJump = false;
                            m_currentFrame = 0;
                            m_frameTimer = 0.f;
                        }
                        break;
                    }
                }
            }
        }
        
        if (!foundGroundCollision) 
        {
            m_onGround = false;
        }
        
        if (newPos.y > 1080.f + 300.f) 
        {
            Die();
            return;
        }
        
        m_shape.setPosition(newPos);
        if (m_sprite)
        {
            m_sprite->setPosition({newPos.x, newPos.y - 16.f});
        }
        m_position = newPos;
    }
}
