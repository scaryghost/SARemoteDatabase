#ifndef ETSAI_SAREMOTEDATABASE_DATACHANNEL
#define ETSAI_SAREMOTEDATABASE_DATACHANNEL

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

#include <stdexcept>
#include <string>

#include "Data/Achievements.h"

namespace etsai {
namespace saremotedatabase {

using std::runtime_error;
using std::string;

class DataChannel {
public:
    virtual bool isOpen() const= 0;
    virtual void open(const string& dataURL, const string& dataUser, const string& dataPw) throw(runtime_error)= 0;
    virtual void close()= 0;
    virtual Achievements retrieveAchievementData(const string& steamid64, const string& packName) throw(runtime_error)= 0;
    virtual void saveAchievementData(const string& steamid64, const string& packName, 
            const Achievements achvData) throw(runtime_error)= 0;
};

}
}

#endif
