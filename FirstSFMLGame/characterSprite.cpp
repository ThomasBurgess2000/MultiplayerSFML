#include "characterSprite.h"
#include "ResourceHolder.h"

sf::Font arial;




ResourceHolder<sf::Texture, Textures::ID> textures;

characterSprite::characterSprite() {
    xloc = 0;
    yloc = 0;
    name = "Default";
    displayName.setFont(arial);
    displayName.setString(name);
    displayName.setCharacterSize(17);
    sf::Sprite tempSprite(textures.get(Textures::playerTexture));
    sprite = tempSprite;
    setPosition(xloc, yloc);
}

characterSprite::characterSprite(string charName, float x, float y)
{
    name = charName;
    xloc = x;
    yloc = y;
    displayName.setFont(arial);
    displayName.setString(name);
    displayName.setCharacterSize(17);
    sf::Sprite tempSprite(textures.get(Textures::playerTexture));
    sprite = tempSprite;
    //sprite.setColor(sf::Color(0, 255, 0));
    setPosition(xloc, yloc);
}

void characterSprite::move(float x, float y)
{
    xloc = xloc + x;
    yloc = yloc + y;
    sprite.move(x, -y);
    displayName.move(x, -y);
}

void characterSprite::setPosition(float x, float y)
{
    xloc = x;
    yloc = y;
    sprite.setPosition(x, -y);
    displayName.setPosition(x + 18, -y + 90);
}