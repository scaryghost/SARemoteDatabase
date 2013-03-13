#ifndef ETSAI_SAREMOTEDATABASE_COMMON
#define ETSAI_SAREMOTEDATABASE_COMMON

#include "Database/Connection.h"
#include "CppUtilities/Logging/logging.h"

#include <memory>
#include <string>

namespace etsai {
namespace saremotedatabase {
namespace common {

using std::shared_ptr;
using std::string;
using namespace cpputilities;

extern shared_ptr<Logger> logger;
extern shared_ptr<Connection> dbConn;

void start(int port, const string& password);
void initDbConnection(const string& dbURL, const string& dbUser, const string& dbPasswd);

}   //namespace common
}   //namespace saremotedatabase
}   //namespace etsai

#endif
