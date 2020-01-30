#include <iostream>

#include "codeParser.h"

namespace FC { namespace FE {

CodeParser::CodeParser(const std::string &path, std::shared_ptr<IR::Tree> tree) {
    this->fs = std::fstream();
    this->fs.open(path.c_str(), std::ios::app);
    if (!this->fs.is_open()) {
        std::cout << "[Error] code file open failed." << std::endl;
        exit(-1);
    }
}

CodeParser::~CodeParser() {
    this->fs.close();
}

}}