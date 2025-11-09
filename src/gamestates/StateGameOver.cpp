#include "StateGameOver.h"
#include "StateMenu.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <fstream>
#include <algorithm>

StateGameOver::StateGameOver(StateStack& stateStack, int finalScore) : m_stateStack(stateStack), m_finalScore(finalScore), m_highScore(0)
{

}

bool StateGameOver::init()
{
    LoadHighScore();

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
    m_backgroundSprite->setColor(sf::Color(255, 255, 255, 150));
        
    
    if (m_finalScore > m_highScore)
    {
        m_highScore = m_finalScore;
        SaveHighScore();
    }
    
    const sf::Font* font = ResourceManager::getOrLoadFont("GlitchGoblin.ttf");
    if (!font)
        return false;
    
    m_gameOverText = std::make_unique<sf::Text>(*font, "GAME OVER", 256u);
    m_gameOverText->setFillColor(sf::Color::Red);
    m_gameOverText->setOutlineColor(sf::Color::Black);
    m_gameOverText->setOutlineThickness(4.f);
    sf::FloatRect gameOverBounds = m_gameOverText->getLocalBounds();
    m_gameOverText->setOrigin({gameOverBounds.size.x / 2.f, gameOverBounds.size.y / 2.f});
    
    m_scoreText = std::make_unique<sf::Text>(*font, "Score: " + std::to_string(m_finalScore), 64u);
    m_scoreText->setFillColor(sf::Color::Yellow);
    m_scoreText->setOutlineColor(sf::Color::Black);
    m_scoreText->setOutlineThickness(3.f);
    sf::FloatRect scoreBounds = m_scoreText->getLocalBounds();
    m_scoreText->setOrigin({scoreBounds.size.x / 2.f, scoreBounds.size.y / 2.f});
    
    std::string highScoreLabel = (m_finalScore >= m_highScore) ? "NEW HIGH SCORE: " : "High Score: ";
    m_highScoreText = std::make_unique<sf::Text>(*font, highScoreLabel + std::to_string(m_highScore), 64u);
    sf::Color highScoreColor = (m_finalScore >= m_highScore) ? sf::Color::Green : sf::Color::Cyan;
    m_highScoreText->setFillColor(highScoreColor);
    m_highScoreText->setOutlineColor(sf::Color::Black);
    m_highScoreText->setOutlineThickness(3.f);
    sf::FloatRect highScoreBounds = m_highScoreText->getLocalBounds();
    m_highScoreText->setOrigin({highScoreBounds.size.x / 2.f, highScoreBounds.size.y / 2.f});
    
    m_continueText = std::make_unique<sf::Text>(*font, "Press <SPACE> to return to menu", 32u);
    m_continueText->setFillColor(sf::Color::White);
    m_continueText->setOutlineColor(sf::Color::Black);
    m_continueText->setOutlineThickness(2.f);
    sf::FloatRect continueBounds = m_continueText->getLocalBounds();
    m_continueText->setOrigin({continueBounds.size.x / 2.f, continueBounds.size.y / 2.f});
    
    return true;
}

void StateGameOver::update(float dt)
{
    m_timePassed += dt;
    m_timerTextContinue += dt;

    if (m_timerTextContinue >= 1.f)
        m_timerTextContinue = 0.f;
}

void StateGameOver::render(sf::RenderTarget& target) const
{
    sf::Vector2u windowSize = target.getSize();

    if (m_backgroundSprite)
    {
        m_backgroundSprite->setScale({static_cast<float>(windowSize.x) / m_backgroundSprite->getTexture().getSize().x, static_cast<float>(windowSize.y) / m_backgroundSprite->getTexture().getSize().y});
        target.draw(*m_backgroundSprite);
    }
    if (m_gameOverText && m_scoreText && m_highScoreText && m_continueText)
    {
        m_gameOverText->setPosition({static_cast<float>(windowSize.x) / 2.f, static_cast<float>(windowSize.y) / 5.f});
        m_scoreText->setPosition({static_cast<float>(windowSize.x) / 2.f, static_cast<float>(windowSize.y) / 2.f});
        m_highScoreText->setPosition({static_cast<float>(windowSize.x) / 2.f, static_cast<float>(windowSize.y) / 1.6f});
        m_continueText->setPosition({static_cast<float>(windowSize.x) / 2.f, static_cast<float>(windowSize.y) / 1.1f});

        target.draw(*m_gameOverText);
        target.draw(*m_scoreText);
        target.draw(*m_highScoreText);
        if (m_timerTextContinue <= 0.5f)
            target.draw(*m_continueText);
    }
}

void StateGameOver::LoadHighScore()
{
    std::ifstream file("highscore.dat");
    if (file.is_open())
    {
        file >> m_highScore;
        file.close();
    }
    else
    {
        // My High Score testing run :D
        m_highScore = 11751;
    }
}

void StateGameOver::SaveHighScore()
{
    std::ofstream file("highscore.dat");
    if (file.is_open())
    {
        file << m_highScore;
        file.close();
    }
}

void StateGameOver::Continue()
{
    m_stateStack.popDeferred();
    m_stateStack.popDeferred();
    m_stateStack.popDeferred();
    m_stateStack.push<StateMenu>();
}
