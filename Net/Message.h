#ifndef ETSAI_SAREMOTEDATABASE_MESSAGE
#define ETSAI_SAREMOTEDATABASE_MESSAGE

#include <string>

namespace etsai {
namespace saremotedatabase {

using std::string;

class Message {
public:
    enum Type {
        UNKNOWN_TYPE= -1,
        REQUEST,
        RESPONSE
    };
    enum Request {
        UNKNOWN_REQUEST= -1,
        CONNECT,
        SAVE,
        RETRIEVE
    };

    static Message parse(const string& msg);

    Message();

    Message& setType(Type type);
    Message& setRequest(Request request);
    Message& setStatus(int status);
    Message& setBody(const string& body);
    Message& setId(int id);

    Type getType() const;
    Request getRequest() const;
    const string& getBody() const;
    int getStatus() const;
    int getId() const;
    string toString() const;

    static const string PROTOCOL;
    static const int VERSION;

private:
    Type type;
    Request request;
    int status;
    string body;
    int id;
};

}   //namespace saremotedatabase
}   //namespace etsai

#endif
