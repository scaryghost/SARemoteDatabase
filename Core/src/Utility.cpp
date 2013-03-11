#include "Core/Utility.h"

namespace etsai {
namespace saremotedatabase {
namespace utility {

vector<string> split(const string& str, char separator) {
    string tempStr;
    vector<string> parts;
    size_t i= 0;

    while(i < str.size()) {
        if (str[i] == separator && !tempStr.empty()) {
            parts.push_back(tempStr);
            tempStr.clear();
        } else {
            tempStr+= str[i];
        }
        i++;
    }
    if (!tempStr.empty()) {
        parts.push_back(tempStr);
    }
    return parts;
}

string join(const vector<string>& parts, char separator) {
    string joinedStr;

    for(auto it= parts.begin(); it != parts.end(); it++) {
        if (it != parts.begin()) {
            joinedStr+= separator;
        }
        joinedStr+= (*it);
    }
    return joinedStr;
}

}
}
}
