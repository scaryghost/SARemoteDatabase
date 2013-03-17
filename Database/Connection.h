#ifndef ETSAI_SAREMOTEDATABASE_CONNECTION
#define ETSAI_SAREMOTEDATABASE_CONNECTION

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

#include <stdexcept>
#include <string>

namespace etsai {
namespace saremotedatabase {

using std::string;
using std::runtime_error;

class Connection {
public:
    virtual void open(const string& dbURL, const string& user, const string& passwd) throw(runtime_error)= 0;
    virtual void close()= 0;
    virtual string retrieveAchievementData(const string& steamid64, const string& packName) throw(runtime_error)= 0;
    virtual void saveAchievementData(const string& steamid64, const string& packName, const string& data) throw(runtime_error)= 0;
};

}
}

#endif
