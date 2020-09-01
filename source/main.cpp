/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2020  natinusala
    Copyright (C) 2019  p-sam

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <switch.h>

#include <borealis.hpp>
#include <string>
#include "mods_tabs.hpp"
#include "config.hpp"

#define BOREALIS_APP_TITLE "Ultimate Manager"

int main(int argc, char* argv[])
{
    // Init the app
    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);

    if (!brls::Application::init(BOREALIS_APP_TITLE))
    {
        brls::Logger::error(std::string("Unable to init ") + std::string(BOREALIS_APP_TITLE));
        return EXIT_FAILURE;
    }

    // Create a sample view
    brls::TabFrame* rootFrame = new brls::TabFrame();
    rootFrame->setTitle(BOREALIS_APP_TITLE);
    rootFrame->setIcon(BOREALIS_ASSET("icon/icon.jpg"));


    ModsList mods;
    if(!Config::initConfig()){
        appletRequestLaunchApplication(0x01006A800016E000, NULL);
        brls::Application::quit();
    };

    if(Config::config_info.infos.version != "1.1.2"){
        rootFrame->setFooterText("Mismatched Config File!");
    }

    rootFrame->registerAction("Launch Smash Ultimate", brls::Key::X, [] {
        appletRequestLaunchApplication(0x01006A800016E000, NULL);
        return true;
    });

    brls::List* testList = new brls::List();

    brls::ListItem* arcPath = new brls::ListItem("Notify ARC Path");
    arcPath->getClickEvent()->subscribe([](brls::View* view) {
        brls::Application::notify(Config::config_info.paths.arc);
    });
    
    brls::ListItem* streamPath = new brls::ListItem("Notify Stream Path");
    streamPath->getClickEvent()->subscribe([](brls::View* view) {
        brls::Application::notify(Config::config_info.paths.stream);
    });
    
    brls::ListItem* ummPath = new brls::ListItem("Notify UMM Path");
    ummPath->getClickEvent()->subscribe([](brls::View* view) {
        brls::Application::notify(Config::config_info.paths.umm);
    });

    testList->addView(arcPath);
    testList->addView(streamPath);
    testList->addView(ummPath);

    rootFrame->addTab("First Tab", testList);

    rootFrame->addTab("UMM Tab", mods.arcModsList());

    // rootFrame->addTab("Mods", mods.arcModsList());
    // rootFrame->addTab("Skyline Plugins", mods.skylinePluginsList());

    // Add the root view to the stack
    brls::Application::pushView(rootFrame);

    // Run the app
    while (brls::Application::mainLoop())
        ;

    // Exit
    return EXIT_SUCCESS;
}

// brls::List* testList = new brls::List();

// brls::ListItem* dialogItem = new brls::ListItem("Open a dialog");
// dialogItem->getClickEvent()->subscribe([](brls::View* view) {
//     brls::Dialog* dialog = new brls::Dialog("Warning: PozzNX will wipe all data on your Switch and render it inoperable, do you want to proceed?");

//     brls::GenericEvent::Callback closeCallback = [dialog](brls::View* view) {
//         dialog->close();
//         brls::Application::notify("Running PozzNX...");
//     };

//     dialog->addButton("Continue", closeCallback);
//     dialog->addButton("Continue", closeCallback);
//     dialog->addButton("Continue", closeCallback);

//     dialog->setCancelable(false);

//     dialog->open();
// });

// brls::ListItem* notificationItem = new brls::ListItem("Post a random notification");
// notificationItem->getClickEvent()->subscribe([](brls::View* view) {
//     std::string notification = NOTIFICATIONS[std::rand() % NOTIFICATIONS.size()];
//     brls::Application::notify(notification);
// });

// brls::ListItem* themeItem = new brls::ListItem("TV Resolution");
// themeItem->setValue("Automatic");

