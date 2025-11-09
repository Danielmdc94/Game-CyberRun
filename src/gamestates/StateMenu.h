#pragma once

#include "IState.h"
#include <memory>

namespace sf { class Text; class Sprite; };

class StateMenu : public IState
{
public:
    StateMenu(StateStack& stateStack);
    ~StateMenu() = default;

    bool init() override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

    void Continue();

public:
    StateStack& m_stateStack;

    std::unique_ptr<sf::Text> m_textTitle;
	std::unique_ptr<sf::Text> m_textContinue;

	std::unique_ptr<sf::Sprite> m_backgroundSprite;

	float m_timerTextContinue = 0.f;
};
