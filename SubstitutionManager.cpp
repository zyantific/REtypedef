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

#include "SubstitutionManager.hpp"

#include "Settings.hpp"

#include <ida.hpp>
#include <kernwin.hpp>
#include <idp.hpp>

// ============================================================================================= //
// [SubstitutionManager]                                                                         //
// ============================================================================================= //

const std::regex SubstitutionManager::m_kMarkerFinder 
    = std::regex("\\$(\\d+)", std::regex_constants::optimize);

SubstitutionManager::SubstitutionManager()
{
    loadFromSettings();
}

SubstitutionManager::~SubstitutionManager()
{
    //saveToSettings();
}

void SubstitutionManager::add(const std::shared_ptr<Substitution> subst)
{
    m_substs.push_back(std::move(subst));
    saveToSettings();
}

void SubstitutionManager::remove(const Substitution *subst)
{
    for (auto it = m_substs.begin(), end = m_substs.end(); it != end; ++it)
    {
        if (it->get() == subst)
        {
            it = m_substs.erase(it);
            if (it == m_substs.end())
                break;
        }
    }
}

void SubstitutionManager::loadFromSettings()
{
    m_substs.clear();

    Settings settings;
    settings.beginGroup(settings.kSubstitutionGroup);
    int size = settings.beginReadArray(settings.kSubstitutionPrefix);
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        auto raw = settings.value(settings.kSubstitutionPrefix).toStringList();
        if (raw.size() == 2)
        {
            auto sbst = std::make_shared<Substitution>();
            sbst->replacement = raw.at(1).toAscii().data();
            sbst->regexpPattern = raw.at(0).toAscii().data();

            try
            {
                sbst->regexp = std::regex(sbst->regexpPattern, std::regex_constants::optimize);
            }
            catch (const std::regex_error &/*e*/) 
            {
                continue;
            }
            
            m_substs.push_back(std::move(sbst));
        }
    }
    settings.endArray();
    settings.endGroup();
}

void SubstitutionManager::saveToSettings() const
{
    Settings settings;
    settings.beginGroup(settings.kSubstitutionGroup);
    settings.beginWriteArray(settings.kSubstitutionPrefix, m_substs.size());
    int i = 0;
    for (auto it = m_substs.cbegin(), end = m_substs.cend(); it != end; ++it, ++i)
    {
        settings.setArrayIndex(i);
        QStringList slist;
        slist.append(QString::fromStdString((*it)->regexpPattern));
        slist.append(QString::fromStdString((*it)->replacement));
        settings.setValue(settings.kSubstitutionPrefix, slist);
    }
    settings.endArray();
    settings.endGroup();
}

void SubstitutionManager::applyToString(const char* in, char* out, uint outLen) const
{
    for (auto it = m_substs.cbegin(), end = m_substs.cend(); it != end; ++it)
    {
        std::cmatch groups;
        if (!std::regex_match(in, groups, (*it)->regexp))
            continue;
        
        auto processed = (*it)->replacement;
        std::smatch markerGroups;
        while (std::regex_search(processed, markerGroups, m_kMarkerFinder))
        {
            assert(markerGroups.size() == 2);
            auto idx = static_cast<unsigned int>(std::stoi(markerGroups[1]));
            if (idx >= groups.size())
                break;

            size_t pos = 0;
            std::string search = markerGroups[0];
            std::string replace = groups[idx];
            while((pos = processed.find(search, pos)) != std::string::npos) 
            {
                processed.replace(pos, search.length(), replace);
                pos += replace.length();
            }
        }
        ::qstrncpy(out, processed.c_str(), outLen);
    }
}

// ============================================================================================= //
