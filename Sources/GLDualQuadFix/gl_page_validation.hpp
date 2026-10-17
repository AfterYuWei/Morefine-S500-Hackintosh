#pragma once
#include "gl_page_guard.hpp"

// Run the real page validator first, retain all of its output flags, and only
// offer an exact executable page to the patcher after validation succeeds.
namespace GLPageValidation {
template <typename Original, typename Vnode, typename Pager, typename Offset, typename Apply>
inline void run(Original original, Vnode vnode, Pager pager, Offset offset, const void* data,
                int* validated, int* tainted, int* nx, Apply apply)
{
    original(vnode,pager,offset,data,validated,tainted,nx);
    if (!vnode || !data || !validated || !tainted || !nx
        || (*validated & 1)==0 || *tainted!=0 || *nx!=0) { return; }
    if (!GLPageGuard::matches(data,offset)) { return; }
    apply(vnode,data);
}
}
