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

#ifndef CORE_HPP
#define CORE_HPP

#include "Utils.hpp"
#include "InlineDetour.hpp"
#include "SubstitutionManager.hpp"

#include <QObject>
#include <ida.hpp>
#include <demangle.hpp>
#include <memory>
#include <kernwin.hpp>

// ============================================================================================== //
// [Core]                                                                                         //
// ============================================================================================== //

/**
 * @brief   Plugin core singleton.
 */
class Core : public QObject, public Utils::Singleton<Core>
{
    Q_OBJECT

    SubstitutionManager m_substitutionManager;
    typedef InlineDetour<demangler_t> DemanglerDetour;
    std::unique_ptr<DemanglerDetour> m_demanglerDetour;
    demangler_t *m_originalMangler;
public:
    /**
     * @brief   Default constructor.
     */
    Core();
    /**
     * @brief   Destructor.
     */
    ~Core();
    /**
     * @brief   Runs the plugin.
     */
    void runPlugin();
    /**
     * @brief   Replacement function for IDA's @c demangle routine.
     * @param   answer          The output buffer for the demangled name.
     * @param   answerLength    Length of @c answer bufer.
     * @param   str             The mangled name.
     * @param   disableMask     See @c demangle.hpp in IDA SDK.
     * @return  See @c demangle.hpp in IDA SDK.
     */
    static int32 idaapi demanglerHookCallback(char* answer, uint answerLength, 
        const char* str, uint32 disableMask);
private:
#if IDA_SDK_VERSION >= 670
    struct OptionsMenuItemClickedAction : public action_handler_t
    {
        int idaapi activate(action_activation_ctx_t *ctx);
        action_state_t idaapi update(action_update_ctx_t *ctx);
    } m_optionsMenuItemClickedAction;
#endif
    /**
     * @brief   Handles clicks on the "Edit name substitutions" menu entry in IDA.
     * @param   userData    @c this.
     * @return  Always 0.
     */
    static bool idaapi onOptionsMenuItemClicked(void* userData);
private slots:
    /**
     * @brief   Saves the rules from the substitution manager to the settings.
     */
    void saveToSettings();
};

// ============================================================================================== //

#endif