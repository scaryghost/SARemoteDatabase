#ifndef ETSAI_SAREMOTEDATABASE_SQLITECONNECTION
#define ETSAI_SAREMOTEDATABASE_SQLITECONNECTION

#include "Database/Connection.h"
#include "sqlite3.h"

namespace etsai {
namespace saremotedatabase {

class SqliteConnection : public Connection {
public:
    SqliteConnection();

    virtual void open(const string& dbURL, const string& user, const string& passwd) throw (runtime_error);
    virtual void close();
    virtual string retrieveAchievementData(const string& steamid64, const string& packName) throw (runtime_error);
    virtual void saveAchievementData(const string& steamid64, const string& packName, const string& data) throw (runtime_error);
private:
    sqlite3 *dbObj;

    static string retrieveQuery, saveStmt, updateStmt, addPackName;
};

}
}

#endif
