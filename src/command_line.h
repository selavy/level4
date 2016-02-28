#ifndef COMMAND_LINE__H_
#define COMMAND_LINE__H_

namespace boost {
    namespace program_options {
        class variables_map;
    }
}

boost::program_options::variables_map parse_cmdline(int argc, char *argv[]);

#endif // COMMAND_LINE__H_
