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

#ifndef INLINEDETOUR_HPP
#define INLINEDETOUR_HPP

#include "Utils.hpp"

#include <cstdint>
#include <stdexcept>
#include <cassert>
#include <Windows.h>

#include <Zydis/Zydis.h>

// ============================================================================================== //
// [InlineDetour]                                                                                 //
// ============================================================================================== //

template<typename Function>
class InlineDetour : public Utils::NonCopyable
{
    Function *m_target;
    void *m_callback;
    void *m_trampoline;
    size_t m_detourLen;
public:
    class Error : public std::runtime_error
        { public: explicit Error(const char *what) : std::runtime_error(what) {} };
public:
    InlineDetour(Function* target, Function* callback);
    virtual ~InlineDetour() {}
public:
    void attach(Function*& trampoline);
    void detach();
private:
    static size_t calculateTrampolineLength(uint8_t* target, size_t minLen, size_t maxLen);
};

// ============================================================================================= //
// Implementation of inline methods [InlineDetour]                                               //
// ============================================================================================= //

template<typename Function> inline
InlineDetour<Function>::InlineDetour(Function* target, Function* callback)
    : m_target(target)
    , m_callback(callback)
    , m_trampoline(nullptr)
    , m_detourLen(0)
{
    assert(target);
    assert(callback);
}

template<typename Function> inline
void InlineDetour<Function>::attach(Function*& trampoline)
{
    // Already attached?
    if (m_trampoline)
        throw Error("already attached");

    try
    {
        // Calculate length
        m_detourLen = calculateTrampolineLength(
            reinterpret_cast<uint8_t*>(m_target), 5, 128 /* TODO */);

        // Allocate trampoline and backup original code
        m_trampoline = VirtualAlloc(nullptr, m_detourLen + 5, MEM_COMMIT | MEM_RESERVE, 
            PAGE_EXECUTE_READWRITE);
        if (!m_trampoline)
            throw Error("cannot allocate RWX memory");
        memcpy(m_trampoline, m_target, m_detourLen);
        trampoline = reinterpret_cast<Function*>(m_trampoline);

        // Place E9-jmp at end of trampoline back to original function
        auto jumpBack = reinterpret_cast<uint32_t>(m_trampoline) + m_detourLen;
        *reinterpret_cast<uint8_t*>(jumpBack) = 0xE9;
        *reinterpret_cast<uint32_t*>(jumpBack + 1) 
            = reinterpret_cast<uint32_t>(m_target) + m_detourLen - jumpBack - 5;

        // Place E9-jmp in original function to user provided callback
        DWORD oldProt;
        if (!VirtualProtect(m_target, m_detourLen, PAGE_EXECUTE_READWRITE, &oldProt))
            throw Error("cannot RWX-protect memory");

        auto jumpTo = reinterpret_cast<uint32_t>(m_target);
        *reinterpret_cast<uint8_t*>(jumpTo) = 0xE9;
        *reinterpret_cast<uint32_t*>(jumpTo + 1)
            = reinterpret_cast<uint32_t>(m_callback) - jumpTo - 5;

        if (!VirtualProtect(m_target, m_detourLen, oldProt, &oldProt))
            throw Error("cannot restore memory protection");
    }
    catch (const Error& /*e*/)
    {
        if (m_trampoline)
        {
            VirtualFree(m_trampoline, 0, MEM_RELEASE);
            m_trampoline = nullptr;
            m_detourLen = 0;
            trampoline = nullptr;
        }
        throw;
    }
}

template<typename Function> inline
void InlineDetour<Function>::detach()
{
    if (!m_trampoline)
        throw Error("not attached");

    // Restore original code
    DWORD oldProt;
    if (!VirtualProtect(m_target, m_detourLen, PAGE_EXECUTE_READWRITE, &oldProt))
        throw Error("cannot RWX-protect memory");

    memcpy(m_target, m_trampoline, m_detourLen);

    if (!VirtualProtect(m_target, m_detourLen, oldProt, &oldProt))
        throw Error("cannot restore memory protection");

    // Free trampoline
    VirtualFree(m_trampoline, 0, MEM_RELEASE);
    m_trampoline = nullptr;
    m_detourLen = 0;
}

template<typename Function> inline
size_t InlineDetour<Function>::calculateTrampolineLength(
    uint8_t* target, size_t minLen, size_t maxLen)
{
    ZydisInstructionDecoder decoder;
    ZydisDecoderInitInstructionDecoder(
        &decoder,
        ZYDIS_MACHINE_MODE_LEGACY_32,
        ZYDIS_ADDRESS_WIDTH_INVALID
    );

    ZydisInstructionInfo nfo;
    ZydisStatus status;
    size_t offs = 0;
    do
    {
        status = ZydisDecoderDecodeBuffer(
            &decoder, target + offs, maxLen - offs, 0, &nfo
        );
        offs += nfo.length;
    } while (status != ZYDIS_STATUS_NO_MORE_DATA && offs < minLen);

    return offs;
}

// ============================================================================================= //

#endif // INLINEDETOUR_HPP
