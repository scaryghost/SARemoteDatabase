#include "CppUtilities/Cli/cli.h"
#include "CppUtilities/Network/ServerSocket.h"
#include "CppUtilities/Network/Socket.h"

#include "Database/Connection.h"
#include "Database/src/SqliteConnection.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>

using namespace etsai::cpputilities;
using namespace etsai::saremotedatabase;
using namespace std;

Connection *dbConn;

void handler(shared_ptr<Socket> socket) {
    string line;
    cout << "request: " << socket->readLine() << endl;
    socket->write("server-achievements,1,response|0");

    while((line= socket->readLine()) != "") {
        cout << line << endl;
        socket->write("server-achievements,1,response|0|" + dbConn->retrieveAchievementData("12345","blah"));
    }
}

void initDbConnection(const string& dbURL, const string& user, const string& passwd) {
    dbConn= new SqliteConnection();
    dbConn->open(dbURL, user, passwd);
}

int main(int argc, char **argv) {
    ServerSocket server;
    int port;
    string dbURL;

    CliBuilder *cli= CliBuilder::getBuilder();
    (*cli).addOption(Option("-tcpport", 0, 1, [&port](const Arguments &args) -> void {
        port= args.asInteger(0);
    }).withArgName("number").withDescription("TPC port to listen on for requests").withRequired(true))
    .addOption(Option("-database", 0, 1, [&dbURL](const Arguments &args) -> void {
        dbURL= args.asString(0);
    }).withArgName("url").withDescription("Database storing achievement information").withRequired(true))
    .addOption(Option("-h", [&cli](const Arguments &args) -> void {
        cli->displayUsage();
    }).withLongOpt("--help").withDescription("Displays this help message and exits"))
    .setUsage("saremotedatabase -database <name> -tcpport <number> [options]");
    cli->parse(argc, argv);

    server.bind(port);
    cout << "Listening on port: " << port << endl;
    while(true) {
        shared_ptr<Socket> client= server.accept();
        cout << "Received connection from " << client->getAddress() << ":" << client->getPort() << endl;
        thread th(handler, client);
        th.detach();
    }

    return 0;
}
