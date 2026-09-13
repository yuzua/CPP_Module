#include "replace_.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

bool readFile(const std::string &path, std::string &out) {
    std::ifstream in(path.c_str());
    if (!in.is_open())
        return false;

    std::ostringstream buffer;
    buffer << in.rdbuf();
    if (!in.good() && !in.eof())
        return false;

    out = buffer.str();
    return true;
}

bool writeFile(const std::string &path, const std::string &content) {
    std::ofstream out(path.c_str());
    if (!out.is_open())
        return false;

    out << content;
    return out.good();
}

} // namespace

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <filename> <s1> <s2>"
                  << std::endl;
        return 1;
    }

    const std::string filename(argv[1]);
    const std::string s1(argv[2]);
    const std::string s2(argv[3]);

    if (s1.empty()) {
        std::cerr << "Error: s1 must not be empty" << std::endl;
        return 1;
    }

    std::string content;
    if (!readFile(filename, content)) {
        std::cerr << "Error: cannot open '" << filename << "'" << std::endl;
        return 1;
    }

    const std::string replaced = replaceAll(content, s1, s2);
    const std::string outPath = filename + ".replace";

    if (!writeFile(outPath, replaced)) {
        std::cerr << "Error: cannot write '" << outPath << "'" << std::endl;
        return 1;
    }

    return 0;
}
