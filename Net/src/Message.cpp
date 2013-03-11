#include "Net/Message.h"
#include "Core/Utility.h"

#include <cstdlib>
#include <sstream>

namespace etsai {
namespace saremotedatabase {

using std::atoi;
using std::stringstream;

const string Message::PROTOCOL= "server-achievements";
const int Message::VERSION= 1;


Message Message::parse(const string& msg) {
    Message msgObj;
    Type type;
    Request request;
    int status;

    auto parts= utility::split(msg, '|');
    auto header= utility::split(parts[0], ',');
    
    if (header[2] == "request") {
        type= REQUEST;
        if (parts[2] == "connect") {
            request= CONNECT;
        } else if (parts[2] == "save") {
            request= SAVE;
        } else if (parts[2] == "retrieve") {
            request= RETRIEVE;
        }
    } else if (header[2] == "response") {
        type= RESPONSE;
        status= atoi(parts[2].c_str());
    }
    msgObj.setType(type).setRequest(request).setStatus(status).setBody(parts[3]).setId(atoi(parts[1].c_str()));
    return msgObj;
}

Message::Message() :
    type(UNKNOWN_TYPE), request(UNKNOWN_REQUEST), status(0) {
}

Message& Message::setType(Type type) {
    this->type= type;
    return *this;
}

Message& Message::setRequest(Request request) {
    this->request= request;
    return *this;
}

Message& Message::setStatus(int status) {
    this->request= request;
    return *this;
}

Message& Message::setBody(const string& body) {
    this->body= body;
    return *this;
}

Message& Message::setId(int id) {
    this->id= id;
    return *this;
}

Message::Type Message::getType() const {
    return type;
}

Message::Request Message::getRequest() const {
    return request;
}

const string& Message::getBody() const {
    return body;
}

int Message::getStatus() const {
    return status;
}

int Message::getId() const {
    return id;
}

string Message::toString() const {
    stringstream msgStr;

    msgStr << PROTOCOL << "," << VERSION << ",";
    if (type == REQUEST) {
        msgStr << "request|" << id << "|";
        if (request == CONNECT) {
            msgStr << "connect";
        } else if (request == SAVE) {
            msgStr << "save";
        } else if (request == RETRIEVE) {
            msgStr << "retrieve";
        }
    } else if (type == RESPONSE) {
        msgStr << "response|" << id << "|" << status;
    }
    if (!body.empty()) {  
        msgStr << "|" << body;
    }
    return msgStr.str();
}

}   //namespace saremotedatabase
}   //namespace etsai
