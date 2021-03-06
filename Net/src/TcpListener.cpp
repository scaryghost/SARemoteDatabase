#include "Net/TcpListener.h"
#include "Net/Message.h"
#include "Core/Global.h"
#include "Core/Utility.h"

#include <vector>

#ifdef WIN32
#include <windows.h> 
#endif

#define SAVE_OR_RETRIEVE(ec, func)\
    if (authenticated) {\
        try {\
            bodyParts= utility::split(request.getBody(), '.');\
            func;\
            (*lastActiveTime)= system_clock::now();\
        } catch (exception &ex) {\
            global::logger->log(Level::SEVERE, ex.what());\
            body= ex.what();\
            status= ec;\
        }\
    } else {\
        pendingRequests.push_back(request);\
    }\
    break

namespace etsai {
namespace saremotedatabase {
namespace tcplistener {

using std::vector;
void handler(shared_ptr<Socket> socket, shared_ptr<DataChannel> dataChnl, shared_ptr<time_point<system_clock> > lastActiveTime, const string& password) {
    bool terminate(false), authenticated(false);
    string line;
    vector<Message> pendingRequests;
    
    auto process= [&pendingRequests, &socket, &terminate, &authenticated, &lastActiveTime, &password, &dataChnl](const Message& request) -> void {
        vector<string> bodyParts;
        Message response;
        int status(0);
        string body;

        switch (request.getRequest()) {
            case Message::CONNECT:
                if (request.getBody() != password) {
                    terminate= true;
                    body= "Invalid password";
                    status= 1;
                } else {
                    authenticated= true;
                    (*lastActiveTime)= system_clock::now();
                }
                break;
            case Message::RETRIEVE:
                SAVE_OR_RETRIEVE(3, body= (dataChnl->retrieveAchievementData(bodyParts.at(0), bodyParts.at(1))));
            case Message::SAVE:
                SAVE_OR_RETRIEVE(2, dataChnl->saveAchievementData(bodyParts.at(0), bodyParts.at(1), bodyParts.at(2)));
            default:
                global::logger->log(Level::INFO, "Unrecognized request");
                body= "Unrecognized request";
                status= 4;
                break;
        }
        if (authenticated) {
            response.setType(Message::RESPONSE).setStatus(status).setBody(body).setId(request.getId());
            socket->write(response.toString() + "\n");
            global::logger->log(Level::INFO,"%s - response= %s", socket->getAddressPort().c_str(), response.toString().c_str());
        }
    };

    try {
        global::logger->log(Level::INFO, "Waiting for a request from %s", socket->getAddressPort().c_str());
        while(!terminate && (line= socket->readLine()) != "") {
            try {
                global::logger->log(Level::INFO,"%s - request= %s", socket->getAddressPort().c_str(), line.c_str());
                process(Message::parse(line));
                if (authenticated && !pendingRequests.empty()) {
                    for(Message &msg: pendingRequests) {
                        process(msg);
                    }
                    pendingRequests.clear();
                }
            } catch (exception &ex) {
                global::logger->log(Level::SEVERE, ex.what());
            }
        }
    } catch (exception &e) {
        global::logger->log(Level::SEVERE, e.what());
    }

    if (!socket->isClosed()) {
        socket->close();
        global::logger->log(Level::INFO, "Connection to %s closed", socket->getAddressPort().c_str());
    }
}

void timeout(shared_ptr<Socket> socket, shared_ptr<time_point<system_clock> > lastActiveTime, int timeout) {
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
    if (timeout > 0 && !socket->isClosed()) {
        global::logger->log(Level::INFO, "Max idle time reached, force closing the connection from %s", socket->getAddressPort().c_str());
        socket->close();
    }
}

}
}
}
