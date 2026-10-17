#include <cstring>
#include <cassert>
#include <cstdio>
#include <cstdint>
#include "gl_page_validation.hpp"
static int sequence,seenValid,seenTainted,seenNx;
static int originalCalls,applyCalls;
static void* expectedVnode;
static void* expectedPager;
static const void* expectedData;
static uint64_t expectedOffset;
static int *expectedV,*expectedT,*expectedN;
static void original(void* vnode,void* pager,uint64_t offset,const void* data,int* v,int* t,int* n)
{
    assert(sequence++==0); ++originalCalls;
    assert(vnode==expectedVnode && pager==expectedPager && offset==expectedOffset && data==expectedData);
    assert(v==expectedV && t==expectedT && n==expectedN);
    if(v) *v=seenValid;
    if(t) *t=seenTainted;
    if(n) *n=seenNx;
}
static void apply(void* vnode,const void* data)
{
    assert(sequence++==1); ++applyCalls;
    assert(vnode==expectedVnode && data==expectedData);
}
int main()
{
    unsigned char page[4096];memcpy(page,GLPageData::Page,sizeof(page));
    int vnode=1,pager=2;
    unsigned cases=0;
    for(int v=0;v<16;v++)for(int t=0;t<16;t++)for(int n=0;n<16;n++)for(unsigned variation=0;variation<8;variation++) {
        int valid=-1,tainted=-1,nx=-1;
        expectedVnode=variation==1?nullptr:&vnode; expectedPager=&pager;
        expectedData=variation==2?nullptr:page; expectedOffset=variation==3?0x7000:0x8000;
        expectedV=variation==4?nullptr:&valid; expectedT=variation==5?nullptr:&tainted; expectedN=variation==6?nullptr:&nx;
        if(variation==7)page[100]^=1;
        seenValid=v;seenTainted=t;seenNx=n;sequence=0;int before=applyCalls;
        GLPageValidation::run(original,expectedVnode,expectedPager,expectedOffset,expectedData,
                              expectedV,expectedT,expectedN,apply);
        bool allowed=variation==0 && (v&1)!=0 && t==0 && n==0;
        assert(applyCalls-before==allowed && sequence==1+allowed);
        if(expectedV)assert(valid==v);if(expectedT)assert(tainted==t);if(expectedN)assert(nx==n);
        if(variation==7)page[100]^=1;
        cases++;
    }
    assert(originalCalls==(int)cases);
    printf("PASS: %u actual page-bridge cases; original called first with all seven arguments, validation/taint/NX outputs preserved; only exact validated executable page accepted.\n",cases);
}
