#ifndef ETSAI_SAREMOTEDATABASE_COMMON
#define ETSAI_SAREMOTEDATABASE_COMMON

#include "Database/Connection.h"
#include "CppUtilities/Logging/logging.h"

namespace etsai {
namespace saremotedatabase {
namespace global {

using std::string;
using namespace cpputilities;

extern Logger* logger;
extern Connection* dbConn;

}   //namespace global
}   //namespace saremotedatabase
}   //namespace etsai

#endif
