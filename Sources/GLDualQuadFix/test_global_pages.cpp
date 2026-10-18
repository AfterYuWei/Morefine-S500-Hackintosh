#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "global_page_guard.hpp"
static unsigned originalCalls,offered,applied;
static int seenV,seenT,seenN;
static void* expectedVnode;
static void* expectedPager;
static const void* expectedData;
static uint64_t expectedOffset;
static int *expectedV,*expectedT,*expectedN;
static const char* path;
static bool metalEnabled=true;
static void original(void* vnode,void* pager,uint64_t offset,const void* data,int* v,int* t,int* n) {
    ++originalCalls;
    assert(vnode==expectedVnode && pager==expectedPager && offset==expectedOffset && data==expectedData);
    assert(v==expectedV && t==expectedT && n==expectedN);
    if(v)*v=seenV;if(t)*t=seenT;if(n)*n=seenN;
}
static void apply(void* vnode,uint64_t offset,const void* data) {
    assert(vnode==expectedVnode && offset==expectedOffset && data==expectedData);
    ++offered;
    if(GlobalPageGuard::find(path,offset,data,metalEnabled)) ++applied;
}
int main() {
    unsigned mutations=0,bridgeCases=0;
    unsigned char page[4096];int vnode=1,pager=2;
    for(const auto& e:GlobalPageData::Entries) {
        memcpy(page,e.original,4096);
        for(unsigned p=0;p<e.pathCount;++p) assert(GlobalPageGuard::find(e.paths[p],e.fileOffset,page,true)==&e);
        assert(!GlobalPageGuard::find(nullptr,e.fileOffset,page,true));
        assert(!GlobalPageGuard::find("/unrelated/driver",e.fileOffset,page,true));
        assert(!GlobalPageGuard::find(e.paths[0],e.fileOffset+4096,page,true));
        assert(!GlobalPageGuard::find(e.paths[0],e.fileOffset,nullptr,true));
        assert((GlobalPageGuard::find(e.paths[0],e.fileOffset,page,false)!=nullptr)==!e.metal);
        for(unsigned i=0;i<4096;++i) {
            page[i]^=1;assert(!GlobalPageGuard::find(e.paths[0],e.fileOffset,page,true));page[i]^=1;++mutations;
        }
        memcpy(page,e.patched,4096);assert(!GlobalPageGuard::find(e.paths[0],e.fileOffset,page,true));
        memcpy(page,e.original,4096);
        for(int v=0;v<16;++v)for(int t=0;t<16;++t)for(int n=0;n<16;++n)for(unsigned variation=0;variation<10;++variation) {
            int valid=-1,tainted=-1,nx=-1;
            expectedVnode=variation==1?nullptr:&vnode;expectedPager=&pager;
            expectedData=variation==2?nullptr:page;expectedOffset=variation==3?0:e.fileOffset;
            expectedV=variation==4?nullptr:&valid;expectedT=variation==5?nullptr:&tainted;expectedN=variation==6?nullptr:&nx;
            path=variation==8?"/unrelated/driver":e.paths[0];metalEnabled=variation!=9;
            if(variation==7)page[100]^=1;
            seenV=v;seenT=t;seenN=n;
            const auto before=applied;const auto originals=originalCalls;
            GlobalPageGuard::validate(original,expectedVnode,expectedPager,expectedOffset,expectedData,expectedV,expectedT,expectedN,apply);
            const bool allowed=(variation==0 || (variation==9 && !e.metal)) && (v&1) && t==0 && n==0;
            assert(applied-before==static_cast<unsigned>(allowed));assert(originalCalls==originals+1);
            if(expectedV)assert(valid==v);if(expectedT)assert(tainted==t);if(expectedN)assert(nx==n);
            if(variation==7)page[100]^=1;++bridgeCases;
        }
    }
    printf("PASS: %u full-page single-byte rejection cases; %u real validation-bridge cases, seven arguments and native outputs preserved; GL/Metal, wrong path/offset, old-mode and already-patched rejection.\n",mutations,bridgeCases);
}
