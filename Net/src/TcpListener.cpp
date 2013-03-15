#include "Net/TcpListener.h"
#include "Net/Message.h"
#include "Core/Common.h"
#include "Core/Utility.h"

#include <vector>

#ifdef WIN32
#include <windows.h> 
#endif

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
        common::logger->log(Level::INFO, "Waiting for a request from " + socket->getAddressPort());
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

void timeout(shared_ptr<Socket> socket, shared_ptr<time_point<system_clock> > lastActiveTime,  int timeout) {
#ifndef WIN32
    int delta(0);
    timespec sleepTime= {timeout, 0};
#else
    __int64 delta(0);
#endif

    while(timeout > 0 && delta < timeout && !socket->isClosed()) {
#ifndef WIN32
        nanosleep(&sleepTime, NULL);
#else
        Sleep(timeout * 1000);
#endif
        delta= duration_cast<seconds>(system_clock::now() - (*lastActiveTime)).count();
#ifndef WIN32
        sleepTime.tv_sec= timeout - delta;
#endif
        
    }
    if (!socket->isClosed()) {
        common::logger->log(Level::INFO, "Idle max time reached.  Force closing the connection from " + socket->getAddressPort());
    }
    socket->close();
    
}

}
}
}
