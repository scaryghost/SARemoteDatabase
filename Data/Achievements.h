#ifndef ETSAI_SAREMOTEDATABASE_ACHIEVEMENTS
#define ETSAI_SAREMOTEDATABASE_ACHIEVEMENTS

#include <string>
#include <vector>

namespace etsai {
namespace saremotedatabase {

using std::string;
using std::vector;

/**
 * C++ equivalent of the UnrealScript AchievementPack class from ServerAchievements
 * @author etsai
 */
class Achievements {
public:
    /** Represents the state of an achievement */
    struct Info {
        int index;          ///< Achievement index
        int completed;      ///< 0 if not yet completed
        int progress;       ///< How much progress has been accumulated
    };

    /**
     * Default constructor
     */
    Achievements();
    /**
     * Constructs an Achievements object reprsented by the serialized data
     * @param   data    Serialized representation of the object
     * @see deserialize
     */
    Achievements(const string& data);

    /**
     * Inserts an achievement state
     * @param   info    Achievement state to add
     */
    void insert(const Info& info);
    /**
     * Deserialize the data and update the Achievements variables
     * @param   data    Serialized representation of the object
     */
    void deserialize(const string& data);

    /**
     * Convert the object into a string representation
     * @return  Serialized form of the object
     */
    string serialize() const;
    /**
     * Get the set of states for each achievement
     * @return  States for each achievement
     */
    const vector<Info>& getInfo() const;

    static const char dataSeparator;        ///< Character that separates each achievement state
    static const char infoSeparator;        ///< Character that separates the data within an achievement state
private:
    vector<Info> achvInfo;                  ///< Set of achievement states
};

}   //namespace saremotedatabase
}   //namespace etsai

#endif
