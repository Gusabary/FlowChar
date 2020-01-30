#pragma once
#include <string>
#include <memory>
#include <fstream>

#include "tree.h"

namespace FC { namespace FE {

class CodeParser
{
private:
    std::fstream fs;
        
public:
    CodeParser(const std::string &path, std::shared_ptr<IR::Tree> tree);
    ~CodeParser();
};
}}