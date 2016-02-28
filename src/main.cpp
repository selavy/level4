#include <iostream>
#include <boost/program_options.hpp>
#include "command_line.h"

int main(int argc, char **argv) {
    using namespace std;
    const auto vm = parse_cmdline(argc, argv);
    
    return 0;
}
