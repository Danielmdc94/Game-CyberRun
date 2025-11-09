#include "ParallaxLayer.h"
#include <iostream>
#include "ResourceManager.h"

bool ParallaxLayer::Init(const std::string& l_textureFile, float l_scrollSpeed, sf::Vector2u l_windowSize, float l_heightScale, float l_yPosition)
{
    const sf::Texture* texture = ResourceManager::getOrLoadTexture(l_textureFile);
    if (!texture) 
    {
        return false;
    }

    m_scrollSpeed = l_scrollSpeed;
    m_windowSize = l_windowSize; 

    sf::Vector2f textureSize = static_cast<sf::Vector2f>(texture->getSize());
    float screenHeight = static_cast<float>(l_windowSize.y);

    float targetHeight = screenHeight * (l_heightScale != 0.0f ? l_heightScale : 1.5f);
    m_scaleY = targetHeight / textureSize.y;

    float textureWidth = static_cast<float>(texture->getSize().x);
    m_scaledWidth = textureWidth * m_scaleY;
    
    int spritesNeeded = static_cast<int>(std::ceil(static_cast<float>(l_windowSize.x) / m_scaledWidth)) + 2;

    float yOffset;
    if (l_yPosition != 0.0f) 
    {
        m_yPosition = l_yPosition;
        yOffset = l_yPosition;
    } 
    else 
    {
        m_yPosition = 0.0f;
        yOffset = (static_cast<float>(l_windowSize.y) - targetHeight) * 0.5f;
    }
    
    float currentX = 0.f;
    float overlap = 1.0f;
    float step = m_scaledWidth - overlap;
    for (int i = 0; i < spritesNeeded; ++i) 
    {
        SpriteInstance instance;
        
        instance.sprite = std::make_unique<sf::Sprite>(*texture);
        instance.sprite->setScale({m_scaleY, m_scaleY});
        instance.xPosition = currentX;

        instance.sprite->setPosition({currentX, yOffset});
        
        currentX += step;
        m_sprites.push_back(std::move(instance));
    }

    return true;
}

void ParallaxLayer::Update(float l_deltaTime)
{
    if (m_sprites.empty()) 
    {
        return;
    }

    float yOffset;
    if (m_yPosition != 0.f) 
    {
        yOffset = m_yPosition;
    } 
    else 
    {
        float screenHeight = static_cast<float>(m_windowSize.y);
        float textureHeight = static_cast<float>(m_sprites.front().sprite->getTexture().getSize().y) * m_scaleY;
        yOffset = (screenHeight - textureHeight) * 0.5f;
    }

    yOffset += m_extraYOffset;
    
    for (SpriteInstance& spriteInstance : m_sprites) 
    {
        spriteInstance.xPosition -= m_scrollSpeed * l_deltaTime;
        spriteInstance.sprite->setPosition({spriteInstance.xPosition, yOffset});
    }
    
    SpriteInstance& leftmost = m_sprites.front();
    if (leftmost.xPosition + m_scaledWidth < 0.f) 
    {
        float rightmostX = m_sprites.back().xPosition;
        leftmost.xPosition = rightmostX + (m_scaledWidth - 1.0f);
    leftmost.sprite->setPosition({leftmost.xPosition, yOffset});
        
        m_sprites.push_back(std::move(leftmost));
        m_sprites.erase(m_sprites.begin());
    }
}

void ParallaxLayer::Draw(sf::RenderTarget& l_window)
{
    for (const auto& spriteInstance : m_sprites) 
    {
        if (spriteInstance.sprite) 
        {
            l_window.draw(*spriteInstance.sprite);
        }
    }
}