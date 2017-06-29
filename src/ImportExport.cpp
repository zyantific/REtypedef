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

#include "ImportExport.hpp"

#include "Settings.hpp"
#include "SubstitutionManager.hpp"
#include "Config.hpp"

#include <cassert>
#include <ida.hpp>
#include <idp.hpp>

// ============================================================================================== //
// [SettingsImporterExporter]                                                                     //
// ============================================================================================== //

SettingsImporterExporter::SettingsImporterExporter(
        SubstitutionManager* manager, QSettings* settings)
    : m_manager(manager)
    , m_settings(settings)
{
    assert(manager);
    assert(settings);
}

void SettingsImporterExporter::importRules() const
{
    assert(m_manager);

    int size = m_settings->beginReadArray(Settings::kSubstitutionGroup);
    for (int i = 0; i < size; ++i)
    {
        m_settings->setArrayIndex(i);
        auto sbst = std::make_shared<Substitution>();
        sbst->replacement 
            = m_settings->value(Settings::kSubstitutionReplacement).toString().toStdString();
        sbst->regexpPattern 
            = m_settings->value(Settings::kSubstitutionPattern).toString().toStdString();

        try
        {
            sbst->regexp = std::regex(sbst->regexpPattern, std::regex_constants::optimize);
        }
        catch (const std::regex_error &e) 
        {
            msg("[" PLUGIN_NAME "] Cannot import entry, invalid regexp: %s\n", e.what());
            continue;
        }
        
        const auto& substs = m_manager->rules();
        auto it = std::find_if(substs.cbegin(), substs.cend(), 
            [&sbst](const std::shared_ptr<Substitution>& cur) -> bool
        {
            assert(cur);
            return cur->regexpPattern == sbst->regexpPattern;
        });

        if (it != substs.cend())
            continue;

        m_manager->addRule(std::move(sbst));
    }
    m_settings->endArray();
}

void SettingsImporterExporter::exportRules() const
{
    assert(m_manager);

    const auto& rules = m_manager->rules();
    m_settings->beginWriteArray(Settings::kSubstitutionGroup, rules.size());
    int i = 0;
    for (auto it = rules.cbegin(), end = rules.cend(); it != end; ++it, ++i)
    {
        m_settings->setArrayIndex(i);
        m_settings->setValue(Settings::kSubstitutionPattern, 
            QString::fromStdString((*it)->regexpPattern));
        m_settings->setValue(Settings::kSubstitutionReplacement,
            QString::fromStdString((*it)->replacement));
    }
    m_settings->endArray();
}

// ============================================================================================== //
// 