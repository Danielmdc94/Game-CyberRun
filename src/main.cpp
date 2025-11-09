#include "ResourceManager.h"
#include "gamestates/StateStack.h"
#include "gamestates/IState.h"
#include "gamestates/StateMenu.h"
#include "gamestates/StatePlaying.h"
#include "gamestates/StatePaused.h"
#include "gamestates/StateGameOver.h"
#include <memory>
#include <stack>
#include <optional>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

void CreateWindow(sf::RenderWindow& window, bool fullscreen)
{
    sf::State state = fullscreen ? sf::State::Fullscreen : sf::State::Windowed;
    window.create(sf::VideoMode({1920, 1080}), "Runner", state);
    window.setKeyRepeatEnabled(false);
}

void HandleStateEvents(const sf::Event& event, IState* pState, StateStack& gamestates, sf::RenderWindow& window)
{
    StatePlaying* playingState = dynamic_cast<StatePlaying*>(pState);
    if (playingState && playingState->GetWorld())
    {
        Player* player = playingState->GetWorld()->GetPlayer();
        if (player)
        {
            if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::Space)
                    player->OnJumpPressed();
                else if (keyPressed->code == sf::Keyboard::Key::A)
                    player->OnMoveLeftPressed();
                else if (keyPressed->code == sf::Keyboard::Key::D)
                    player->OnMoveRightPressed();
                else if (keyPressed->code == sf::Keyboard::Key::LShift)
                    player->OnRunPressed();
                else if (keyPressed->code == sf::Keyboard::Key::E)
                    player->OnDashPressed();
                else if (keyPressed->code == sf::Keyboard::Key::Tab)
                {
                    player->ResetKeys();
                    gamestates.push<StatePaused>();
                }
            }
            if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>())
            {
                if (keyReleased->code == sf::Keyboard::Key::Space)
                    player->OnJumpReleased();
                else if (keyReleased->code == sf::Keyboard::Key::A)
                    player->OnMoveLeftReleased();
                else if (keyReleased->code == sf::Keyboard::Key::D)
                    player->OnMoveRightReleased();
                else if (keyReleased->code == sf::Keyboard::Key::LShift)
                    player->OnRunReleased();
            }
            if (const auto* mouseButtonPressed = event.getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);
                    player->Shoot(mouseWorldPos);
                }
            }
        }
    }

    StateMenu* menuState = dynamic_cast<StateMenu*>(pState);
    if (menuState)
    {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::Space)
                menuState->Continue();
        }
    }

    StateGameOver* gameOverState = dynamic_cast<StateGameOver*>(pState);
    if (gameOverState)
    {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::Space)
                gameOverState->Continue();
        }
    }
    StatePaused* pauseState = dynamic_cast<StatePaused*>(pState);
    if (pauseState)
    {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::Tab)
                pauseState->Unpause();
        }
    }
}

int main(int argc, char* argv[])
{
    (void)argc;

    // ResourceManager must be instantiated here -- DO NOT CHANGE
    ResourceManager::init(argv[0]);

    bool isFullscreen = true;
    sf::RenderWindow window;
    CreateWindow(window, isFullscreen);
    window.setFramerateLimit(60);

    StateStack gamestates;
    if (!gamestates.push<StateMenu>())
        return -1;

    sf::Music music;
    if (ResourceManager::openMusic("Dystopian.ogg", music))
    {
        music.setLooping(true);
        music.setVolume(70.f); 
        music.play();
    }

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Time elapsedTime = clock.restart();

        IState* pState = gamestates.getCurrentState();
        if (!pState) return -1;

        while (const std::optional<sf::Event> eventOpt = window.pollEvent())
        {
            const sf::Event& event = *eventOpt;
            if (const auto* closed = event.getIf<sf::Event::Closed>())
            {
                return 0;
            }
            if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::F4)
                {
                    window.close();
                    isFullscreen = !isFullscreen;
                    CreateWindow(window, isFullscreen);
                }
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                {
                    return 0;
                }
            }
            
            HandleStateEvents(event, pState, gamestates, window);
        }

        pState->update(elapsedTime.asSeconds());
        window.clear(sf::Color::Black);
        pState->render(window);
        window.display();

        gamestates.performDeferredPops();
    }
    
    return 0;
}
