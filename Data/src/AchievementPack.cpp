#include "Data/AchievementPack.h"
#include "Core/Utility.h"

#include <cstdlib>
#include <sstream>
#include <stdexcept>

namespace etsai {
namespace saremotedatabase {

using std::atoi;
using std::stringstream;
using std::out_of_range;

const char AchievementPack::dataSeparator= ';';
const char AchievementPack::achvSeparator= ',';

AchievementPack::AchievementPack() {
}

AchievementPack::AchievementPack(const string &data) {
    deserialize(data);
}

AchievementPack& AchievementPack::operator=(const AchievementPack &rhs) {
    if (this != &rhs) {
        this->achievements= rhs.achievements;
    }
    return *this;
}

void AchievementPack::insert(const Achievement& achv) {
    if (achvVectorIndex.count(achv.index) == 0) {
        achievements.push_back(achv);
        achvVectorIndex[achv.index]= achievements.size() - 1;
    } else {
        achievements[achvVectorIndex[achv.index]]= achv;
    }
}

void AchievementPack::deserialize(const string& data) {
    auto achvParts= utility::split(data, dataSeparator);
    
    for(string &achv: achvParts) {
        auto achvParts= utility::split(achv, achvSeparator);
        if (achvParts.size() == 3) {
            Achievement achv= {atoi(achvParts[0].c_str()), atoi(achvParts[1].c_str()), atoi(achvParts[2].c_str())};
            insert(achv);
        }
    }
}

string AchievementPack::serialize() const {
    vector<string> dataParts;

    for(auto achv: achievements) {
        if (achv.completed != 0 || achv.progress != 0) {
            stringstream dataStream;

            dataStream << achv.index << achvSeparator << achv.completed << achvSeparator << achv.progress;
            dataParts.push_back(dataStream.str());
        }
    }
    return utility::join(dataParts, dataSeparator);
}

const vector<AchievementPack::Achievement>& AchievementPack::getAchievements() const {
    return achievements;
}

}
}
