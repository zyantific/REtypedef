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

// ============================================================================================== //
// [SubstitutionManager]                                                                          //
// ============================================================================================== //

const std::regex SubstitutionManager::m_kMarkerFinder 
    = std::regex("\\$(\\d+)", std::regex_constants::optimize);

SubstitutionManager::SubstitutionManager()
{
    
}

SubstitutionManager::~SubstitutionManager()
{
    
}

void SubstitutionManager::addRule(const std::shared_ptr<Substitution> subst)
{
    m_rules.push_back(std::move(subst));
    emit entryAdded();
}

void SubstitutionManager::removeRule(const Substitution* subst)
{
    for (auto it = m_rules.begin(), end = m_rules.end(); it != end; ++it)
    {
        if (it->get() == subst)
        {
            it = m_rules.erase(it);
            emit entryDeleted();
            if (it == m_rules.end())
                break;
        }
    }
}

void SubstitutionManager::clearRules()
{
    if (m_rules.size())
    {
        m_rules.clear();
        emit entryDeleted();
    }
}

void SubstitutionManager::applyToString(char* str, uint outLen) const
{
    for (auto it = m_rules.cbegin(), end = m_rules.cend(); it != end; ++it)
    {
        std::cmatch groups;
        while (std::regex_match(str, groups, (*it)->regexp))
        {
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
            ::qstrncpy(str, processed.c_str(), outLen);
        }
    }
}

// ============================================================================================== //
