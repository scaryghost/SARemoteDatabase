#include "Net/TcpListener.h"
#include "Net/Message.h"
#include "Core/Common.h"
#include "Core/Utility.h"

#include <vector>

namespace etsai {
namespace saremotedatabase {
namespace tcplistener {

using std::vector;

void handler(shared_ptr<Socket> socket, shared_ptr<time_point<system_clock> > lastActiveTime, const string& password) {
    bool terminate(false), authenticated(false);
    string line;
    vector<Message> pendingRequests;
    
    auto process= [&pendingRequests, &socket, &terminate, &authenticated, &lastActiveTime, &password](const Message& request) -> void {
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
                    if (bodyParts.size() >= 2) {
                        body= common::dbConn->retrieveAchievementData(bodyParts[0], bodyParts[1]);
                    }
                    status= 0;
                    (*lastActiveTime)= system_clock::now();
                } else {
                    pendingRequests.push_back(request);
                }
                break;
            case Message::SAVE:
                if (authenticated) {
                    bodyParts= utility::split(request.getBody(), '.');
                    if (bodyParts.size() >= 3) {
                        common::dbConn->saveAchievementData(bodyParts[0], bodyParts[1], bodyParts[2]);
                    }
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
            common::logger->log(Level::INFO, "response: " + response.toString());
        }
    };

    try {
        common::logger->log(Level::INFO, "Trying to get a line!");
        while(!terminate && (line= socket->readLine()) != "") {
            common::logger->log(Level::INFO, "request: " + line);
            process(Message::parse(line));
            if (authenticated && !pendingRequests.empty()) {
                for(Message &msg: pendingRequests) {
                    process(msg);
                }
                pendingRequests.clear();
            }
        }
    } catch (exception &e) {
        common::logger->log(Level::SEVERE, e.what());
    }

    socket->close();
    common::logger->log(Level::INFO, "Connection to " + socket->getAddressPort() + " closed");

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
    common::logger->log(Level::INFO, "Timeout!  force closing the connection from " + socket->getAddressPort());
}

}
}
}
