#ifndef ETSAI_SAREMOTEDATABASE_UTILITY
#define ETSAI_SAREMOTEDATABASE_UTILITY

#include <string>
#include <vector>

namespace etsai {
namespace saremotedatabase {
namespace utility {

using std::string;
using std::vector;

vector<string> split(const string& str, char separator);

string join(const vector<string>& parts, char separator);

}   //namespace utility
}   //namespace saremotedatabase
}   //namespace etsai

#endif
