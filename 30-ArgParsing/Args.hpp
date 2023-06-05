#ifndef ARGS_PARSER_HPP
#define ARGS_PARSER_HPP

#include <string>
#include <vector>
#include <unordered_map>

namespace Args
{
    class Parser
    {
    public:
        Parser();

        template <typename T>
        void AddPositional(const std::string& shortName, const std::string& longName, const std::string& help, const T& default = T());
        template <typename T>
        void AddPositional(const std::string& shortName, const std::string& longName, const std::string& help, const T& default = T());

        bool ParseArgs(int argc, char **argv);
        std::string GetHelp() const;
    private:
        std::vector<std::string> mPositionals;
    };
}

#endif // ARGS_PARSER_HPP
