#ifndef ETSAI_SAREMOTEDATABASE_COMMON
#define ETSAI_SAREMOTEDATABASE_COMMON

#include "Database/Connection.h"
#include "CppUtilities/Logging/logging.h"

#include <string>
#ifndef WIN32
#include <signal.h>
#endif

namespace etsai {
namespace saremotedatabase {
namespace common {

using std::string;
using namespace cpputilities;

extern Logger* logger;
extern Connection* dbConn;
#ifndef WIN32
extern struct sigaction sigIntHandler;
#endif

void start(int port, const string& password, int timeout);
void initCtrlHandler();
void loadDBLib(const string& dbLib);

}   //namespace common
}   //namespace saremotedatabase
}   //namespace etsai

#endif
