#ifndef ETSAI_SAREMOTEDATABASE_SQLITEDATACHANNEL
#define ETSAI_SAREMOTEDATABASE_SQLITEDATACHANNEL

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

    virtual void open(const string& dataURL, const string& dataUser, const string& dataPw) throw (runtime_error);
    virtual void close();
    virtual string retrieveAchievementData(const string& steamid64, const string& packName) throw(runtime_error);
    virtual void saveAchievementData(const string& steamid64, const string& packName, const string achvData) throw(runtime_error);

private:
    sqlite3 *dbObj;
    static string retrieveQuery, insertData, updateData, addPackName, addSteamID64;
};

}   //namespace saremotedatabase
}   //namespace etsai

#endif
