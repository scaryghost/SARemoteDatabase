#include "Core/Utility.h"
#include "Database/src/SqliteConnection.h"

#include <cstdlib>
#include <exception>
#include <sstream>
#include <vector>

#define UPSERT(stmt)\
    sqlite3_bind_int(stmt, 1, atoi(stateParts.at(1).c_str()));\
    sqlite3_bind_int(stmt, 2, atoi(stateParts.at(2).c_str()));\
    sqlite3_bind_text(stmt, 3, steamid64.c_str(), steamid64.size(), SQLITE_STATIC);\
    sqlite3_bind_int(stmt, 4, atoi(stateParts.at(0).c_str()));\
    sqlite3_bind_text(stmt, 5, packName.c_str(), packName.size(), SQLITE_STATIC);\
    if (sqlite3_step(stmt) != SQLITE_DONE) {\
        throw runtime_error(sqlite3_errmsg(dbObj));\
    }\
    sqlite3_reset(stmt);\

namespace etsai {
namespace saremotedatabase {

using std::atoi;
using std::exception;
using std::stringstream;
using std::vector;

string SqliteConnection::retrieveQuery= "select achv_index, completed, progress from data d inner join pack p on p.id = d.pack_id and p.name=? and steamid64=?";
string SqliteConnection::saveStmt= "insert or ignore into data (completed,progress,steamid64,achv_index,pack_id) select ?, ?, ?, ?, p.id from pack p where p.name=?";
string SqliteConnection::updateStmt= "update data set completed=?, progress=? where steamid64=? and achv_index=? and pack_id=(select id from pack where name=?)";
string SqliteConnection::addPackName= "insert into pack (name) values (?)";

SqliteConnection::SqliteConnection() {
}

void SqliteConnection::open(const string& dbURL, const string& user, const string& passwd) throw (runtime_error) {
    int status;

    status= sqlite3_open_v2(dbURL.c_str(), &dbObj, SQLITE_OPEN_READWRITE|SQLITE_OPEN_NOMUTEX, NULL);
    if (status != SQLITE_OK) {
        throw runtime_error(sqlite3_errmsg(dbObj));
    }
}

void SqliteConnection::close() {
    sqlite3_close(dbObj);
}

string SqliteConnection::retrieveAchievementData(const string& steamid64, const string& packName) throw (runtime_error) {
    sqlite3_stmt *stmt;

    vector<string> dataParts;
    int status;

    sqlite3_prepare_v2(dbObj, retrieveQuery.c_str(), retrieveQuery.size(), &stmt, NULL);
    sqlite3_bind_text(stmt, 1, packName.c_str(), packName.size(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, steamid64.c_str(), steamid64.size(), SQLITE_STATIC);

    while((status= sqlite3_step(stmt)) == SQLITE_ROW) {
        stringstream colData;

        colData << sqlite3_column_int(stmt, 0) << "," << sqlite3_column_int(stmt, 1) << "," << sqlite3_column_int(stmt, 2);
        dataParts.push_back(colData.str());
    }
    sqlite3_finalize(stmt);
    if (status != SQLITE_DONE) {
        throw runtime_error(sqlite3_errmsg(dbObj));
    }
    return utility::join(dataParts, ';');
}

void SqliteConnection::saveAchievementData(const string& steamid64, const string& packName, const string& data) throw (runtime_error) {
    string exceptMsg;
    char* errorMessage;
    sqlite3_stmt *packStmt, *insert, *update;
    auto dataParts= utility::split(data, ';');

    sqlite3_prepare_v2(dbObj, addPackName.c_str(), addPackName.size(), &packStmt, NULL);
    sqlite3_bind_text(packStmt, 1, packName.c_str(), packName.size(), SQLITE_STATIC);
    sqlite3_step(packStmt);
    sqlite3_finalize(packStmt);

    try {
        sqlite3_exec(dbObj, "BEGIN TRANSACTION", NULL, NULL, &errorMessage);
        sqlite3_prepare_v2(dbObj, saveStmt.c_str(), saveStmt.size(), &insert, NULL);
        sqlite3_prepare_v2(dbObj, updateStmt.c_str(), updateStmt.size(), &update, NULL);
        for(string &achvState: dataParts) {
            auto stateParts= utility::split(achvState, ',');
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
