#pragma once
#include <borealis.hpp>

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
        brls::List* skylinePluginsList();
};