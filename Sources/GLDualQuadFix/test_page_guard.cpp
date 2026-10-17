#include <cstring>
#include <cassert>
#include <cstdio>
#include "gl_page_guard.hpp"
int main()
{
    unsigned char page[4096];
    memcpy(page,GLPageData::Page,sizeof(page));
    assert(GLPageGuard::matches(page,0x8000));
    assert(!GLPageGuard::matches(nullptr,0x8000));
    for(unsigned i=0;i<4096;i++) {
        page[i]^=1;
        assert(!GLPageGuard::matches(page,0x8000));
        page[i]^=1;
    }
    for(unsigned i=0;i<0x20000;i+=4096) {
        if(i!=0x8000) assert(!GLPageGuard::matches(page,i));
    }
    memcpy(page+GLPageData::FunctionInPage,GLPageData::Replacement,sizeof(GLPageData::Replacement));
    assert(!GLPageGuard::matches(page,0x8000));
    puts("PASS: exact page accepted; all 4096 single-byte mutations, wrong file offsets, null and already-patched pages rejected.");
}
