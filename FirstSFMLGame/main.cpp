#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <map>
#include <algorithm>
#include "characterSprite.h"
#include "ResourceHolder.h"

using namespace std;

//Overload packet to carry characterSprite objects

sf::Packet& operator <<(sf::Packet& packet, const characterSprite& character)
{
    return packet << character.name << character.xloc << character.yloc;
}

sf::Packet& operator >>(sf::Packet& packet, characterSprite& character)
{
    return packet >> character.name >> character.xloc >> character.yloc;
}

// Globals 
bool serverReady = false;
bool multiplayerOn = true;
float servX, servY;
map <string, vector<float>> playerLocations;
vector<characterSprite> players;
sf::UdpSocket clientSocket;
bool playersBeingAccessed = false;


void clientSend(sf::IpAddress recipient, unsigned short port, characterSprite player)
{
    
    sf::Packet packet;
    packet << player;
    if (clientSocket.send(packet, recipient, port) != sf::Socket::Done) {
        cout << "Did not send data." << endl;
    }
    else {
        cout << "Data successfully sent." << endl;
    }
    
    return;
}

void clientReceive(characterSprite player) {
    sf::IpAddress sender;
    unsigned short port;
    sf::Packet packet;

    vector<characterSprite> localVarPlayers;
    int numberOfPlayers = 0;
    if (clientSocket.receive(packet, sender, port) == sf::Socket::Done)
    {
        packet >> numberOfPlayers;
        for (int i = 0; i < numberOfPlayers; i++)
        {
            string playerName;
            vector<float> xyCoord(2, 0);
            packet >> playerName >> xyCoord[0] >> xyCoord[1];
            if (playerName != player.name)
            {
                characterSprite tempPlayer(playerName, xyCoord[0], xyCoord[1]);
                tempPlayer.setPosition(xyCoord[0], xyCoord[1]);
                localVarPlayers.push_back(tempPlayer);
            }
        }
        if (playersBeingAccessed == false)
        {
            playersBeingAccessed = true;
            players = localVarPlayers;
            playersBeingAccessed = false;
        }
    }

    return;
}


// Resource ID for sf::Texture


int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML");
    window.setVerticalSyncEnabled(true);

    if (!arial.loadFromFile("C:/Windows/Fonts/arial.ttf")) cout << "Could not load font" << endl;

    //Client communication with server
    clientSocket.setBlocking(false);
    if (clientSocket.bind(50241) != sf::Socket::Done) {
        cout << "Client unable to bind to socket." << endl;
    }
    else {
        cout << "Client bound on socket: " << clientSocket.getLocalPort() << endl;
    }
    cout << "Client public IP is: " << sf::IpAddress::getPublicAddress() << endl;
    string serverIP = "174.82.86.186";
    sf::IpAddress recipient = serverIP;
    int serverPort = 50240;
    unsigned short port = serverPort;
    textures.load(Textures::playerTexture, "PinkGhost.png");
    characterSprite player("Ninjinka", 100, -200);
    vector<characterSprite> lastPlayerData;
    //Display
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                cout << "Window closed.";
                window.close();
                break;

            case sf::Event::KeyPressed:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) player.move(0.f, -10.f);
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) player.move(0.f, 10.f);
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) player.move(-10.f, 0.f);
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) player.move(10.f, 0.f);
                clientSend(recipient, port, player);
                break;

            default:
                break;
                    
            }
            
        }
        clientReceive(player);
        window.clear();
        if (playersBeingAccessed == false)
        {
            playersBeingAccessed = true;
            lastPlayerData = players;
            for (auto& it : players)
            {
                window.draw(it.sprite);
                window.draw(it.displayName);
            }
            playersBeingAccessed = false;
        }
        else {
            for (auto& it : lastPlayerData)
            {
                window.draw(it.sprite);
                window.draw(it.displayName);
            }
        }
        
        window.draw(player.sprite);
        window.draw(player.displayName);
        window.display();
    }

    return 0;
}