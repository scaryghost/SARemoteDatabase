#ifndef ETSAI_SAREMOTEDATABASE_OUTER
#define ETSAI_SAREMOTEDATABASE_OUTER

#include <string>

namespace etsai {
namespace saremotedatabase {
namespace outer {

using std::string;

void start(int port, const string& password, int timeout);
void initCtrlHandler();
void loadDBLib(const string& dbLib);

}   //namespace outer
}   //namespace saremotedatabase
}   //namespace etsai

#endif
