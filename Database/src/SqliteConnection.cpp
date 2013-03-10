#include "Database/src/SqliteConnection.h"

namespace etsai {
namespace saremotedatabase {

using etsai::cpputilities::Level;

SqliteConnection::SqliteConnection() {
    logger= Logger::getLogger("saremotedatabase");
}

void SqliteConnection::open(const string& dbURL, const string& user, const string& passwd) {
    int status;

    status= sqlite3_open(dbURL.c_str(), &dbObj);
    if (status) {
        logger->log(Level::SEVERE, "Cannot connect to database: " + dbURL);
    }
}

void SqliteConnection::close() {
    sqlite3_close(dbObj);
}

string SqliteConnection::retrieveAchievementData(const string& steamid64, const string& packName) {
    return "0,1,315;1,0,12;2,0,1;3,0,1;4,1,0;5,1,0;6,0,0;7,0,0;8,0,0;9,1,0;10,0,4;11,0,1;12,0,2;13,1,1200;14,0,0;15,0,0;16,0,0;17,0,315;18,0,315;19,0,0;20,0,0;21,0,1;22,1,0;23,0,3;24,0,0;25,0,0;26,0,1;27,0,0;28,0,0;29,0,0;30,0,0;31,0,0;32,0,0;33,0,0;34,0,0;35,0,0;36,0,0;37,0,0;38,0,0;39,0,0;40,0,0;41,0,0;42,0,0;43,0,0";
}

void SqliteConnection::saveAchievementData(const string& steamid64, const string& packName, const string& data) {
}

}
}
