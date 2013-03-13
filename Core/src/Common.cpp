#include "Core/Common.h"
#include "Database/src/SqliteConnection.h"
#include "Net/TcpListener.h"

#include "CppUtilities/Network/ServerSocket.h"
#include "CppUtilities/Network/Socket.h"

#include <chrono>
#include <memory>
#include <thread>

namespace etsai {
namespace saremotedatabase {
namespace common {

using namespace std::chrono;
using std::shared_ptr;
using std::thread;

shared_ptr<Logger> logger;
shared_ptr<Connection> dbConn;

void start(int port, const string& password) {
    ServerSocket server;
    stringstream msg;

    msg << "Listening on port: " << port;
    common::logger->log(Level::INFO, msg.str());
    server.bind(port);
    while(true) {
        shared_ptr<Socket> client(server.accept());
        shared_ptr<time_point<system_clock> > lastActiveTime(new time_point<system_clock>);

        common::logger->log(Level::INFO, "Received connection from " + client->getAddressPort());
        thread th(tcplistener::handler, client, lastActiveTime, password);
        thread th2(tcplistener::timeout, client, lastActiveTime);
        th.detach();
        th2.detach();
    }
}

void initDbConnection(const string& dbURL, const string& dbUser, const string& dbPasswd) {
    common::dbConn.reset(new SqliteConnection());
    common::dbConn->open(dbURL, dbUser, dbPasswd);
}

}
}
}
