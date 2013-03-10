#include "CppUtilities/Network/ServerSocket.h"
#include "CppUtilities/Network/Socket.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>

using namespace etsai::cpputilities;
using namespace std;

void handler(shared_ptr<Socket> socket) {
    string line;
    cout << "request: " << socket->readLine() << endl;
    socket->write("server-achievements,1,response|0");

    while((line= socket->readLine()) != "") {
        cout << line << endl;
        socket->write("server-achievements,1,response|0|0,1,315;1,0,12;2,0,1;3,0,1;4,1,0;5,1,0;6,0,0;7,0,0;8,0,0;9,1,0;10,0,4;11,0,1;12,0,2;13,1,1200;14,0,0;15,0,0;16,0,0;17,0,315;18,0,315;19,0,0;20,0,0;21,0,1;22,1,0;23,0,3;24,0,0;25,0,0;26,0,1;27,0,0;28,0,0;29,0,0;30,0,0;31,0,0;32,0,0;33,0,0;34,0,0;35,0,0;36,0,0;37,0,0;38,0,0;39,0,0;40,0,0;41,0,0;42,0,0;43,0,0");
    }
}

int main(int argc, char **argv) {
    int port= atoi(argv[1]);
    ServerSocket server(port);

    cout << "Listening on port: " << port << endl;
    while(true) {
        shared_ptr<Socket> client= server.accept();
        cout << "Received connection from " << client->getAddress() << ":" << client->getPort() << endl;
        thread th(handler, client);
        th.detach();
    }

    return 0;
}
