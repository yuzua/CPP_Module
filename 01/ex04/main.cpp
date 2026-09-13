#include "replace.hpp"



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

}



int main(int argc, char **argv) {

    if (argc != 4) {
        std::cerr << "引数の数が不正です" << std::endl;
        return 1;
    }
    const std::string s1(argv[2]);
    if (s1.empty()) {
        std::cerr << "置き換え元文字列が空です" << std::endl;
        return 1;
    }

    const std::string file_name(argv[1]);
    std::string file_content;
    if (!readFile(file_name, file_content)) {
        std::cerr << "ファイルを開けません" << std::endl;
        return 1;
    }

    const std::string s2(argv[3]);
    const std::string replaced_content = replaceAll(file_content, s1, s2);
    if (!writeFile(file_name + ".replace", replaced_content)) {
        std::cerr << "ファイルを作成できません" << std::endl;
        return 1;
    }
    return 0;
}
