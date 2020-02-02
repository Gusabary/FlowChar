#pragma once

#include <memory>

#include "tree.h"

namespace FC { namespace BE {

class ChartEmitter
{
private:
    std::shared_ptr<IR::Stm> tree;

public:
    ChartEmitter(std::shared_ptr<IR::Stm> tree);
    void attachChartInfo();
};
}}