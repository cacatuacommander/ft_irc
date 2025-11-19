#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include "user.hpp"

class Parser {
    private:
        Parser();
    public:
        struct Command {
            std::string name;
            std::vector<std::string> params;
            std::string trailing;
            bool valid;
        };
        static bool cmd_exist(const std::string& cmd);
        static Command parse(const std::string& input, const std::vector<User>& usr_vec, const int fd);
};

#endif
