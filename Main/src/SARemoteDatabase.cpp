#include "CppUtilities/Cli/cli.h"
#include "CppUtilities/Logging/logging.h"
#include "CppUtilities/Network/ServerSocket.h"
#include "CppUtilities/Network/Socket.h"

#include "Core/Utility.h"
#include "Database/Connection.h"
#include "Database/src/SqliteConnection.h"
#include "Net/Message.h"

#include <chrono>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <thread>
#include <vector>

using namespace etsai::cpputilities;
using namespace etsai::saremotedatabase;
using namespace std::chrono;

using std::shared_ptr;
using std::thread;
using std::vector;

shared_ptr<Connection> dbConn;
Logger *logger;

static int port;
static string dbURL, dbUser, dbPasswd, password;

void start();
void initDbConnection();
void handler(shared_ptr<Socket> socket, shared_ptr<time_point<system_clock> > lastActiveTime);
void timeout(shared_ptr<Socket> socket, shared_ptr<time_point<system_clock> > lastActiveTime);

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
    }).withArgName("value").withDescription("Password for connecting to the server").withRequired(true))
    .addOption(Option("-h", [&cli](const Arguments &args) -> void {
        cli->displayUsage();
    }).withLongOpt("--help").withDescription("Displays this help message and exits"))
    .addOption(Option("-dbuser", 0, 1, [](const Arguments &args) -> void {
        dbUser= args.asString(0);
    }).withArgName("user name").withDescription("Username for logging into the database"))
    .addOption(Option("-dbpwd", 0, 1, [](const Arguments &args) -> void {
        dbPasswd= args.asString(0);
    }).withArgName("password").withDescription("Password for logging into the database"))
    .setUsage("saremotedatabase -dburl <url> -tcpport <number> -passwd <value> [options]");
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
        shared_ptr<Socket> client(server.accept());
        shared_ptr<time_point<system_clock> > lastActiveTime(new time_point<system_clock>);

        logger->log(Level::INFO, "Received connection from " + client->getAddressPort());
        thread th(handler, client, lastActiveTime);
        thread th2(timeout, client, lastActiveTime);
        th.detach();
        th2.detach();
    }
}

void initDbConnection() {
    dbConn.reset(new SqliteConnection());
    dbConn->open(dbURL, dbUser, dbPasswd);
}

void handler(shared_ptr<Socket> socket, shared_ptr<time_point<system_clock> > lastActiveTime) {
    bool terminate(false), authenticated(false);
    string line;
    vector<Message> pendingRequests;
    
    auto process= [&pendingRequests, &socket, &terminate, &authenticated, &lastActiveTime](const Message& request) -> void {
        vector<string> bodyParts;
        Message response;
        int status;
        string body;

        switch (request.getRequest()) {
            case Message::CONNECT:
                if (request.getBody() != password) {
                    terminate= true;
                    body= "Invalid password";
                    status= 1;
                } else {
                    status= 0;
                    authenticated= true;
                    (*lastActiveTime)= system_clock::now();
                }
                break;
            case Message::RETRIEVE:
                if (authenticated) {
                    bodyParts= utility::split(request.getBody(), '.');
                    body= dbConn->retrieveAchievementData(bodyParts[0], bodyParts[1]);
                    status= 0;
                    (*lastActiveTime)= system_clock::now();
                } else {
                    pendingRequests.push_back(request);
                }
                break;
            case Message::SAVE:
                if (authenticated) {
                    bodyParts= utility::split(request.getBody(), '.');
                    dbConn->saveAchievementData(bodyParts[0], bodyParts[1], bodyParts[2]);
                    status= 0;
                    (*lastActiveTime)= system_clock::now();
                } else {
                    pendingRequests.push_back(request);
                }
                break;
            default:
                break;
        }
        if (authenticated) {
            response.setType(Message::RESPONSE).setStatus(status).setBody(body).setId(request.getId());
            socket->write(response.toString() + "\n");
            logger->log(Level::INFO, "response: " + response.toString());
        }
    };

    try {
        logger->log(Level::INFO, "Trying to get a line!");
        while(!terminate && (line= socket->readLine()) != "") {
            logger->log(Level::INFO, "request: " + line);
            process(Message::parse(line));
            if (authenticated && !pendingRequests.empty()) {
                for(Message &msg: pendingRequests) {
                    process(msg);
                }
                pendingRequests.clear();
            }
        }
    } catch (exception &e) {
        logger->log(Level::SEVERE, e.what());
    }

    socket->close();
    logger->log(Level::INFO, "Connection to " + socket->getAddressPort() + " closed");
}

void timeout(shared_ptr<Socket> socket, shared_ptr<time_point<system_clock> > lastActiveTime) {
    int delta(0);
    timespec timeout= {60, 0};

    while(delta < 60 && !socket->isClosed()) {
        nanosleep(&timeout, NULL);
        delta= duration_cast<seconds>(system_clock::now() - (*lastActiveTime)).count();
        timeout.tv_sec= 60 - delta;
    }
    socket->close();
    logger->log(Level::INFO, "Timeout!  force closing the connection!");
}

