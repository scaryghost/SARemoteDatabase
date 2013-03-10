#ifndef ETSAI_SAREMOTEDATABASE_CONNECTION
#define ETSAI_SAREMOTEDATABASE_CONNECTION

#include <string>

namespace etsai {
namespace saremotedatabase {

using std::string;

class Connection {
public:
    virtual void open(const string& dbURL, const string& user, const string& passwd)= 0;
    virtual void close()= 0;
    virtual string retrieveAchievementData(const string& steamid64, const string& packName)= 0;
    virtual void saveAchievementData(const string& steamid64, const string& packName, const string& data)= 0;
};

}
}

#endif
