#pragma once
#include "gl_page_data.hpp"
namespace GLPageGuard {
inline bool matches(const void* page, unsigned long long fileOffset)
{
    const auto *p=static_cast<const unsigned char *>(page);
    return p && fileOffset==GLPageData::FilePageOffset
        && memcmp(p+GLPageData::FunctionInPage,GLPageData::Original,sizeof(GLPageData::Original))==0
        && memcmp(p,GLPageData::Page,sizeof(GLPageData::Page))==0;
}
}
