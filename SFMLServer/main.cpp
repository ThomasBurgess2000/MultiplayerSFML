#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <map>
#include <thread>

using namespace std;

map <string, vector<float>> playerLocations;
map <sf::IpAddress, unsigned short> clientsConnected;
sf::UdpSocket socket;

bool serverOn = true;

void receiveData()
{
    while (serverOn) {
        // Client data
        sf::IpAddress sender;
        unsigned short port;
        sf::Packet packet;

        // Player data
        vector<float> xyCoord(2, 0);
        string playerName;

        // Receive and store data
        if (socket.receive(packet, sender, port) != sf::Socket::Done) cout << "Error receiving data.";
        clientsConnected[sender] = port;
        packet >> playerName >> xyCoord[0] >> xyCoord[1];
        playerLocations[playerName] = xyCoord;

        // Log location
        cout << playerName << " is at " << playerLocations[playerName][0] << ", " << playerLocations[playerName][1] << endl;

    }
    return;
}

void sendData()
{
   
    while (serverOn) {
        // Make a packet of player locations
        sf::Packet packet;
        for (auto const& playerInfo : playerLocations) {
            // Format: number of players, playerName, playerNameX, playerNameY, playerName2...
            packet << playerLocations.size() << playerInfo.first << playerInfo.second[0] << playerInfo.second[1];
        }
        // Send player locations to all connected clients
        for (auto const& clientInfo : clientsConnected) {
            sf::IpAddress recipient = clientInfo.first;
            unsigned short port = clientInfo.second;
            if (socket.send(packet, recipient, port) != sf::Socket::Done) cout << "Error sending data to " << recipient << endl;
        }
    }
}

int main()
{
    // Startup server



    if (socket.bind(50240) != sf::Socket::Done) {
        cout << "Server unable to bind to socket." << endl;
    }
    else {
        cout << "Server bound on port: " << socket.getLocalPort() << endl;
    }
    cout << "Server public IP is: " << sf::IpAddress::getPublicAddress() << endl;

    // Start receiving data
    thread receiveThread(receiveData);
    
    // Start sending data
    thread sendThread(sendData);
    while (serverOn);
    return 0;
}