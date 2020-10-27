#include "mods_tabs.hpp"

#include <stdio.h>
#include <switch.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#include <borealis.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "config.hpp"
#include "inipp.h"

#define QUASAR_WORKSPACES "sdmc://Quasar"

std::string replace(std::string str, const std::string from, const std::string to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return str;
    str.replace(start_pos, from.length(), to);
    return str;
}

int move_file(std::string og_path, std::string out_path)
{
    std::ifstream in(og_path, std::ios::in | std::ios::binary);
    std::ofstream out(out_path, std::ios::out | std::ios::binary);
    out << in.rdbuf();

    in.close();
    out.close();

    return std::remove(og_path.c_str());
}

std::string clean_dot(std::string str)
{
    if (str.at(0) == '.')
        return str.substr(1, str.length());
    else
        return str;
}

bool compareFunction(const ModInfo& lhs, const ModInfo& rhs) { return lhs.name < rhs.name; }

brls::List* ModsList::arcModsList()
{

    brls::List* arcModsList = new brls::List();

    bool is_empty         = true;
    std::string base_path = replace(Config::config_info.paths.umm, "sd:", "sdmc:");

    if (!std::filesystem::exists(base_path))
    {
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

        info.mod_path    = mod_path;
        info.folder_name = directoryPath.path().filename();
        std::string name;
        std::string author;
        std::string version;
        std::string description;

        std::ifstream is(info_path);

        if (!is)
        {
          //If no info.ini exists
            name        = clean_dot(info.folder_name);
            author      = "Unavailable";
            version     = "Unavailable";
            description = "Unavailable";
        }
        else
        {
          //If info.ini does exist
            info_ini.parse(is);
            info_ini.default_section(info_ini.sections["ModInfo"]);

            inipp::extract(info_ini.sections["ModInfo"]["name"], name);
            inipp::extract(info_ini.sections["ModInfo"]["author"], author);
            inipp::extract(info_ini.sections["ModInfo"]["version"], version);
            inipp::extract(info_ini.sections["ModInfo"]["description"], description);
        }

        info.name        = name;
        info.author      = author;
        info.version     = version;
        info.description = description;

        umm_children.push_back(info);
    }

    if (ARCadiaConfig::sort_option == "name")
        std::sort(umm_children.begin(), umm_children.end(), compareFunction);

    if (ARCadiaConfig::sort_desc)
        std::reverse(umm_children.begin(), umm_children.end());

    for (ModInfo umm_child : umm_children)
    {
        std::string mod_path = umm_child.mod_path;

        std::string clean_folder_name = clean_dot(umm_child.folder_name);
        std::string name              = umm_child.name;
        std::string author            = umm_child.author;
        std::string version           = umm_child.version;
        std::string description       = umm_child.description;

        brls::ListItem* dialogItem = new brls::ListItem(name);

        dialogItem->registerAction("Show Info", brls::Key::Y, [name, author, version, description] {
            brls::Dialog* dialog = new brls::Dialog("Mod Name: " + std::string(name) + "\nAuthor Name: " + std::string(author) + "\nVersion: " + std::string(version) + "\nDescription: " + std::string(description));

            brls::GenericEvent::Callback toggleCallback = [dialog](brls::View* view) {
                dialog->close();
            };

            dialog->addButton("Close", toggleCallback);
            dialog->setCancelable(true);

            dialog->open();
            return true;
        });

        dialogItem->getClickEvent()->subscribe([name, author, version, description, dialogItem, mod_path, clean_folder_name, base_path](brls::View* view) {
            std::string disabled_folder_path = base_path + "/." + clean_folder_name;
            std::string enabled_folder_path  = base_path + "/" + clean_folder_name;

            if (std::filesystem::exists(disabled_folder_path))
            {
                rename(disabled_folder_path.c_str(), enabled_folder_path.c_str());
                dialogItem->setChecked(true);
            }
            else if (std::filesystem::exists(enabled_folder_path))
            {
                rename(enabled_folder_path.c_str(), disabled_folder_path.c_str());
                dialogItem->setChecked(false);
            }
            else
            {
                brls::Application::notify(disabled_folder_path);
            }
        });

        if (umm_child.folder_name.at(0) == '.')
        {
            dialogItem->setChecked(false);
        }
        else
        {
            dialogItem->setChecked(true);
        }

        arcModsList->addView(dialogItem);
    }

    if(is_empty){
        //No mod folders found
        brls::ListItem* noFolderFound = new brls::ListItem("No Mod Folders");
        arcModsList->addView(noFolderFound);
        return arcModsList;
    }

    return arcModsList;
}

