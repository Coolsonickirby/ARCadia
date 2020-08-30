#include "mods_tabs.hpp"

#include <stdio.h>
#include <switch.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <borealis.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "mods_handler.hpp"
#include "inipp.h"
#include "sample_installer_page.hpp"
#include "sample_loading_page.hpp"

#define MODS_FOLDER "sdmc://UltimateModManager/mods/"
#define MODS_BACKUPS "sdmc://UltimateModManager/backups/"

void restart_app()
{
    envSetNextLoad("sdmc://switch/gui_ultimatemodmanager.nro", "");
    brls::Application::quit();
}

std::string set_plugin_state(std::string path)
{
    std::string pluginsDir       = ModsHandler::getPluginsPath();
    std::string enabled_plugins  = pluginsDir + "/plugins";
    std::string disabled_plugins = pluginsDir + "/disabled_plugins";

    std::string og_path = path;

    std::string parent_folder = path.substr(0, path.find_last_of("/\\"));
    parent_folder             = parent_folder.substr(parent_folder.find_last_of("/\\") + 1, parent_folder.length());

    if (parent_folder == "plugins")
    {

        auto copyOptions = std::filesystem::copy_options::overwrite_existing;

        std::ifstream ifs(path, std::ios::in | std::ios::binary);
        if (ifs.is_open())
        {
            std::ofstream ofs(std::string(disabled_plugins) + std::string(path.substr(path.find_last_of("/\\"), path.length())), std::ios::out | std::ios::binary);
            ofs << ifs.rdbuf();
        }
        else
        {
            return "Failed to open file!";
        }

        ifs.close();

        std::string remove_path = std::string(enabled_plugins) + std::string(path.substr(path.find_last_of("/\\"), path.length()));

        if (std::remove(remove_path.c_str()) != 0)
        {
            return "Failed removing plugin!";
        };

        return "Plugin Disabled!";
    }
    else
    {
        std::ifstream ifs(path, std::ios::in | std::ios::binary);
        if (ifs.is_open())
        {
            std::ofstream ofs(std::string(enabled_plugins) + std::string(path.substr(path.find_last_of("/\\"), path.length())), std::ios::out | std::ios::binary);
            ofs << ifs.rdbuf();
        }
        else
        {
            return "Failed to open file!";
        }

        ifs.close();

        std::string remove_path = std::string(disabled_plugins) + std::string(path.substr(path.find_last_of("/\\"), path.length()));

        if (std::remove(remove_path.c_str()) != 0)
        {
            return "Failed removing plugin!";
        };

        return "Plugin Enabled!";
    }
}

