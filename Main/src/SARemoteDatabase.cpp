#include "CppUtilities/Cli/cli.h"
#include "CppUtilities/Logging/logging.h"
#include "CppUtilities/Network/ServerSocket.h"
#include "CppUtilities/Network/Socket.h"

#include "Database/Connection.h"
#include "Database/src/SqliteConnection.h"

#include <cstdlib>
#include <memory>
#include <sstream>
#include <thread>

using namespace etsai::cpputilities;
using namespace etsai::saremotedatabase;
using namespace std;

Connection *dbConn;
Logger *logger;

void handler(shared_ptr<Socket> socket) {
    string line, data;
    
    logger->log(Level::INFO, "request: " + socket->readLine());
    socket->write("server-achievements,1,response|0");

    while((line= socket->readLine()) != "") {
        logger->log(Level::INFO, line);
        data= "server-achievements,1,response|0|" + dbConn->retrieveAchievementData("76561197961630515","Stock KF");
        socket->write(data);
        logger->log(Level::INFO, "data: " + data);
    }
}

void initDbConnection(const string& dbURL, const string& user, const string& passwd) {
    dbConn= new SqliteConnection();
    dbConn->open(dbURL, user, passwd);
}

int main(int argc, char **argv) {
    logger= Logger::getLogger("saremotedatabase");
    ServerSocket server;
    int port;
    string dbURL, dbUser, dbPasswd;

    CliBuilder *cli= CliBuilder::getBuilder();
    (*cli).addOption(Option("-tcpport", 0, 1, [&port](const Arguments &args) -> void {
        port= args.asInteger(0);
    }).withArgName("number").withDescription("TPC port to listen on for requests").withRequired(true))
    .addOption(Option("-dburl", 0, 1, [&dbURL](const Arguments &args) -> void {
        dbURL= args.asString(0);
    }).withArgName("url").withDescription("URL to the database storing achievement information").withRequired(true))
    .addOption(Option("-h", [&cli](const Arguments &args) -> void {
        cli->displayUsage();
    }).withLongOpt("--help").withDescription("Displays this help message and exits"))
    .addOption(Option("-dbuser", 0, 1, [&dbUser](const Arguments &args) -> void {
        dbUser= args.asString(0);
    }).withArgName("user name").withDescription("Username for logging into the database"))
    .addOption(Option("-dbpwd", 0, 1, [&dbPasswd](const Arguments &args) -> void {
        dbPasswd= args.asString(0);
    }).withArgName("password").withDescription("Password for logging into the database"))
    .setUsage("saremotedatabase -database <name> -tcpport <number> [options]");
    cli->parse(argc, argv);

    logger->addHandler(new ConsoleHandler());
    logger->addHandler(new FileHandler());
    server.bind(port);
    initDbConnection(dbURL, dbUser, dbPasswd);
    {
        stringstream msg;
        msg << "Listening on port: " << port;
        logger->log(Level::INFO, msg.str());
    }
    while(true) {
        stringstream msg;
        shared_ptr<Socket> client= server.accept();
        msg << "Received connection from " << client->getAddress() << ":" << client->getPort();
        logger->log(Level::INFO, msg.str());
        thread th(handler, client);
        th.detach();
    }

    return 0;
}
