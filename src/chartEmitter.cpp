#include <iostream>
#include <fstream>

#include "chartEmitter.h"

namespace FC { namespace BE {

ChartEmitter::ChartEmitter(std::shared_ptr<IR::Stm> tree) : tree(tree), doRedirect(false) {
    this->boxes.reset(this->tree->Build());
}

ChartEmitter::ChartEmitter(std::shared_ptr<IR::Stm> tree, const std::string &chartPath) : tree(tree), doRedirect(true), chartPath(chartPath) {
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

    if (!this->doRedirect) {
        // print to stdout
        for (std::vector<char> row : this->flowchart) {
            for (char c : row) {
                std::cout << c;
            }
            std::cout << std::endl;
        }
    }
    else {
        // print to chartPath
        std::fstream fs = std::fstream();
        fs.open(this->chartPath.c_str(), std::fstream::out);
        if (!fs.is_open()) {
            std::cout << "[Error] output file open failed." << std::endl;
            exit(-1);
        }
        for (std::vector<char> row : this->flowchart) {
            for (char c : row) {
                fs << c;
            }
            fs << std::endl;
        }
        fs.close();
    }
}

} // namespace BE
} // namespace FC