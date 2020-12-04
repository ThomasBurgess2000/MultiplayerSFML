#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <map>
#include <thread>


using namespace std;

struct client
{
    sf::IpAddress ip;
    unsigned short port;
};


map <string, vector<float>> playerLocations;
//map <sf::IpAddress, unsigned short> clientsConnected;
vector<client> clientsConnected;
sf::UdpSocket socket;


bool operator==(const client& client1, const client& client2)
{
    if (client1.ip == client2.ip && client1.port == client2.port)
    {
        return true;
    }
    else {
        return false;
    }
}

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
        client tempClient;
        tempClient.ip = sender;
        tempClient.port = port;
        if (find(clientsConnected.begin(), clientsConnected.end(), tempClient) == clientsConnected.end())
        {
            clientsConnected.push_back(tempClient);
        }
        packet >> playerName >> xyCoord[0] >> xyCoord[1];
        playerLocations[playerName] = xyCoord;

        // Log location
        cout << playerName << " is at " << playerLocations[playerName][0] << ", " << playerLocations[playerName][1] << endl;

    }
    return;
}

void sendData()
{
    sf::Time t1 = sf::milliseconds(33);
    while (serverOn) {
        // Make a packet of player locations
        sf::Packet packet;
        packet << playerLocations.size();
        for (auto const& playerInfo : playerLocations) {
            // Format: number of players, playerName, playerNameX, playerNameY, playerName2...
            packet << playerInfo.first << playerInfo.second[0] << playerInfo.second[1];
        }
        // Send player locations to all connected clients
        for (auto const& clientInfo : clientsConnected) {
            sf::IpAddress recipient = clientInfo.ip;
            unsigned short port = clientInfo.port;
            if (socket.send(packet, recipient, port) != sf::Socket::Done) cout << "Error sending data to " << recipient << endl;
        }
        sf::sleep(t1);
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