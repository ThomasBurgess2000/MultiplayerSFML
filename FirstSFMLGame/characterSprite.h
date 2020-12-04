#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "ResourceHolder.h"


using namespace std;

namespace Textures
{
    enum ID
    {
        playerTexture,
    };
}

extern sf::Font arial;
extern sf::Texture playerTexture;
//Try to load resources
extern ResourceHolder<sf::Texture, Textures::ID> textures;

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