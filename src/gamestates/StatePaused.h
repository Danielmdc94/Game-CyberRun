#pragma once

#include "IState.h"
#include <memory>
#include <SFML/Graphics/RectangleShape.hpp>

namespace sf { class Text; };

class StatePaused : public IState
{
public:
    StatePaused(StateStack& stateStack);
    ~StatePaused() = default;

    bool init() override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

    void Unpause();

private:
    StateStack& m_stateStack;
    const IState* m_pPrevState = nullptr;

    std::unique_ptr<sf::Text> m_textTitle;
    std::unique_ptr<sf::Text> m_textContinue;
    sf::RectangleShape m_overlayRect;

	float m_timerTextContinue = 0.f;
};