// brls::SelectListItem* jankItem = new brls::SelectListItem(
//     "User Interface Jank",
//     { "Native", "Minimal", "Regular", "Maximum", "SX OS", "Windows Vista", "iOS 14" });

// brls::ListItem* crashItem = new brls::ListItem("Divide by 0", "Can the Switch do it?");
// crashItem->getClickEvent()->subscribe([](brls::View* view) { brls::Application::crash("The software was closed because an error occured:\nSIGABRT (signal 6)"); });

// brls::ListItem* popupItem = new brls::ListItem("Open popup");
// popupItem->getClickEvent()->subscribe([](brls::View* view) {
//     brls::TabFrame* popupTabFrame = new brls::TabFrame();
//     popupTabFrame->addTab("Red", new brls::Rectangle(nvgRGB(255, 0, 0)));
//     popupTabFrame->addTab("Green", new brls::Rectangle(nvgRGB(0, 255, 0)));
//     popupTabFrame->addTab("Blue", new brls::Rectangle(nvgRGB(0, 0, 255)));
//     brls::PopupFrame::open("Popup title", BOREALIS_ASSET("icon/borealis.jpg"), popupTabFrame, "Subtitle left", "Subtitle right");
// });

// brls::ListItem* installerItem = new brls::ListItem("Open example installer");
// installerItem->getClickEvent()->subscribe([](brls::View* view) {
//     brls::StagedAppletFrame* stagedFrame = new brls::StagedAppletFrame();
//     stagedFrame->setTitle("My great installer");

//     stagedFrame->addStage(new SampleInstallerPage(stagedFrame, "Go to step 2"));
//     stagedFrame->addStage(new SampleLoadingPage(stagedFrame));
//     stagedFrame->addStage(new SampleInstallerPage(stagedFrame, "Finish"));

//     brls::Application::pushView(stagedFrame);
// });

// brls::SelectListItem* layerSelectItem = new brls::SelectListItem("Select Layer", { "Layer 1", "Layer 2" });

// testList->addView(dialogItem);
// testList->addView(notificationItem);
// testList->addView(themeItem);
// testList->addView(jankItem);
// testList->addView(crashItem);
// testList->addView(installerItem);
// testList->addView(popupItem);

// brls::Label* testLabel = new brls::Label(brls::LabelStyle::REGULAR, "For more information about how to use Nintendo Switch and its features, please refer to the Nintendo Support Website on your smart device or PC.", true);
// testList->addView(testLabel);

// brls::ListItem* actionTestItem = new brls::ListItem("Custom Actions");
// actionTestItem->registerAction("Show notification", brls::Key::L, [] {
//     brls::Application::notify("Custom Action triggered");
//     return true;
// });
// testList->addView(actionTestItem);

// brls::LayerView* testLayers = new brls::LayerView();
// brls::List* layerList1      = new brls::List();
// brls::List* layerList2      = new brls::List();

// layerList1->addView(new brls::Header("Layer 1", false));
// layerList1->addView(new brls::ListItem("Item 1"));
// layerList1->addView(new brls::ListItem("Item 2"));
// layerList1->addView(new brls::ListItem("Item 3"));

// layerList2->addView(new brls::Header("Layer 2", false));
// layerList2->addView(new brls::ListItem("Item 1"));
// layerList2->addView(new brls::ListItem("Item 2"));
// layerList2->addView(new brls::ListItem("Item 3"));

// testLayers->addLayer(layerList1);
// testLayers->addLayer(layerList2);

// layerSelectItem->getValueSelectedEvent()->subscribe([=](size_t selection) {
//     testLayers->changeLayer(selection);
// });

// testList->addView(layerSelectItem);

// rootFrame->addTab("First tab", testList);
// rootFrame->addTab("Second tab", testLayers);
// rootFrame->addSeparator();
// rootFrame->addTab("Third tab", new brls::Rectangle(nvgRGB(255, 0, 0)));
// rootFrame->addTab("Fourth tab", new brls::Rectangle(nvgRGB(0, 255, 0)));
