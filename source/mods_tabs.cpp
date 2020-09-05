#include "mods_tabs.hpp"

#include <stdio.h>
#include <switch.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <borealis.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>

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

bool compareFunction (const ModInfo &lhs, const ModInfo &rhs) {return lhs.name<rhs.name;}

brls::List* ModsList::arcModsList()
{

    brls::List* arcModsList = new brls::List();

    bool is_empty = true;
    std::string base_path = replace(Config::config_info.paths.umm, "sd:", "sdmc:");


    if(!std::filesystem::exists(base_path)){
        brls::ListItem* noFolderFound = new brls::ListItem(std::string(Config::config_info.paths.umm + " doesn't exist!"));
        arcModsList->addView(noFolderFound);
        return arcModsList;
    }

    std::vector<ModInfo> umm_children;


    for (auto& directoryPath : std::filesystem::directory_iterator(base_path))
    {
        is_empty = false;
        ModInfo info;
        std::string mod_path = directoryPath.path();

        inipp::Ini<char> info_ini;

        std::string info_path = std::string(directoryPath.path()) + "/info.ini";

        info.mod_path = mod_path;
        info.folder_name = directoryPath.path().filename();
        std::string name;
        std::string author;
        std::string version;
        std::string description;

        std::ifstream is(info_path);

        if (!is)
        {
            name        = clean_dot(info.folder_name);
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

        info.name = name;
        info.author = author;
        info.version = version;
        info.description = description;

        umm_children.push_back(info);
    }

    if(ARCadiaConfig::sort_option == "name"){
        std::sort(umm_children.begin(),umm_children.end(),compareFunction);
    }

    if(ARCadiaConfig::sort_desc){
        std::reverse(umm_children.begin(), umm_children.end());
    }

    for (ModInfo umm_child : umm_children )
    {
        std::string mod_path = umm_child.mod_path;

        std::string clean_folder_name = clean_dot(umm_child.folder_name);
        std::string name = umm_child.name;
        std::string author = umm_child.author;
        std::string version = umm_child.version;
        std::string description = umm_child.description;

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

        if (umm_child.folder_name.at(0) == '.')
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