#ifndef ETSAI_SAREMOTEDATABASE_DATACHANNEL
#define ETSAI_SAREMOTEDATABASE_DATACHANNEL

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

#include <stdexcept>
#include <string>

#include "Data/AchievementPack.h"

namespace etsai {
namespace saremotedatabase {

using std::runtime_error;
using std::string;

/**
 * Provides a connection to the achievement data.  All implementations of this interface must be thread safe.
 * @author etsai
 */
class DataChannel {
public:
    /**
     * Open a connection to where the data is stored, using the provided credentials if login is needed
     * @param   dataURL             URL of the data location
     * @param   dataUser            Username for login credentials
     * @param   dataPw              Password for login credentials
     * @throw   runtimne_error      If a connection cannot be made with the given parameters
     */
    virtual void open(const string& dataURL, const string& dataUser, const string& dataPw) throw(runtime_error)= 0;
    /**
     * Close the connection and deallocates resources
     */
    virtual void close()= 0;
    /**
     * Retrieves the achievement data for the specified player and achievement pack
     * @param   steamid64           Player's steamid64 to lookup
     * @param   packName            Achievement pack name to lookup
     * @throw   runtime_error       If an error occured during the retrieval
     */
    virtual AchievementPack retrieveAchievementData(const string& steamid64, const string& packName) throw(runtime_error)= 0;
    /**
     * Save the achievement data
     * @param   steamid64           Owner's steamid64
     * @param   packName            Achievement pack name
     * @param   achvData            Achievement data to store
     * @throw   runtime_error       If the data was unable to be written
     */
    virtual void saveAchievementData(const string& steamid64, const string& packName, 
            const AchievementPack achvData) throw(runtime_error)= 0;
};

}
}

#endif
