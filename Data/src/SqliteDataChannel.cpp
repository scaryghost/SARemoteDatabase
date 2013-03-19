#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

#include "Data/src/SqliteDataChannel.h"

#include <exception>

#define UPSERT(stmt)\
    sqlite3_bind_int(stmt, 1, achv.completed);\
    sqlite3_bind_int(stmt, 2, achv.progress);\
    sqlite3_bind_int(stmt, 3, achv.index);\
    sqlite3_bind_text(stmt, 4, steamid64.c_str(), steamid64.size(), SQLITE_STATIC);\
    sqlite3_bind_text(stmt, 5, packName.c_str(), packName.size(), SQLITE_STATIC);\
    if (sqlite3_step(stmt) != SQLITE_DONE) {\
        throw runtime_error("Error committing data to the db");\
    }\
    sqlite3_reset(stmt)

#define ADD_PACK_OR_STEAMID64(stmt, sql, name)\
    sqlite3_prepare_v2(dbObj, sql.c_str(), sql.size(), &stmt, NULL);\
    sqlite3_bind_text(stmt, 1, name.c_str(), name.size(), SQLITE_STATIC);\
    sqlite3_step(stmt);\
    sqlite3_finalize(stmt)

namespace etsai {
namespace saremotedatabase {

using std::exception;

string SqliteDataChannel::retrieveQuery= "select achv_index, completed, progress from data d inner join pack p, user u on p.id = d.pack_id and p.name=? and u.id = d.user_id and u.steamid64=?";
string SqliteDataChannel::insertData= "insert or ignore into data (completed,progress,achv_index,user_id,pack_id) select ?,?,?,u.id,p.id from pack p, user u where u.steamid64=? and p.name=?";
string SqliteDataChannel::updateData= "update data set completed=?, progress=? where achv_index=? and user_id=(select id from user where steamid64=?) and pack_id=(select id from pack where name=?)";
string SqliteDataChannel::addPackName= "insert or ignore into pack (name) values (?)";
string SqliteDataChannel::addSteamID64= "insert or ignore into user (steamid64) values (?)";

SqliteDataChannel::SqliteDataChannel() : dbObj(NULL) {
}

void SqliteDataChannel::open(const string& dataURL, const string& dataUser, const string& dataPw) throw (runtime_error) {
    if (sqlite3_open_v2(dataURL.c_str(), &dbObj, SQLITE_OPEN_READWRITE|SQLITE_OPEN_NOMUTEX, NULL) != SQLITE_OK) {
        throw runtime_error(sqlite3_errmsg(dbObj));
    }
    sqlite3_exec(dbObj, "PRAGMA foreign_keys = ON", NULL, NULL, NULL);
}

void SqliteDataChannel::close() {
    sqlite3_close(dbObj);
    dbObj= NULL;
}

Achievements SqliteDataChannel::retrieveAchievementData(const string& steamid64, const string& packName) throw (runtime_error) {
    sqlite3_stmt *stmt;
    Achievements achvInfo;
    int status;

    sqlite3_prepare_v2(dbObj, retrieveQuery.c_str(), retrieveQuery.size(), &stmt, NULL);
    sqlite3_bind_text(stmt, 1, packName.c_str(), packName.size(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, steamid64.c_str(), steamid64.size(), SQLITE_STATIC);

    while((status= sqlite3_step(stmt)) == SQLITE_ROW) {
        Achievements::Info info= {sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1), sqlite3_column_int(stmt, 2)};
        achvInfo.insert(info);
    }
    sqlite3_finalize(stmt);
    if (status != SQLITE_DONE) {
        throw runtime_error("error retrieving saved data");
    }
    return achvInfo;
}

void SqliteDataChannel::saveAchievementData(const string& steamid64, const string& packName, const Achievements achvData) throw (runtime_error) {
    string exceptMsg;
    sqlite3_stmt *addStmt, *insertStmt, *updateStmt;

    ADD_PACK_OR_STEAMID64(addStmt, addPackName, packName);
    ADD_PACK_OR_STEAMID64(addStmt, addSteamID64, steamid64);

    try {
        sqlite3_exec(dbObj, "BEGIN TRANSACTION", NULL, NULL, NULL);
        sqlite3_prepare_v2(dbObj, insertData.c_str(), insertData.size(), &insertStmt, NULL);
        sqlite3_prepare_v2(dbObj, updateData.c_str(), updateData.size(), &updateStmt, NULL);
        for(auto achv: achvData.getInfo()) {
            UPSERT(insertStmt);
            UPSERT(updateStmt);
        }
        sqlite3_exec(dbObj, "COMMIT TRANSACTION", NULL, NULL, NULL);
    } catch (exception &ex) {
        sqlite3_exec(dbObj, "ROLLBACK TRANSACTION", NULL, NULL, NULL);
        exceptMsg= ex.what();
    }

    sqlite3_finalize(insertStmt);
    sqlite3_finalize(updateStmt);
    if (!exceptMsg.empty()) {
        throw runtime_error(exceptMsg);
    }
}

}
}
