#pragma once
#include <borealis.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <switch.h>
#include <toml.hpp>

#define CONFIG_PATH "sdmc://atmosphere/contents/01006A800016E000/romfs/arcropolis.toml"

class Infos {
    public:
        std::string version;
};

class Paths{
    public:
        std::string arc;
        std::string stream;
        std::string umm;
};

class Miscellaneous{
    public:
        bool debug;
        bool mowjoh;
};

class ConfigLayout {
    public:
        Infos infos;
        Paths paths;
        Miscellaneous misc;
};


class Config {
    public:
        inline static ConfigLayout config_info;
        static bool initConfig(){
            if(!std::filesystem::exists(CONFIG_PATH)){
                return false;
            }else{
                auto config_data = toml::parse(CONFIG_PATH);

                const auto& info_table = toml::find(config_data, "infos");
                config_info.infos.version  = toml::find<std::string>(info_table, "version");
                
                const auto& paths_table = toml::find(config_data, "paths");
                config_info.paths.arc  = toml::find<std::string>(paths_table, "arc");
                config_info.paths.stream  = toml::find<std::string>(paths_table, "stream");
                config_info.paths.umm  = toml::find<std::string>(paths_table, "umm");
                
                const auto& misc_table = toml::find(config_data, "misc");
                config_info.misc.debug  = toml::find_or<bool>(misc_table, "debug", false);
                config_info.misc.mowjoh  = toml::find_or<bool>(misc_table, "mowjoh", false);

                return true;
            }
        };
};