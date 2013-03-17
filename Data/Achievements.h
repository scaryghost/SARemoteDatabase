#ifndef ETSAI_SAREMOTEDATABASE_ACHIEVEMENTS
#define ETSAI_SAREMOTEDATABASE_ACHIEVEMENTS

#include <string>
#include <vector>

namespace etsai {
namespace saremotedatabase {

using std::string;
using std::vector;

class Achievements {
public:
    struct Info {
        int index;
        int completed;
        int progress;
    };

    Achievements();
    Achievements(const string& data);

    void insert(const Info& info);
    void deserialize(const string& data);

    string serialize() const;
    const vector<Info>& getInfo() const;

    static const char dataSeparator;
    static const char infoSeparator;
private:
    vector<Info> achvInfo;

};

}   //namespace saremotedatabase
}   //namespace etsai

#endif