brls::List* ModsList::arcModsList()
{

    brls::List* arcModsList = new brls::List();

    for (auto& directoryPath : std::filesystem::directory_iterator(MODS_FOLDER))
    {
        std::string mod_path = directoryPath.path();
        std::string backup_path = std::string(MODS_BACKUPS) + std::string(directoryPath.path().filename());

        inipp::Ini<char> info_ini;

        std::string info_path = std::string(directoryPath.path()) + "/info.ini";

        std::string name;
        std::string author;
        std::string version;

        std::ifstream is(info_path);

        if (!is)
        {
            name    = directoryPath.path().filename();
            author  = "Unavaliable";
            version = "Unavaliable";
        }
        else
        {
            info_ini.parse(is);
            info_ini.default_section(info_ini.sections["ModInfo"]);

            inipp::extract(info_ini.sections["ModInfo"]["name"], name);
            inipp::extract(info_ini.sections["ModInfo"]["author"], author);
            inipp::extract(info_ini.sections["ModInfo"]["version"], version);
        }

        brls::ListItem* dialogItem = new brls::ListItem(name);

        dialogItem->getClickEvent()->subscribe([name, author, version, dialogItem, mod_path, backup_path](brls::View* view) {
            brls::Dialog* dialog = new brls::Dialog("Mod Name: " + std::string(name) + "\nAuthor Name: " + std::string(author) + "\nVersion: " + std::string(version));

            brls::GenericEvent::Callback uninstallCallback = [dialog, name, dialogItem, backup_path](brls::View* view) {
                dialog->close();

                dialogItem->setChecked(false);
            };

            brls::GenericEvent::Callback installCallback = [dialog, name, dialogItem, mod_path](brls::View* view) {
                dialog->close();

                dialogItem->setChecked(true);
            };

            dialog->addButton("Install", installCallback);
            dialog->addButton("Uninstall", uninstallCallback);

            dialog->setCancelable(true);

            dialog->open();
        });

        bool backup_exist = std::filesystem::is_directory(backup_path);

        if (backup_exist)
        {
            dialogItem->setChecked(true);
        }

        arcModsList->addView(dialogItem);
    }

    // brls::ListItem* installerItem = new brls::ListItem("Open example installer");
    // installerItem->getClickEvent()->subscribe([](brls::View* view) {
    //     brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
    //     stagedFrame->setTitle("My great installer");

    //     stagedFrame->addStage(new SampleLoadingPage(stagedFrame, "Installing Mod"));
    //     stagedFrame->addStage(new SampleInstallerPage(stagedFrame, "Done!"));

    //     brls::Application::pushView(stagedFrame);
    // });

    // arcModsList->addView(installerItem);

    return arcModsList;
}

std::vector<brls::View*> getPlugins()
{

    std::string pluginsDir       = ModsHandler::getPluginsPath();
    std::string enabled_plugins  = pluginsDir + "/plugins";
    std::string disabled_plugins = pluginsDir + "/disabled_plugins";

    std::vector<brls::View*> holder;

    if (!std::filesystem::exists(ModsHandler::getPluginsPath()))
    {
        brls::Label* noPlugins = new brls::Label(brls::LabelStyle::REGULAR, "Skyline Folder doesn't exist!", true);
        holder.push_back(noPlugins);
        return holder;
    }

    brls::Label* enabledLabel = new brls::Label(brls::LabelStyle::REGULAR, "Enabled Plugins", true);
    holder.push_back(enabledLabel);

    for (auto& directoryPath : std::filesystem::directory_iterator(pluginsDir + "/plugins"))
    {
        std::string mod_path = directoryPath.path();

        std::string name = directoryPath.path().filename();

        std::string ext = directoryPath.path().extension();

        brls::ListItem* pluginItem = new brls::ListItem(name);

        pluginItem->getClickEvent()->subscribe([name, mod_path](brls::View* view) {
            brls::Application::notify(set_plugin_state(mod_path));
        });

        holder.push_back(pluginItem);
    }

    brls::Label* disabledLabel = new brls::Label(brls::LabelStyle::REGULAR, "Disabled Plugins", true);
    holder.push_back(disabledLabel);

    for (auto& directoryPath : std::filesystem::directory_iterator(pluginsDir + "/disabled_plugins"))
    {
        std::string plugin_path = directoryPath.path();

        std::string name = directoryPath.path().filename();

        std::string ext = directoryPath.path().extension();

        brls::ListItem* pluginItem = new brls::ListItem(name);

        pluginItem->getClickEvent()->subscribe([name, plugin_path](brls::View* view) {
            brls::Application::notify(set_plugin_state(plugin_path));
        });

        holder.push_back(pluginItem);
    }

    return holder;
}

brls::List* ModsList::skylinePluginsList()
{

    std::vector<brls::View*> items = getPlugins();

    brls::List* skylineList = new brls::List();
     
    for (auto& item : items)
    {
        skylineList->addView(item);
    };

    skylineList->registerAction("Refresh List", brls::Key::L, [skylineList] {
        skylineList->clear();
        std::vector<brls::View*> items = getPlugins();
        for (auto& item : items)
        {
            skylineList->addView(item);
        };

        return skylineList;
    });

    return skylineList;
}