#pragma once

#include "IState.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "../World.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class StatePlaying : public IState
{
public:
    StatePlaying(StateStack& stateStack);
    ~StatePlaying();

    bool init() override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

    World* GetWorld() { return m_world; }
    void ResetPlayerKeys();

private:
    StateStack& m_stateStack;

    World* m_world;

	std::unique_ptr<sf::Sprite> m_healthSprite;
	std::vector<sf::IntRect> m_healthFrames;
	
	std::unique_ptr<sf::Text> m_scoreText;
	std::unique_ptr<sf::Text> m_speedText;

    sf::RectangleShape m_ground;
    bool m_hasPauseKeyBeenReleased = true;
    bool m_gameOverTriggered = false;

    void updateCollisions();

    void SetupHealthUI();
	void DrawHealthUI(sf::RenderTarget& target) const;
	void SetupScoreUI();
	void DrawScoreUI(sf::RenderTarget& target) const;
	void SetupSpeedUI();
	void DrawSpeedUI(sf::RenderTarget& target) const;

    void GameOver();
    void Pause();
};
