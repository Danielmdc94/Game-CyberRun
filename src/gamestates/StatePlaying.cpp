#include "StatePlaying.h"
#include "StatePaused.h"
#include "StateMenu.h"
#include "StateGameOver.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include <memory>
#include <iostream>
#include <SFML/Graphics/RenderTarget.hpp>

StatePlaying::StatePlaying(StateStack& stateStack) : m_stateStack(stateStack)
{
    
}

StatePlaying::~StatePlaying()
{
    if (m_world)
    {
        m_world->OnDestroy();
        delete m_world;
        m_world = nullptr;
    }
}

bool StatePlaying::init()
{
    m_ground.setSize({1024.0f, 256.0f});
    m_ground.setPosition({0.0f, 800.0f});
    m_ground.setFillColor(sf::Color::Green);

    m_world = new World(this);
    if (m_world) 
    {
        m_world->OnCreate();
    }

    SetupHealthUI();
    SetupScoreUI();
    SetupSpeedUI();

    return true;
}

void StatePlaying::update(float dt)
{
	if (!m_world) 
    { 
        return; 
    }
	m_world->Update(sf::seconds(dt));

	if (!m_gameOverTriggered) 
    {
		if (auto* player = m_world->GetPlayer()) 
        {
			if (player->IsDead()) 
            {
				m_gameOverTriggered = true;
				GameOver();
				return;
			}
		}
	}
}

void StatePlaying::ResetPlayerKeys()
{
	if (m_world && m_world->GetPlayer())
	{
		m_world->GetPlayer()->ResetKeys();
	}
}

void StatePlaying::render(sf::RenderTarget& target) const
{
    m_world->Draw(&target);

    DrawHealthUI(target);
	DrawScoreUI(target);
	DrawSpeedUI(target);
}

void StatePlaying::SetupHealthUI()
{
    const sf::Texture* healthTexture = ResourceManager::getOrLoadTexture("Health.png");
    if (!healthTexture)
    {
        return;
    }
	m_healthSprite = std::make_unique<sf::Sprite>(*healthTexture);

	m_healthFrames.clear();
	unsigned frameWidth = 67;
	unsigned frameHeight = 60;
	for (unsigned i = 0; i < 2; ++i)
	{
		m_healthFrames.push_back(sf::IntRect(sf::Vector2i(static_cast<int>(i * frameWidth), 0), sf::Vector2i(static_cast<int>(frameWidth), static_cast<int>(frameHeight))));
	}
}
void StatePlaying::DrawHealthUI(sf::RenderTarget& target) const
{
	if (!m_healthSprite || m_healthFrames.empty() || !m_world)
    {
        return;
    }
		
	Player* player = m_world->GetPlayer();
	if (!player)
    {
        return;
    }

	float startX = 40.f;
	float startY = 40.f;
	float spacing = 80.f;
	
	int currentHealth = player->GetHealth();
	int maxHealth = player->GetMaxHealth();
	
	for (int i = 0; i < maxHealth; ++i)
	{
		int frameIndex = (i < currentHealth) ? 0 : 1;
		
		m_healthSprite->setTextureRect(m_healthFrames[frameIndex]);
		m_healthSprite->setPosition({startX + i * spacing, startY});

		target.draw(*m_healthSprite);
	}
}

void StatePlaying::SetupScoreUI()
{
	const sf::Font* font = ResourceManager::getOrLoadFont("GlitchGoblin.ttf");
	if (!font)
		return;
	
	m_scoreText = std::make_unique<sf::Text>(*font, "Score: 0", 48u);
	m_scoreText->setFillColor(sf::Color::Yellow);
	m_scoreText->setPosition({40.f, 120.f});
}

void StatePlaying::DrawScoreUI(sf::RenderTarget& target) const
{
	if (!m_scoreText || !m_world)
		return;
		
	Player* player = m_world->GetPlayer();
	if (!player)
		return;
	
	std::string scoreString = "Score: " + std::to_string(player->GetScore());
	m_scoreText->setString(scoreString);
	
	target.draw(*m_scoreText);
}

void StatePlaying::SetupSpeedUI()
{
	const sf::Font* font = ResourceManager::getOrLoadFont("GlitchGoblin.ttf");
	if (!font)
		return;
	
	m_speedText = std::make_unique<sf::Text>(*font, "Speed: 100", 32u);
	m_speedText->setFillColor(sf::Color::Cyan);
	m_speedText->setOutlineColor(sf::Color::Black);
	m_speedText->setOutlineThickness(2.f);
	m_speedText->setPosition({40.f, 200.f});
}

void StatePlaying::DrawSpeedUI(sf::RenderTarget& target) const
{
	if (!m_speedText || !m_world)
		return;
	
	std::string speedString = "Speed: " + std::to_string(static_cast<int>(m_world->GetWorldSpeed()));
	m_speedText->setString(speedString);

	target.draw(*m_speedText);
}

void StatePlaying::GameOver()
{
	int finalScore = 0;
	if (m_world && m_world->GetPlayer())
	{
		finalScore = m_world->GetPlayer()->GetScore();
		m_world->GetPlayer()->ResetKeys();
	}
	
	m_stateStack.push<StateGameOver>(finalScore);
}

void StatePlaying::Pause()
{

}
