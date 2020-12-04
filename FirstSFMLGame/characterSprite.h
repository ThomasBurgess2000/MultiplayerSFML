#pragma once

#include <SFML/Graphics.hpp>
#include <string>


using namespace std;

extern sf::Font arial;
extern sf::Texture playerTexture;


class characterSprite
{
    
public:
    string name;
    float xloc;
    float yloc;
    sf::Sprite sprite;
    sf::Text displayName;


    characterSprite();

    characterSprite(string charName, float x, float y);

    void move(float x, float y);

    void setPosition(float x, float y);
};