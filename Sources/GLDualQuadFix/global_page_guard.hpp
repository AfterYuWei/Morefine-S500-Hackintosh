#pragma once
#include <string.h>
#include "global_page_data.hpp"

namespace GlobalPageGuard {
inline bool candidateOffset(unsigned long long offset) {
    for (const auto& e : GlobalPageData::Entries) if (offset == e.fileOffset) return true;
    return false;
}
inline const GlobalPageData::Entry* find(const char* path, unsigned long long offset,
                                         const void* page, bool metalEnabled) {
    if (!path || !page) return nullptr;
    for (const auto& e : GlobalPageData::Entries) {
        if ((e.metal && !metalEnabled) || offset != e.fileOffset) continue;
        bool pathMatches = false;
        for (unsigned i = 0; i < e.pathCount; ++i) if (strcmp(path,e.paths[i]) == 0) { pathMatches=true; break; }
        if (pathMatches && memcmp(page,e.original,GlobalPageData::PageSize) == 0) return &e;
    }
    return nullptr;
}
template <typename Original, typename Vnode, typename Pager, typename Offset, typename Apply>
inline void validate(Original original, Vnode vnode, Pager pager, Offset offset, const void* page,
                     int* validated, int* tainted, int* nx, Apply apply) {
    original(vnode,pager,offset,page,validated,tainted,nx);
    if (!vnode || !page || !validated || !tainted || !nx || (*validated & 1)==0
        || *tainted!=0 || *nx!=0 || !candidateOffset(offset)) return;
    apply(vnode,offset,page);
}
}
