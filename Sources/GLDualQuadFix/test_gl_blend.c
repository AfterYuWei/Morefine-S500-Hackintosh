#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "gl_blend_bytes.h"

static unsigned char context[0x4000], saved[sizeof(context)], hw[0x200];
static uint32_t command[16], afterFlush[16];
static unsigned flushes;
static void put32(void *p, size_t off, uint32_t v) { memcpy((char *)p+off,&v,4); }
static uint32_t get32(void *p, size_t off) { uint32_t v; memcpy(&v,(char *)p+off,4); return v; }
static void putptr(void *p, size_t off, void *v) { memcpy((char *)p+off,&v,sizeof(v)); }
static void *getptr(void *p, size_t off) { void *v; memcpy(&v,(char *)p+off,sizeof(v)); return v; }
__attribute__((noinline)) static void submit(void *p, unsigned token)
{
    assert(p==context && token==2);
    assert(((uintptr_t)__builtin_frame_address(0)&15)==0);
    ++flushes;
    putptr(p,0xd68,afterFlush);
}
typedef void (*Code)(void *);
static Code load(const unsigned char *bytes, unsigned callOffset)
{
    unsigned char *p=mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE,-1,0);
    assert(p!=MAP_FAILED);
    memcpy(p,bytes,sizeof(Original));
    int32_t rel=(int32_t)(0x800-callOffset-5);
    memcpy(p+callOffset+1,&rel,4);
    p[0x800]=0x48; p[0x801]=0xb8;
    uintptr_t target=(uintptr_t)&submit;
    memcpy(p+0x802,&target,8); p[0x80a]=0xff; p[0x80b]=0xe0;
    assert(mprotect(p,4096,PROT_READ|PROT_EXEC)==0);
    return (Code)(uintptr_t)p;
}
static void setup(unsigned id,unsigned dual,unsigned capability,unsigned capacity,unsigned initial)
{
    memset(context,0xa5,sizeof(context));
    memset(hw,0,sizeof(hw));
    memset(command,0xcd,sizeof(command));
    memset(afterFlush,0xcd,sizeof(afterFlush));
    flushes=0;
    putptr(context,0x12d8,hw);
    put32(hw,12,id); put32(hw,0xa0,capability?2:0);
    put32(context,0x30ec,dual?4:0);
    put32(context,0x1398,initial);
    putptr(context,0xd68,command);
    putptr(context,0xd60,(unsigned char *)command+capacity);
}
static void check(unsigned capacity,unsigned expected)
{
    uint32_t *p=capacity<=12?afterFlush:command;
    assert(flushes==(capacity<=12));
    assert(p[0]==0xc0016900 && p[1]==0x202 && p[2]==expected && p[3]==0xcdcdcdcd);
    assert(get32(context,0x1398)==expected);
    assert(getptr(context,0xd68)==p+3 && getptr(context,0x30f8)==p+3);
}
int main(void)
{
    Code original=load(Original,OriginalCallOffset);
    Code replacement=load(Replacement,ReplacementCallOffset);
    unsigned long long cases=0;
    for(unsigned id=0;id<65536;id++) for(unsigned dual=0;dual<2;dual++)
    for(unsigned cap=0;cap<2;cap++) for(unsigned k=0;k<3;k++) {
        unsigned capacity=(unsigned[]){8,12,64}[k];
        unsigned initial=0x7fedcba0u^((id*0x9e37u)<<1)^dual;
        unsigned native=(initial&~1u)|(dual&&cap);
        setup(id,dual,cap,capacity,initial);
        original(context); check(capacity,native);
        memcpy(saved,context,sizeof(context));
        unsigned expected=(initial&~1u)|((id==0x15e7)||(dual&&cap));
        setup(id,dual,cap,capacity,initial);
        replacement(context); check(capacity,expected);
        put32(saved,0x1398,expected);
        assert(memcmp(context,saved,sizeof(context))==0);
        cases++;
    }
    printf("PASS: %llu native machine-code comparisons; all 65536 PCI IDs, dual-source/capability combinations, normal/equal/short buffers; APU bit corrected; other IDs and opaque context preserved. No GPU commands submitted.\n",cases);
    return 0;
}
