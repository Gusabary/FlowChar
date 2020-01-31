#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <vector>

#include "tree.h"
#include "token.h"

namespace FC { namespace FE {

class CodeParser
{
private:
    std::fstream fs;
    std::vector<Token *> tokenList;

public:
    CodeParser(const std::string &path, std::shared_ptr<IR::Tree> tree);
    ~CodeParser();
    void scan();
};
}}