#ifndef ETSAI_SAREMOTEDATABASE_OUTER
#define ETSAI_SAREMOTEDATABASE_OUTER

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

#include <string>
#include <stdexcept>

namespace etsai {
namespace saremotedatabase {
namespace outer {

using std::string;
using std::runtime_error;

void start(int port, const string& password, int timeout);
void initCtrlHandler();
void loadDBLib(const string& dbLib) throw(runtime_error);

}   //namespace outer
}   //namespace saremotedatabase
}   //namespace etsai

#endif
