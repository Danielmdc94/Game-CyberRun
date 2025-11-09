#include "StatePaused.h"
#include "StatePlaying.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

StatePaused::StatePaused(StateStack& stateStack) : m_stateStack(stateStack)
{
    
}

bool StatePaused::init()
{
    m_pPrevState = m_stateStack.getCurrentState();

    m_overlayRect.setSize({1920.f, 1080.f});
    m_overlayRect.setFillColor(sf::Color(0, 0, 0, 150));

    const sf::Font* pFont = ResourceManager::getOrLoadFont("GlitchGoblin.ttf");
    if (pFont == nullptr)
    {
        return false;
    }
    m_textTitle = std::make_unique<sf::Text>(*pFont, "PAUSED", 96u);
    if (!m_textTitle)
    {
        return false;
    }
    m_textTitle->setStyle(sf::Text::Bold);
	m_textTitle->setOutlineColor(sf::Color::Black);
	m_textTitle->setOutlineThickness(2.f);
	m_textTitle->setFillColor(sf::Color::Yellow);
	sf::FloatRect localBounds = m_textTitle->getLocalBounds();
	m_textTitle->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});


    m_textContinue = std::make_unique<sf::Text>(*pFont, "PRESS <Tab> TO UNPAUSE", 32u);
    if (!m_textContinue)
    {
        return false;
    }
    m_textContinue->setStyle(sf::Text::Bold);
	m_textContinue->setOutlineColor(sf::Color::Black);
	m_textContinue->setOutlineThickness(2.f);
	m_textContinue->setFillColor(sf::Color::White);
	localBounds = m_textContinue->getLocalBounds();
	m_textContinue->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});

    return true;
}

void StatePaused::update(float dt)
{
    m_timerTextContinue += dt;

    if (m_timerTextContinue >= 1.f)
        m_timerTextContinue = 0.f;
}

void StatePaused::render(sf::RenderTarget& target) const
{
    if (m_pPrevState != nullptr)
    {
        m_pPrevState->render(target);
    }
    sf::RectangleShape overlayRect = m_overlayRect;
    overlayRect.setSize({static_cast<float>(target.getSize().x), static_cast<float>(target.getSize().y)});
    target.draw(overlayRect);
    m_textTitle->setPosition({target.getSize().x / 2.f, target.getSize().y / 4.f});
    target.draw(*m_textTitle);
    m_textContinue->setPosition({target.getSize().x / 2.f, target.getSize().y / 1.5f});
    if (m_timerTextContinue <= 0.5f)
        target.draw(*m_textContinue);
}

void StatePaused::Unpause()
{
    StatePlaying* playingState = dynamic_cast<StatePlaying*>(const_cast<IState*>(m_pPrevState));
    if (playingState)
    {
        playingState->ResetPlayerKeys();
    }
    m_stateStack.popDeferred();
}
