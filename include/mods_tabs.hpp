#pragma once
#include <borealis.hpp>
#include <vector>

struct ModInfo {
    public:
        std::string mod_path;
        std::string folder_name;
        std::string name;
        std::string author;
        std::string version;
        std::string description;
};

class ModsList{
    public:
        brls::List* arcModsList();
        std::vector<brls::ListItem*> skylinePlugins();
        brls::List* quasarWorkspaces();
};