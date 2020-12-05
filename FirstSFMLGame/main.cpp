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

//Overload operators for packet to carry characterSprite objects

sf::Packet& operator <<(sf::Packet& packet, const characterSprite& character)
{
    return packet << character.name << character.xloc << character.yloc;
}

sf::Packet& operator >>(sf::Packet& packet, characterSprite& character)
{
    return packet >> character.name >> character.xloc >> character.yloc;
}

// Globals 
vector<characterSprite> players;
sf::UdpSocket clientSocket;
bool playersBeingAccessed = false;
sf::View view1;

// Server communication functions

void clientSend(sf::IpAddress recipient, unsigned short port, characterSprite player)
{
    
    sf::Packet packet;
    packet << player;
    if (clientSocket.send(packet, recipient, port) != sf::Socket::Done) {
        cout << "Did not send data." << endl;
    }
    /*else {
        cout << "Data successfully sent: " << player.xloc <<", " << player.yloc << endl;
    }*/
    
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
        players = localVarPlayers;
    }

    return;
}

// Other functions

void unencryptedMessage() {
    string messageString;
    cout << "Unencrypted message: ";
    
    cout << messageString;
    return;
}

// Main

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML");
    window.setVerticalSyncEnabled(true);
    view1.setCenter(sf::Vector2f(0, 0));
    view1.setSize(sf::Vector2f(1920, 1080));
    window.setView(view1);
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

    // Initialization of player
    textures.load(Textures::playerTexture, "PinkGhost.png");
    characterSprite player("Ninjinka", 100, -200);
    int lViewBoundary = view1.getCenter().x - (view1.getSize().x / 2);
    int rViewBoundary = view1.getCenter().x + (view1.getSize().x / 2);
    int uViewBoundary = view1.getCenter().y + (view1.getSize().y / 2);
    int dViewBoundary = view1.getCenter().y - (view1.getSize().y / 2);
    bool messagingMode = false;
    //Display
    while (window.isOpen())
    {
        char playerDirection;
        bool movementKeyPressed = false;
        sf::Event event;
        string mostRecentMessage;
        sf::Text mostRecentMessageDrawable;
        mostRecentMessageDrawable.setFont(arial);
        mostRecentMessageDrawable.setCharacterSize(15);
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                cout << "Window closed.";
                window.close();
                break;

            case sf::Event::KeyPressed:
                // MOVEMENT
                if (messagingMode == false)
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                        player.move(0.f, -10.f);
                        playerDirection = 'd';
                        movementKeyPressed = true;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                        player.move(0.f, 10.f);
                        playerDirection = 'u';
                        movementKeyPressed = true;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                        player.move(-10.f, 0.f);
                        playerDirection = 'l';
                        movementKeyPressed = true;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                        player.move(10.f, 0.f);
                        playerDirection = 'r';
                        movementKeyPressed = true;
                    }

                    if (movementKeyPressed)
                    {
                        // Send data whenever you move
                        clientSend(recipient, port, player);
                        // Set the current view boundaries
                        lViewBoundary = view1.getCenter().x - (view1.getSize().x / 2);
                        rViewBoundary = view1.getCenter().x + (view1.getSize().x / 2);
                        uViewBoundary = view1.getCenter().y + (view1.getSize().y / 2);
                        dViewBoundary = view1.getCenter().y - (view1.getSize().y / 2);
                        // If outside the view boundary, move the view
                        if (player.xloc < lViewBoundary) view1.move(-1920, 0);
                        if (player.xloc >= rViewBoundary) view1.move(1920, 0);
                        if (-player.yloc < dViewBoundary) view1.move(0, -1080);
                        if (-player.yloc >= uViewBoundary) view1.move(0, 1080);
                    }
                }

                // Messaging

                if (event.key.code == sf::Keyboard::U)
                {
                    messagingMode = true;
                }

                if (messagingMode == true) {
                    if (event.type == sf::Event::TextEntered)
                    {
                        if (event.text.unicode < 128) 
                        {
                            mostRecentMessage = mostRecentMessage + static_cast<char>(event.text.unicode);
                            mostRecentMessageDrawable.setString(mostRecentMessage);
                        }
                    }
                }

                
                break;

            default:
                break;
                    
            }
            
        }
        // Receive data before drawing the scene
        clientReceive(player);
        window.clear();
        window.setView(view1);

        for (auto& it : players)
        {
            window.draw(it.sprite);
            window.draw(it.displayName);
        }

        
        window.draw(player.sprite);
        window.draw(player.displayName);
        if (mostRecentMessage.empty() != true) {
            mostRecentMessageDrawable.setPosition(lViewBoundary + 960, dViewBoundary + 10);
            window.draw(mostRecentMessageDrawable);
        }
        window.display();
    }

    return 0;
}