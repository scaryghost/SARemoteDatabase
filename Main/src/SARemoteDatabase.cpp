#include "CppUtilities/Cli/cli.h"
#include "CppUtilities/Logging/logging.h"
#include "CppUtilities/Network/ServerSocket.h"
#include "CppUtilities/Network/Socket.h"

#include "Core/Utility.h"
#include "Database/Connection.h"
#include "Database/src/SqliteConnection.h"
#include "Net/Message.h"

#include <cstdlib>
#include <memory>
#include <sstream>
#include <thread>

using namespace etsai::cpputilities;
using namespace etsai::saremotedatabase;
using namespace std;

Connection *dbConn;
Logger *logger;

static int port;
static string dbURL, dbUser, dbPasswd, password;

void start();
void handler(shared_ptr<Socket> socket);
void initDbConnection();

int main(int argc, char **argv) {
    logger= Logger::getLogger("saremotedatabase");

    CliBuilder *cli= CliBuilder::getBuilder();
    (*cli).addOption(Option("-tcpport", 0, 1, [](const Arguments &args) -> void {
        port= args.asInteger(0);
    }).withArgName("number").withDescription("TCP port to listen on for requests").withRequired(true))
    .addOption(Option("-dburl", 0, 1, [](const Arguments &args) -> void {
        dbURL= args.asString(0);
    }).withArgName("url").withDescription("URL to the database storing achievement information").withRequired(true))
    .addOption(Option("-passwd", 0, 1, [](const Arguments &args) -> void {
        password= args.asString(0);
    }).withArgName("value").withDescription("Password for connecting to the server"))
    .addOption(Option("-h", [&cli](const Arguments &args) -> void {
        cli->displayUsage();
    }).withLongOpt("--help").withDescription("Displays this help message and exits"))
    .addOption(Option("-dbuser", 0, 1, [](const Arguments &args) -> void {
        dbUser= args.asString(0);
    }).withArgName("user name").withDescription("Username for logging into the database"))
    .addOption(Option("-dbpwd", 0, 1, [](const Arguments &args) -> void {
        dbPasswd= args.asString(0);
    }).withArgName("password").withDescription("Password for logging into the database"))
    .setUsage("saremotedatabase -dburl <url> -tcpport <number> [options]");
    cli->parse(argc, argv);

    logger->addHandler(new ConsoleHandler());
    logger->addHandler(new FileHandler());
    initDbConnection();
    start();

    return 0;
}

void start() {
    ServerSocket server;
    stringstream msg;

    msg << "Listening on port: " << port;
    logger->log(Level::INFO, msg.str());
    server.bind(port);
    while(true) {
        shared_ptr<Socket> client= server.accept();
        logger->log(Level::INFO, "Received connection from " + client->getAddressPort());
        thread th(handler, client);
        th.detach();
    }
}

void handler(shared_ptr<Socket> socket) {
    bool validConnection= true;
    string line;
    
    while(validConnection && (line= socket->readLine()) != "") {
        vector<string> bodyParts;
        string data;
        Message request, response;
        logger->log(Level::INFO, "request: " + line);
        request= Message::parse(line);

        switch (request.getRequest()) {
            case Message::CONNECT:
                if (request.getBody() != password) {
                    validConnection= false;
                    data= "Invalid password";
                } else {
                    data= "ok";
                }
                
                break;
            case Message::RETRIEVE:
                bodyParts= utility::split(request.getBody(), '.');
                data= dbConn->retrieveAchievementData(bodyParts[0], bodyParts[1]);
                break;
            case Message::SAVE:
                bodyParts= utility::split(request.getBody(), '.');
                dbConn->saveAchievementData(bodyParts[0], bodyParts[1], bodyParts[2]);
                break;
            default:
                break;
        }
        response.setType(Message::RESPONSE).setStatus(0).setBody(data);
        socket->write(response.toString());
        logger->log(Level::INFO, "response: " + response.toString());
    }
    socket->close();
    logger->log(Level::INFO, "Connection to " + socket->getAddressPort() + " closed");
}

void initDbConnection() {
    dbConn= new SqliteConnection();
    dbConn->open(dbURL, dbUser, dbPasswd);
}


