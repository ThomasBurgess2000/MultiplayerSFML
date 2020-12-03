#include <SFML/Network.hpp>
#include <string>
#include <iostream>

using namespace std;



int main()
{
    sf::UdpSocket socket;

    if (socket.bind(50240) != sf::Socket::Done) {
        cout << "Server unable to bind to socket." << endl;
    }
    else {
        cout << "Server bound on port: " << socket.getLocalPort() << endl;
    }
    cout << "Server public IP is: " << sf::IpAddress::getPublicAddress() << endl;

    sf::IpAddress sender;
    unsigned short port;
    bool serverOn = true;
    sf::Packet packet;
    float servX, servY;
    int packetCounter=0;
    while (serverOn) {

        if (socket.receive(packet, sender, port) != sf::Socket::Done) cout << "Error receiving data.";
        cout << "Received a packet from " << sender << " on port " << port << endl;
        packetCounter++;
        packet >> servX >> servY;
        cout << servX << ", " << servY << endl;

    }
    return 0;
}