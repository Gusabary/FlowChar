#pragma once

#include <memory>

#include "tree.h"
#include "box.h"

namespace FC { namespace BE {

class ChartEmitter
{
private:
    std::shared_ptr<IR::Stm> tree;
    std::unique_ptr<Box> boxes;
    int chartWidth;
    std::vector<std::vector<char>> flowchart;

public:
    ChartEmitter(std::shared_ptr<IR::Stm> tree);
    void attachChartInfo();
    // void draw();
};
}}