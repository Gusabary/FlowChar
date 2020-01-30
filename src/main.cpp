#include <iostream>
#include <cstdlib>

struct ArgInfo {
    std::string codePath;
    bool doRedirect;
    std::string chartPath;

    ArgInfo(std::string codePath) : codePath(codePath), doRedirect(false) {}
    ArgInfo(std::string codePath, std::string chartPath) : codePath(codePath), doRedirect(true), chartPath(chartPath) {}
};

void printHelp() {
    std::cout << "flowchar - a tiny tool for conversion from pseudo-code to flowchart in character format" << std::endl << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "\t flowchar -c <codePath> [-o <chartPath>]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "\t -c <path> \t specify path of code to be parsed, required." << std::endl;
    std::cout << "\t -o <path> \t specify path of chart to be emitted, optional. If absent, print to stdout." << std::endl;
}

ArgInfo parseArg(int argc, char **argv) {
    // -c <path>, specify path of code to be parsed, required.
    // -o <path>, specify path of chart to be emitted, optional. If absent, print to stdout. 
    
    if (argc == 3 && argv[1] == "-c") {
        // must be -c
        return ArgInfo(argv[2]);
    }
    
    if (argc == 5) {
        // both -c and -o
        if (argv[1] == "-c" && argv[3] == "-o")
            return ArgInfo(argv[2], argv[4]);
        if (argv[1] == "-o" && argv[3] == "-c")
            return ArgInfo(argv[4], argv[2]);
    }

    // illegal arguments, print help info
    printHelp();
    exit(0);
}

int main(int argc, char **argv)
{
    // std::cout << argc << std::endl;
    ArgInfo argInfo = parseArg(argc, argv);
    return 0;
}