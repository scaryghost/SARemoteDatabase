#ifndef ETSAI_SAREMOTEDATABASE_SQLITECONNECTION
#define ETSAI_SAREMOTEDATABASE_SQLITECONNECTION

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

#include "Data/DataChannel.h"
#include "sqlite3.h"

namespace etsai {
namespace saremotedatabase {

class SqliteDataChannel : public DataChannel {
public:
    SqliteDataChannel();

    virtual void open(const string& dbURL, const string& user, const string& passwd) throw (runtime_error);
    virtual void close();
    virtual Achievements retrieveAchievementData(const string& steamid64, const string& packName) throw(runtime_error);
    virtual void saveAchievementData(const string& steamid64, const string& packName, 
            const Achievements achvData) throw(runtime_error);
private:
    sqlite3 *dbObj;

    static string retrieveQuery, saveStmt, updateStmt, addPackName;
};

}
}

#endif
