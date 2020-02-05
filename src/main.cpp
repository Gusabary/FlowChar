#include <iostream>
#include <cstdlib>
#include <memory>
#include <cstring>

#include "codeParser.h"
#include "tree.h"
#include "chartEmitter.h"

using namespace FC;

struct ArgInfo {
    std::string codePath;
    bool doRedirect;
    std::string chartPath;

    ArgInfo(std::string codePath) : codePath(codePath), doRedirect(false) {}
    ArgInfo(std::string codePath, std::string chartPath) : codePath(codePath), doRedirect(true), chartPath(chartPath) {}
};

void printHelp() {
    std::cout << std::endl;
    std::cout << "flowchar - a tiny tool for conversion from pseudo-code to flowchart in character format" << std::endl
              << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "\t flowchar -c <codePath> [-o <chartPath>]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "\t -c <path> \t specify path of code to be parsed, required." << std::endl;
    std::cout << "\t -o <path> \t specify path of chart to be emitted, optional. If absent, print to stdout." << std::endl;
}

ArgInfo parseArg(int argc, char **argv) {
    // -c <path>, specify path of code to be parsed, required.
    // -o <path>, specify path of chart to be emitted, optional. If absent, print to stdout.

    if (argc == 3 && !strcmp(argv[1], "-c")) {
        // must be -c
        return ArgInfo(argv[2]);
    }
    
    if (argc == 5) {
        // both -c and -o
        if (!strcmp(argv[1], "-c") && !strcmp(argv[3], "-o"))
            return ArgInfo(argv[2], argv[4]);
        if (!strcmp(argv[3], "-c") && !strcmp(argv[1], "-o"))
            return ArgInfo(argv[4], argv[2]);
    }

    // illegal arguments, print help info
    printHelp();
    exit(0);
}

int main(int argc, char **argv)
{
    ArgInfo argInfo = parseArg(argc, argv);

    std::unique_ptr<FE::CodeParser> codeParser = std::make_unique<FE::CodeParser>(argInfo.codePath);
    codeParser->scan();
    std::shared_ptr<IR::Stm> tree = codeParser->parse();
    // tree->Print(0);
    std::unique_ptr<BE::ChartEmitter> chartEmitter;
    if (!argInfo.doRedirect) {
        chartEmitter = std::make_unique<BE::ChartEmitter>(tree);
    }
    else {
        chartEmitter = std::make_unique<BE::ChartEmitter>(tree, argInfo.chartPath);
    }
    chartEmitter->attachChartInfo();
    chartEmitter->drawFlowChart();
    return 0;
}