#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

#include "Data/src/SqliteDataChannel.h"

#include <exception>

#define UPSERT(stmt)\
    sqlite3_bind_int(stmt, 1, achv.completed);\
    sqlite3_bind_int(stmt, 2, achv.progress);\
    sqlite3_bind_text(stmt, 3, steamid64.c_str(), steamid64.size(), SQLITE_STATIC);\
    sqlite3_bind_int(stmt, 4, achv.index);\
    sqlite3_bind_text(stmt, 5, packName.c_str(), packName.size(), SQLITE_STATIC);\
    if (sqlite3_step(stmt) != SQLITE_DONE) {\
        throw runtime_error(sqlite3_errmsg(dbObj));\
    }\
    sqlite3_reset(stmt);\

namespace etsai {
namespace saremotedatabase {

using std::exception;

string SqliteDataChannel::retrieveQuery= "select achv_index, completed, progress from data d inner join pack p on p.id = d.pack_id and p.name=? and steamid64=?";
string SqliteDataChannel::saveStmt= "insert or ignore into data (completed,progress,steamid64,achv_index,pack_id) select ?, ?, ?, ?, p.id from pack p where p.name=?";
string SqliteDataChannel::updateStmt= "update data set completed=?, progress=? where steamid64=? and achv_index=? and pack_id=(select id from pack where name=?)";
string SqliteDataChannel::addPackName= "insert into pack (name) values (?)";

SqliteDataChannel::SqliteDataChannel() {
}

void SqliteDataChannel::open(const string& dataURL, const string& dataUser, const string& dataPw) throw (runtime_error) {
    int status;

    status= sqlite3_open_v2(dataURL.c_str(), &dbObj, SQLITE_OPEN_READWRITE|SQLITE_OPEN_NOMUTEX, NULL);
    if (status != SQLITE_OK) {
        throw runtime_error(sqlite3_errmsg(dbObj));
    }
}

void SqliteDataChannel::close() {
    sqlite3_close(dbObj);
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
        throw runtime_error(sqlite3_errmsg(dbObj));
    }
    return achvInfo;
}

void SqliteDataChannel::saveAchievementData(const string& steamid64, const string& packName, const Achievements achvData) throw (runtime_error) {
    string exceptMsg;
    char* errorMessage;
    sqlite3_stmt *packStmt, *insert, *update;

    sqlite3_prepare_v2(dbObj, addPackName.c_str(), addPackName.size(), &packStmt, NULL);
    sqlite3_bind_text(packStmt, 1, packName.c_str(), packName.size(), SQLITE_STATIC);
    sqlite3_step(packStmt);
    sqlite3_finalize(packStmt);

    try {
        sqlite3_exec(dbObj, "BEGIN TRANSACTION", NULL, NULL, &errorMessage);
        sqlite3_prepare_v2(dbObj, saveStmt.c_str(), saveStmt.size(), &insert, NULL);
        sqlite3_prepare_v2(dbObj, updateStmt.c_str(), updateStmt.size(), &update, NULL);
        for(auto achv: achvData.getInfo()) {
            UPSERT(insert);
            UPSERT(update);
        }
        sqlite3_exec(dbObj, "COMMIT TRANSACTION", NULL, NULL, &errorMessage);
    } catch (exception &ex) {
        sqlite3_exec(dbObj, "ROLLBACK TRANSACTION", NULL, NULL, &errorMessage);
        exceptMsg= ex.what();
    }
    sqlite3_finalize(insert);
    sqlite3_finalize(update);
    if (!exceptMsg.empty()) {
        throw runtime_error(exceptMsg);
    }
}

}
}
