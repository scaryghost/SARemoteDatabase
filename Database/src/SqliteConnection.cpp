#include "Core/Utility.h"
#include "Database/src/SqliteConnection.h"

#include <cstdlib>
#include <sstream>
#include <vector>

namespace etsai {
namespace saremotedatabase {

using std::atoi;
using std::stringstream;
using std::vector;

string SqliteConnection::retrieveQuery= "select achv_index, completed, progress from data d inner join pack p on p.id = d.pack_id and p.name=? and steamid64=?";
string SqliteConnection::saveStmt= "insert or ignore into data (steamid64,pack_id,achv_index,completed,progress) select ?, p.id, ?, ?, ? from pack p where p.name=?";
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
    if (status != SQLITE_DONE) {
        throw runtime_error(sqlite3_errmsg(dbObj));
    }
    sqlite3_finalize(stmt);
    return utility::join(dataParts, ';');
}

void SqliteConnection::saveAchievementData(const string& steamid64, const string& packName, const string& data) throw (runtime_error) {
    char* errorMessage;
    sqlite3_stmt *stmt, *update;
    auto dataParts= utility::split(data, ';');

    sqlite3_prepare_v2(dbObj, addPackName.c_str(), addPackName.size(), &stmt, NULL);
    sqlite3_bind_text(stmt, 1, packName.c_str(), packName.size(), SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_exec(dbObj, "BEGIN TRANSACTION", NULL, NULL, &errorMessage);
    sqlite3_prepare_v2(dbObj, saveStmt.c_str(), saveStmt.size(), &stmt, NULL);
    sqlite3_prepare_v2(dbObj, updateStmt.c_str(), updateStmt.size(), &update, NULL);
    for(string &achvState: dataParts) {
        auto stateParts= utility::split(achvState, ',');

        sqlite3_bind_text(stmt, 1, steamid64.c_str(), steamid64.size(), SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, atoi(stateParts[0].c_str()));
        sqlite3_bind_int(stmt, 3, atoi(stateParts[1].c_str()));
        sqlite3_bind_int(stmt, 4, atoi(stateParts[2].c_str()));
        sqlite3_bind_text(stmt, 5, packName.c_str(), packName.size(), SQLITE_STATIC);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            throw runtime_error(sqlite3_errmsg(dbObj));
        }

        sqlite3_bind_int(update, 1, atoi(stateParts[1].c_str()));
        sqlite3_bind_int(update, 2, atoi(stateParts[2].c_str()));
        sqlite3_bind_text(update, 3, steamid64.c_str(), steamid64.size(), SQLITE_STATIC);
        sqlite3_bind_int(update, 4, atoi(stateParts[0].c_str()));
        sqlite3_bind_text(update, 5, packName.c_str(), packName.size(), SQLITE_STATIC);
        if (sqlite3_step(update) != SQLITE_DONE) {
            throw runtime_error(sqlite3_errmsg(dbObj));
        }
 
        sqlite3_reset(stmt);
        sqlite3_reset(update);
    }
    if (sqlite3_exec(dbObj, "COMMIT TRANSACTION", NULL, NULL, &errorMessage)) {
        string msg(errorMessage);
        sqlite3_free(errorMessage);
        throw runtime_error(msg);
    }
    sqlite3_finalize(stmt);
    sqlite3_finalize(update);
}

}
}
