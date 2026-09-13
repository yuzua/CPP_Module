#include "replace.hpp"

std::string replaceAll(const std::string &content, const std::string &s1,
                       const std::string &s2) {
    if (s1.empty())
        return content;

    std::string result;
    std::string::size_type pos = 0;

    while (true) {
        const std::string::size_type found = content.find(s1, pos);
        if (found == std::string::npos) {
            result.append(content, pos, std::string::npos);
            break;
        }
        result.append(content, pos, found - pos);
        result.append(s2);
        pos = found + s1.length();
    }
    return result;
}
