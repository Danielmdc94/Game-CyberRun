#pragma once

#include "IState.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <memory>

class StateStack;

class StateGameOver : public IState
{
public:
    StateGameOver(StateStack& stateStack, int finalScore);
    
    bool init() override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

    void Continue();

private:
    StateStack& m_stateStack;
    int m_finalScore;
    int m_highScore;
    
    std::unique_ptr<sf::Sprite> m_backgroundSprite;
    std::unique_ptr<sf::Text> m_gameOverText;
    std::unique_ptr<sf::Text> m_scoreText;
    std::unique_ptr<sf::Text> m_highScoreText;
    std::unique_ptr<sf::Text> m_continueText;

    float m_timePassed = 0.f;
	float m_timerTextContinue = 0.f;
    
    void LoadHighScore();
    void SaveHighScore();
};
