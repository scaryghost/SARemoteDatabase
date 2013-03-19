#ifndef ETSAI_SAREMOTEDATABASE_ACHIEVEMENTPACK
#define ETSAI_SAREMOTEDATABASE_ACHIEVEMENTPACK

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
class AchievementPack {
public:
    /** Represents the state of an achievement */
    struct Achievement {
        int index;          ///< Achievement index
        int completed;      ///< 0 if not yet completed
        int progress;       ///< How much progress has been accumulated
    };

    /**
     * Constructs a blank achievement pack
     */
    AchievementPack();

    /**
     * Assignment operator
     * @param   rhs     Object to copy from
     */
    AchievementPack& operator=(const AchievementPack &rhs);
    /**
     * Inserts an achievement state
     * @param   achv    Achievement state to add
     */
    void insert(const Achievement& achv);
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
    const vector<Achievement>& getAchievements() const;

    static const char dataSeparator;        ///< Character that separates each achievement state
    static const char achvSeparator;        ///< Character that separates the data within an achievement state
private:
    vector<Achievement> achievements;       ///< Set of achievement states
};

}   //namespace saremotedatabase
}   //namespace etsai

#endif
