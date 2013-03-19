#ifndef ETSAI_SAREMOTEDATABASE_TCPLISTENER
#define ETSAI_SAREMOTEDATABASE_TCPLISTENER

#include "Data/DataChannel.h"
#include "CppUtilities/Network/Socket.h"

#include <chrono>
#include <memory>

namespace etsai {
namespace saremotedatabase {
namespace tcplistener {

using namespace cpputilities;
using namespace std::chrono;
using std::shared_ptr;

void handler(shared_ptr<Socket> socket, shared_ptr<DataChannel> dataChnl, shared_ptr<time_point<system_clock> > lastActiveTime, const string& password);
void timeout(shared_ptr<Socket> socket, shared_ptr<time_point<system_clock> > lastActiveTime, int timeout);

}   //namespace tcplistener
}   //namespace saremotedatabase
}   //namespace etsai

#endif