brls::List* ModsList::quasarWorkspaces()
{
    brls::List* quasarWorkspacesList = new brls::List();

    brls::ListItem* dialogItem = new brls::ListItem("Default (sd:/ultimate/mods)");

    if(Config::config_info.paths.umm == "sd:/ultimate/mods")
        dialogItem->setChecked(true);

    dialogItem->getClickEvent()->subscribe([](brls::View* view) {
        Config::config_info.paths.umm = "sd:/ultimate/mods";
        Config::saveConfig();
        brls::Application::quit();
    });

    quasarWorkspacesList->addView(dialogItem);

    if (!std::filesystem::exists(QUASAR_WORKSPACES))
        return quasarWorkspacesList;

    std::vector<ModInfo> quasar_workspaces;

    for (auto& directoryPath : std::filesystem::directory_iterator(QUASAR_WORKSPACES))
    {
        ModInfo info;

        info.mod_path    = replace(directoryPath.path(), "sdmc:", "sd:");
        info.folder_name = directoryPath.path().filename();

        quasar_workspaces.push_back(info);
    }

    if (ARCadiaConfig::sort_option == "name")
        std::sort(quasar_workspaces.begin(), quasar_workspaces.end(), compareFunction);

    if (ARCadiaConfig::sort_desc)
        std::reverse(quasar_workspaces.begin(), quasar_workspaces.end());

    for (ModInfo quasar_workspace : quasar_workspaces)
    {
        brls::ListItem* dialogItem = new brls::ListItem(quasar_workspace.folder_name);

        if(quasar_workspace.mod_path == Config::config_info.paths.umm)
            dialogItem->setChecked(true);
        

        // dialogItem->registerAction("Show Info", brls::Key::Y, [name, author, version, description] {
        //     return true;
        // });

        dialogItem->getClickEvent()->subscribe([quasar_workspace](brls::View* view) {
            brls::Application::notify(quasar_workspace.mod_path);
            Config::config_info.paths.umm = quasar_workspace.mod_path;
            Config::saveConfig();
            brls::Application::quit();
        });

        quasarWorkspacesList->addView(dialogItem);
    }



    return quasarWorkspacesList;
}

// REWORK THIS!
std::vector<brls::ListItem*> ModsList::skylinePlugins()
{
    std::vector<brls::ListItem*> skylineList;

    std::string enabled_path  = "sdmc://atmosphere/contents/01006A800016E000/romfs/skyline/plugins/";
    std::string disabled_path = "sdmc://atmosphere/contents/01006A800016E000/romfs/skyline/disabled_plugins/";

    if (std::filesystem::exists(enabled_path))
    {
        for (auto& directoryPath : std::filesystem::directory_iterator(enabled_path))
        {
            std::string pluginFilename = directoryPath.path().filename();
            brls::ListItem* dialogItem = new brls::ListItem(pluginFilename);
            dialogItem->setChecked(true);

            dialogItem->getClickEvent()->subscribe([enabled_path, disabled_path, pluginFilename, dialogItem](brls::View* view) {
                std::string disabled_plugin_path = enabled_path + "/" + pluginFilename;
                std::string enabled_plugin_path  = disabled_path + "/" + pluginFilename;

                if (std::filesystem::exists(disabled_plugin_path))
                {
                    if (move_file(disabled_plugin_path, enabled_plugin_path) != 0)
                        brls::Application::notify("Failed enabling plugin!");
                    else
                        dialogItem->setChecked(false);
                }
                else if (std::filesystem::exists(enabled_plugin_path))
                {
                    if (move_file(enabled_plugin_path, disabled_plugin_path) != 0)
                        brls::Application::notify("Failed disabling plugin!");
                    else
                        dialogItem->setChecked(true);
                }
                else
                {
                    brls::Application::notify("Failed");
                };
            });
            skylineList.push_back(dialogItem);
        }
    }
    else
    {
        std::filesystem::create_directories(enabled_path);
    }

    if (std::filesystem::exists(disabled_path))
    {
        for (auto& directoryPath : std::filesystem::directory_iterator(disabled_path))
        {
            std::string pluginFilename = directoryPath.path().filename();
            brls::ListItem* dialogItem = new brls::ListItem(pluginFilename);

            dialogItem->getClickEvent()->subscribe([enabled_path, disabled_path, pluginFilename, dialogItem](brls::View* view) {
                std::string disabled_plugin_path = enabled_path + "/" + pluginFilename;
                std::string enabled_plugin_path  = disabled_path + "/" + pluginFilename;

                if (std::filesystem::exists(disabled_plugin_path))
                {
                    if (move_file(disabled_plugin_path, enabled_plugin_path) != 0)
                        brls::Application::notify("Failed enabling plugin!");
                    else
                        dialogItem->setChecked(false);
                }
                else if (std::filesystem::exists(enabled_plugin_path))
                {
                    if (move_file(enabled_plugin_path, disabled_plugin_path) != 0)
                        brls::Application::notify("Failed disabling plugin!");
                    else
                        dialogItem->setChecked(true);
                }
                else
                {
                    brls::Application::notify("Failed");
                };
            });
            skylineList.push_back(dialogItem);
        }
    }
    else
    {
        std::filesystem::create_directories(disabled_path);
    }

    return skylineList;
}