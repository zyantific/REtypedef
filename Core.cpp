/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 athre0z
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Core.hpp"

#include "Config.hpp"
#include "Settings.hpp"
#include "Ui.hpp"
#include "ImportExport.hpp"

#include <QDir>
#include <QApplication>
#include <QMessageBox>
#include <idp.hpp>
#include <diskio.hpp>
#include <kernwin.hpp>
#include <loader.hpp>

// =============================================================================================== //
// [Core]                                                                                          //
// =============================================================================================== //

Core::Core()
    : m_originalMangler(nullptr)
{
    add_menu_item("Options", "Edit name substitutions...", nullptr, 0, 
        &Core::onOptionsMenuItemClicked, this);

    // First start? Initialize with default rules.
    Settings settings;
    if (settings.value(Settings::kFirstStart, true).toBool())
    {
        QSettings defaultRules(":/Misc/default_rules.ini", QSettings::IniFormat);
        SettingsImporterExporter importer(&m_substitutionManager, &defaultRules);
        importer.importRules();
        saveToSettings();
        settings.setValue(Settings::kFirstStart, false);
    }

    // Load rules from settings and subscribe to changes in the manager
    try
    {
        SettingsImporterExporter importer(&m_substitutionManager, &settings);
        importer.importRules();
    }
    catch (const SettingsImporterExporter::Error& e)
    {
        msg("[" PLUGIN_NAME "] Cannot load settings: %s\n", e.what());
    }
    connect(&m_substitutionManager, SIGNAL(entryAdded()), SLOT(saveToSettings()));
    connect(&m_substitutionManager, SIGNAL(entryDeleted()), SLOT(saveToSettings()));

    // Place demangler detour
    HMODULE hIdaWll = GetModuleHandleA("IDA.WLL");
    if (!hIdaWll)
        throw std::runtime_error("cannot find IDA.WLL");

    auto demangle = reinterpret_cast<demangler_t*>(GetProcAddress(hIdaWll, "demangle"));
    if (!demangle)
        throw std::runtime_error("cannot find exported function \"demangle\" in IDA.WLL");

    m_demanglerDetour.reset(new DemanglerDetour(demangle, &Core::demanglerHookCallback));
    m_demanglerDetour->attach(m_originalMangler);
}

Core::~Core()
{
    // Remove demangler detour
    try
    {
        m_demanglerDetour->detach();
    }
    catch (const DemanglerDetour::Error& /*e*/)
    {
        QMessageBox::critical(nullptr, PLUGIN_NAME, 
            "Critical: cannot detach internal hooks. Will terminate now.");
        std::terminate();
    }

    del_menu_item("Options/Edit name substitutions...");
}

void Core::runPlugin()
{

}

int32 Core::demanglerHookCallback(char* answer, uint answerLength, 
    const char* str, uint32 disableMask)
{
    auto &thiz = instance();
    auto ret = thiz.m_originalMangler(answer, answerLength, str, disableMask);

    //msg("str: %s; ret: 0x%08X\n", str, ret);

    if (answer && answerLength != 0)
        thiz.m_substitutionManager.applyToString(answer, answerLength);

    return ret;
}

bool Core::onOptionsMenuItemClicked(void* userData)
{
    auto thiz = reinterpret_cast<Core*>(userData);
    SubstitutionModel model(&thiz->m_substitutionManager);
    SubstitutionEditor editor(qApp->activeWindow());
    editor.setModel(&model);

    editor.exec();
    return 0;
}

void Core::saveToSettings()
{
    try
    {
        Settings settings;
        SettingsImporterExporter exporter(&m_substitutionManager, &settings);
        exporter.exportRules();

        request_refresh(IWID_NAMES | IWID_DISASMS);
    }
    catch (const SettingsImporterExporter::Error &e)
    {
        msg("[" PLUGIN_NAME "] Cannot save to settings: %s\n", e.what());
    }
}

// ============================================================================================== //
