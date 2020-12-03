#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <thread>

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

    characterSprite(string charName, float charHeight, float charWidth)
    {
        height = charHeight;
        width = charWidth;
        name = charName;
        widthPx = width * 100;
        heightPx = height * 100;
        xloc = 0;
        yloc = 0;
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

// Globals 
bool serverReady = false;
characterSprite player("Ninjinka", 0.75, 0.25, 100, -200);
bool multiplayerOn = true;
float servX, servY;
/*
void renderWindow() {
    sf::RenderWindow serverWindow(sf::VideoMode(1280, 720), "Server");
    serverWindow.setPosition(sf::Vector2i(10, 50));
    sf::CircleShape circle(20.f);
    while (serverWindow.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (serverWindow.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                serverWindow.close();
        }
        circle.setPosition(servX, -servY);
        serverWindow.clear(sf::Color::Black);
        serverWindow.draw(circle);
        serverWindow.display();
    }
    return;
}

void server()
{
    thread serverWindowRenderThread (renderWindow);
    sf::UdpSocket socket;
    size_t received;
    char data[100];
    if (socket.bind(50240) != sf::Socket::Done) {
        cout << "Server unable to bind to socket." << endl;
    }
    else {
        cout << "Server bound on port: " << socket.getLocalPort() << endl;
    }
    cout << "Server public IP is: " << sf::IpAddress::getPublicAddress() << endl;
    //cout << "Enter the client IP: ";
    //string clientIP = "174.82.86.186";
    //cin >> clientIP;
    //sf::IpAddress sender = clientIP;
    sf::IpAddress sender;
    //cout << "Enter the client port: ";
    //int clientPort = 50241;
    //cin >> clientPort;
    //unsigned short port = clientPort;
    unsigned short port;
    serverReady = true;
    
    
    while (multiplayerOn) {
        sf::Packet packet;
        if (socket.receive(packet, sender, port) != sf::Socket::Done)
        {
            cout << "Didn't receieve client data" << endl;
        }
        cout << "Received a packet from " << sender << " on port " << port << endl;
        // Server window
        
        packet >> servX >> servY;
        

    }
    return;
}
*/


void client()
{

    sf::UdpSocket clientSocket;
    if (clientSocket.bind(50241) != sf::Socket::Done) {
        cout << "Client unable to bind to socket." << endl;
    }
    else {
        cout << "Client bound on socket: " << clientSocket.getLocalPort() << endl;
    }
    cout << "Client public IP is: " << sf::IpAddress::getPublicAddress() << endl;
    //cout << "Enter the server IP: ";
    string serverIP = "174.82.86.186";
    //cin >> serverIP;
    sf::IpAddress recipient = serverIP;
    //cout << "Enter the server port: ";
    int serverPort = 50240;
    //cin >> serverPort;
    unsigned short port = serverPort;
    // Send data if location changed
    bool locationChanged = true;
    float prevXY = player.xloc + player.yloc;
    while (multiplayerOn)
    {
        if (prevXY != player.xloc + player.yloc) locationChanged = true;
        if (locationChanged) {
            sf::Packet packet;
            packet << player.xloc << player.yloc;
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

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML");
    window.setVerticalSyncEnabled(true);
    if (!arial.loadFromFile("arial.ttf")) cout << "Could not load font" << endl;
    
    //Client communication with server
    thread clientThread(client);

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
        window.draw(player.charRec);
        window.draw(player.displayName);
        window.display();
    }

    return 0;
}