#ifndef ETSAI_SAREMOTEDATABASE_SQLITECONNECTION
#define ETSAI_SAREMOTEDATABASE_SQLITECONNECTION

#include "Database/Connection.h"
#include "CppUtilities/Logging/Logger.h"

extern "C" {
    #include "sqlite3.h"
}

namespace etsai {
namespace saremotedatabase {

using etsai::cpputilities::Logger;

class SqliteConnection : public Connection {
public:
    SqliteConnection();

    virtual void open(const string& dbURL, const string& user, const string& passwd);
    virtual void close();
    virtual string retrieveAchievementData(const string& steamid64, const string& packName);
    virtual void saveAchievementData(const string& steamid64, const string& packName, const string& data);
private:
    Logger *logger;
    sqlite3 *dbObj;
};

}
}

#endif
