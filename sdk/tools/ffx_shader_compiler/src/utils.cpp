// This file is part of the FidelityFX SDK.
//
// Copyright (C) 2024 Advanced Micro Devices, Inc.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "utils.h"

std::string WCharToUTF8(const std::wstring& wstr)
{
    if (wstr.empty())
        return std::string();

    iconv_t cd = iconv_open("UTF-8", "WCHAR_T");
    if (cd == (iconv_t)-1) {
        throw std::runtime_error("iconv_open failed");
    }

    // Input buffer
    size_t inBytesLeft = wstr.size() * sizeof(wchar_t);
    char* inBuf = reinterpret_cast<char*>(const_cast<wchar_t*>(wstr.data()));

    // Output buffer (UTF-8 may expand, so allocate generously)
    size_t outBytesLeft = inBytesLeft * 4;
    std::string str(outBytesLeft, '\0');
    char* outBuf = str.data();

    // Perform conversion
    if (iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft) == (size_t)-1) {
        iconv_close(cd);
        throw std::runtime_error(std::string("iconv failed"));
    }

    iconv_close(cd);

    // Resize to actual converted length
    str.resize(str.size() - outBytesLeft);

    return str;
}

std::wstring UTF8ToWChar(const std::string& str)
{
    if (str.empty())
        return std::wstring();

    // int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), nullptr, 0);

    // std::wstring wstr;
    // wstr.resize(size);
    // MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), &wstr[0], size);

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    // Convert wstring to UTF-8 string
    std::wstring wstr = converter.from_bytes(str);

    return wstr;
}
