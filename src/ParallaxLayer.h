#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include <string>
#include <memory>
#include <vector>

class World;

class ParallaxLayer
{
public:
    ParallaxLayer(World* l_world) : m_world(l_world) {}
    ~ParallaxLayer() = default;

    bool Init(const std::string& l_textureFile, float l_scrollSpeed, sf::Vector2u l_windowSize = {1280, 720}, float l_heightScale = 1.0f, float l_yPosition = 0.0f);

    void Update(float l_deltaTime);
    void SetScrollSpeed(float l_scrollSpeed) { m_scrollSpeed = l_scrollSpeed; }
    void SetExtraYOffset(float l_offset) { m_extraYOffset = l_offset; }

    void Draw(sf::RenderTarget& l_window);

private:
    struct SpriteInstance {
        std::unique_ptr<sf::Sprite> sprite;
        float xPosition;
    };

    std::vector<SpriteInstance> m_sprites;

    World* m_world = nullptr;
    float m_scrollSpeed = 0.f;
    float m_scaledWidth = 0.f;
    float m_scaleY = 1.f;
    float m_yPosition = 0.f;
    float m_extraYOffset = 0.f;
    sf::Vector2u m_windowSize;
};
