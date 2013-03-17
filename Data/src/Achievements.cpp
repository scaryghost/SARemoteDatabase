#include "Data/Achievements.h"
#include "Core/Utility.h"

#include <cstdlib>
#include <sstream>

namespace etsai {
namespace saremotedatabase {

using std::atoi;
using std::stringstream;

const char Achievements::dataSeparator= ';';
const char Achievements::infoSeparator= ',';

Achievements::Achievements() {
}

Achievements::Achievements(const string& data) {
    deserialize(data);
}

void Achievements::insert(const Info& info) {
    achvInfo.push_back(info);
}

void Achievements::deserialize(const string& data) {
    auto achvParts= utility::split(data, dataSeparator);
    
    for(string &achv: achvParts) {
        auto infoParts= utility::split(achv, infoSeparator);
        if (infoParts.size() == 3) {
            Info info= {atoi(infoParts[0].c_str()), atoi(infoParts[1].c_str()), atoi(infoParts[2].c_str())};
            insert(info);
        }
    }
}

string Achievements::serialize() const {
    vector<string> dataParts;

    for(auto info: achvInfo) {
        stringstream dataStream;

        dataStream << info.index << infoSeparator << info.completed << infoSeparator << info.progress;
        dataParts.push_back(dataStream.str());
    }
    return utility::join(dataParts, dataSeparator);
}

const vector<Achievements::Info>& Achievements::getInfo() const {
    return achvInfo;
}

}
}
