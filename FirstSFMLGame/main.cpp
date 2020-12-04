#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <map>
#include <algorithm>

using namespace std;

// Global needed for class
sf::Font arial;

// Classes

class characterSprite
{
public:
    string name;
    float xloc;
    float yloc;
    float height;
    float width;
    float heightPx = height * 100;
    float widthPx = width * 100;
    sf::Text displayName;
    sf::RectangleShape charRec;

    characterSprite() {
        height = 0.75;
        width = 0.25;
        widthPx = width * 100;
        heightPx = height * 100;
        xloc = 0;
        yloc = 0;
        name = "Default";
        sf::RectangleShape tempRec(sf::Vector2f(widthPx, heightPx));
        charRec = tempRec;
        sf::Text tempText;
        tempText.setFont(arial);
        tempText.setString(name);
        tempText.setCharacterSize(width * 100);
        displayName = tempText;
    }

    characterSprite(string charName, float x, float y)
    {
        height = 0.75;
        width = 0.25;
        name = charName;
        widthPx = width * 100;
        heightPx = height * 100;
        xloc = x;
        yloc = y;
        sf::RectangleShape tempRec(sf::Vector2f(widthPx, heightPx));
        charRec = tempRec;
        sf::Text tempText;
        tempText.setFont(arial);
        tempText.setString(name);
        tempText.setCharacterSize(width * 100);
        displayName = tempText;
    }

    characterSprite(string charName, float charHeight, float charWidth, float x, float y)
    {
        height = charHeight;
        width = charWidth;
        name = charName;
        widthPx = width * 100;
        heightPx = height * 100;
        xloc = x;
        yloc = y;
        sf::RectangleShape tempRec(sf::Vector2f(widthPx, heightPx));
        charRec = tempRec;
        sf::Text tempText;
        tempText.setFont(arial);
        tempText.setString(name);
        tempText.setCharacterSize(width * 100);
        displayName = tempText;
        setPosition(xloc, yloc);
    }

    void move(float x, float y)
    {
        xloc = xloc + x;
        yloc = yloc + y;
        charRec.move(x, -y);
        displayName.move(x, -y);
    }

    void setPosition(float x, float y)
    {
        xloc = x;
        yloc = y;
        charRec.setPosition(x, -y);
        displayName.setPosition(x - 32, -y - 40);
    }
};

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
characterSprite player("Bob", 0.75, 0.25, 100, -200);
bool multiplayerOn = true;
float servX, servY;
map <string, vector<float>> playerLocations;
vector<characterSprite> players;
sf::UdpSocket clientSocket;
bool playersBeingAccessed = false;

void clientSend(sf::IpAddress recipient, unsigned short port)
{

    
    // Send data if location changed
    bool locationChanged = true;
    float prevXY = player.xloc + player.yloc;
    while (multiplayerOn)
    {
        if (prevXY != player.xloc + player.yloc) locationChanged = true;
        if (locationChanged) {
            sf::Packet packet;
            packet << player;
            if (clientSocket.send(packet, recipient, port) != sf::Socket::Done) {
                cout << "Did not send data." << endl;
            }
            else {
                cout << "Data successfully sent." << endl;
            }
            locationChanged = false;
            prevXY = player.xloc + player.yloc;
        }
    }
    return;
}

void clientReceive() {
    sf::IpAddress sender;
    unsigned short port;
    sf::Packet packet;
    while (multiplayerOn)
    {
        vector<characterSprite> localVarPlayers;
        int numberOfPlayers = 0;
        if (clientSocket.receive(packet, sender, port) != sf::Socket::Done) cout << "Error receiving data.";
        packet >> numberOfPlayers;
        for (int i = 0; i < numberOfPlayers; i++)
        {
            string playerName;
            vector<float> xyCoord(2, 0);
            packet >> playerName >> xyCoord[0] >> xyCoord[1];
            characterSprite tempPlayer(playerName, xyCoord[0], xyCoord[1]);
            tempPlayer.setPosition(xyCoord[0], xyCoord[1]);
            if (tempPlayer.name != player.name)
            {
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

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML");
    window.setVerticalSyncEnabled(true);
    if (!arial.loadFromFile("arial.ttf")) cout << "Could not load font" << endl;
    
    //Client communication with server
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

    thread clientSendThread(clientSend,recipient,port);
    thread clientReceiveThread(clientReceive);

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
                break;

            default:
                break;
                    
            }
            
        }

        window.clear();
        if (playersBeingAccessed == false)
        {
            playersBeingAccessed = true;
            for (auto& it : players)
            {
                window.draw(it.charRec);
                window.draw(it.displayName);
            }
            playersBeingAccessed = false;
        }
        
        window.draw(player.charRec);
        window.draw(player.displayName);
        window.display();
    }

    return 0;
}