// Copyright (c) 2019 chendi
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>

static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                if (!(ch >= -1 && ch <= 255))
                {
                    return true;
                }
                return !std::isspace(ch);
            }));
}

static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
                if (!(ch >= -1 && ch <= 255))
                {
                    return true;
                }
                return !std::isspace(ch);
            })
                .base(),
            s.end());
}

static inline void trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
}

static inline std::vector<std::string> split_string(const std::string &s, const std::string &c)
{
    std::vector<std::string> v;
    std::string::size_type pos1;
    std::string::size_type pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
    {
        v.push_back(s.substr(pos1));
    }
    return v;
};