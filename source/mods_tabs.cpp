#include "mods_tabs.hpp"

#include <stdio.h>
#include <switch.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <borealis.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "inipp.h"
#include "config.hpp"

std::string replace(std::string str, const std::string from, const std::string to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return str;
    str.replace(start_pos, from.length(), to);
    return str;
}

std::string clean_dot(std::string str){
    if(str.at(0) == '.')
        return str.substr(1, str.length());
    else
        return str;
}

brls::List* ModsList::arcModsList()
{

    brls::List* arcModsList = new brls::List();

    bool is_empty = true;
    std::string base_path = replace(Config::config_info.paths.umm, "sd:", "sdmc:");

    for (auto& directoryPath : std::filesystem::directory_iterator(base_path))
    {
        is_empty = false;
        std::string mod_path = directoryPath.path();

        inipp::Ini<char> info_ini;

        std::string info_path = std::string(directoryPath.path()) + "/info.ini";

        std::string clean_folder_name = clean_dot(directoryPath.path().filename());
        std::string name;
        std::string author;
        std::string version;
        std::string description;

        std::ifstream is(info_path);

        if (!is)
        {
            name        = clean_folder_name;
            author      = "Unavaliable";
            version     = "Unavaliable";
            description = "Unavaliable";
        }
        else
        {
            info_ini.parse(is);
            info_ini.default_section(info_ini.sections["ModInfo"]);

            inipp::extract(info_ini.sections["ModInfo"]["name"], name);
            inipp::extract(info_ini.sections["ModInfo"]["author"], author);
            inipp::extract(info_ini.sections["ModInfo"]["version"], version);
            inipp::extract(info_ini.sections["ModInfo"]["description"], description);
        }

        brls::ListItem* dialogItem = new brls::ListItem(name);

        dialogItem->getClickEvent()->subscribe([name, author, version, description, dialogItem, mod_path, clean_folder_name, base_path](brls::View* view) {
            brls::Dialog* dialog = new brls::Dialog("Mod Name: " + std::string(name) + "\nAuthor Name: " + std::string(author) + "\nVersion: " + std::string(version) + "\nDescription: " + std::string(description));

            brls::GenericEvent::Callback toggleCallback = [dialog, name, dialogItem, clean_folder_name, base_path](brls::View* view) {
                dialog->close();

                dialogItem->setChecked(true);

                std::string disabled_folder_path = base_path + "\\." + clean_folder_name;
                std::string enabled_folder_path = base_path + "\\" + clean_folder_name;

                if(std::filesystem::exists(disabled_folder_path)){
                    rename(disabled_folder_path.c_str(), enabled_folder_path.c_str());
                    dialogItem->setChecked(true);
                }else if(std::filesystem::exists(enabled_folder_path)){
                    rename(enabled_folder_path.c_str(), disabled_folder_path.c_str());
                    dialogItem->setChecked(false);
                }else {
                    brls::Application::notify("Failed toggling mod!");
                }
            };

            dialog->addButton("Toggle", toggleCallback);

            dialog->setCancelable(true);

            dialog->open();
        });

        if (std::string(directoryPath.path().filename()).at(0) == '.')
        {
            dialogItem->setChecked(false);
        } else {
            dialogItem->setChecked(true);
        }

        arcModsList->addView(dialogItem);
    }

    if(is_empty){
        brls::ListItem* noFolderFound = new brls::ListItem(std::string("No Mod Folders found in ") + Config::config_info.paths.umm);
        arcModsList->addView(noFolderFound);
        return arcModsList;
    }

    return arcModsList;
}