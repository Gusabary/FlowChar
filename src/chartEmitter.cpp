#include <iostream>

#include "chartEmitter.h"

namespace FC { namespace BE {

ChartEmitter::ChartEmitter(std::shared_ptr<IR::Stm> tree) : tree(tree) {
    this->boxes.reset(this->tree->Build());
}

void ChartEmitter::attachChartInfo() {
    AttachInfo ainfo = this->boxes->Attach();
    int chartWidth = ainfo.lWidth + ainfo.rWidth + 1;
    // this->boxes->Print(0);

    // init flowchart
    this->flowchart.resize(ainfo.height);
    for (std::vector<char> &row : this->flowchart) {
        row = std::vector<char>(chartWidth, ' ');
    }
    this->start = std::make_pair(1, ainfo.lWidth);
}

void ChartEmitter::drawFlowChart() {
    this->boxes->Draw(this->flowchart, this->start);
    for (std::vector<char> row : this->flowchart) {
        for (char c : row) {
            std::cout << c;
        }
        std::cout << std::endl;
    }
}
}}