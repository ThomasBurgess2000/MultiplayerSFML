#include "game.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <map>
#include <algorithm>
#include "characterSprite.h"
#include "ResourceHolder.h"
#include <iostream>

Game::Game() : window(sf::VideoMode(1920, 1080), "Ktema")
{
    window.setVerticalSyncEnabled(true);
    view1.setCenter(sf::Vector2f(0, 0));
    view1.setSize(sf::Vector2f(1920, 1080));
    window.setView(view1);
    if (!arial.loadFromFile("C:/Windows/Fonts/arial.ttf")) cout << "Could not load font" << endl;
}

void Game::run()
{
    while (window.isOpen())
    {
        processEvents();
        update();
        render();
    }
}
