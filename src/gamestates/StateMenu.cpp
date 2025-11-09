#include "StateMenu.h"
#include "StatePlaying.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <memory>


StateMenu::StateMenu(StateStack& stateStack) : m_stateStack(stateStack)
{
    
}

bool StateMenu::init()
{

    const sf::Texture* backgroundTexture = ResourceManager::getOrLoadTexture("Background.png");
    if (!backgroundTexture)
    {
        return false;
    }
    m_backgroundSprite = std::make_unique<sf::Sprite>(*backgroundTexture);
    if (!m_backgroundSprite)
    {
        return false;
    }
    const sf::Font* pFont = ResourceManager::getOrLoadFont("GlitchGoblin.ttf");
    if (pFont == nullptr)
    {
        return false;
    }       
	m_textTitle = std::make_unique<sf::Text>(*pFont, "Cyber-Run", 128u);
    if (!m_textTitle)
    {
        return false;
    }
        
	m_textTitle->setStyle(sf::Text::Bold);
	m_textTitle->setOutlineColor(sf::Color::Black);
	m_textTitle->setOutlineThickness(4.f);
	m_textTitle->setFillColor(sf::Color::Yellow);
	sf::FloatRect textRect = m_textTitle->getLocalBounds();
	m_textTitle->setOrigin({textRect.position.x + textRect.size.x / 2.f, textRect.position.y  + textRect.size.y / 2.f});

	m_textContinue = std::make_unique<sf::Text>(*pFont, "PRESS <SPACE> TO CONTINUE", 32u);
    if (!m_textContinue)
    {
        return false;
    }
        
	m_textContinue->setStyle(sf::Text::Bold);
	m_textContinue->setOutlineColor(sf::Color::Black);
	m_textContinue->setOutlineThickness(2.f);
	m_textContinue->setFillColor(sf::Color::White);
	textRect = m_textContinue->getLocalBounds();
	m_textContinue->setOrigin({textRect.position.x + textRect.size.x / 2.f, textRect.position.y  + textRect.size.y / 2.f});

    return true;
}

void StateMenu::update(float dt)
{
    m_timePassed += dt;
    m_timerTextContinue += dt;

    if (m_timerTextContinue >= 1.f)
        m_timerTextContinue = 0.f;
}

void StateMenu::render(sf::RenderTarget& target) const
{
    sf::Vector2u windowSize = target.getSize();

    m_backgroundSprite->setScale({static_cast<float>(windowSize.x) / m_backgroundSprite->getTexture().getSize().x, static_cast<float>(windowSize.y) / m_backgroundSprite->getTexture().getSize().y});

    m_textTitle->setPosition({static_cast<float>(windowSize.x) / 2.f, static_cast<float>(windowSize.y) / 4.f});
    m_textContinue->setPosition({static_cast<float>(windowSize.x) / 2.f, static_cast<float>(windowSize.y) / 1.1f});

    target.draw(*m_backgroundSprite);
    target.draw(*m_textTitle);
    if (m_timerTextContinue <= 0.5f)
        target.draw(*m_textContinue);
}

void StateMenu::Continue()
{
    m_stateStack.push<StatePlaying>();
}
