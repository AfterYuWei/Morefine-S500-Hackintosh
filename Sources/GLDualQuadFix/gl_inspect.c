#include <dlfcn.h>
#include <mach-o/dyld.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "gl_blend_bytes.h"
int main(void)
{
    const char *path="/System/Library/Extensions/AMDRadeonX5000GLDriver.bundle/Contents/MacOS/AMDRadeonX5000GLDriver";
    void *handle=dlopen(path,RTLD_LAZY|RTLD_LOCAL);
    if(!handle) { fprintf(stderr,"GL image load failed: %s\n",dlerror()); return 3; }
    for(uint32_t i=0;i<_dyld_image_count();i++) {
        const char *name=_dyld_get_image_name(i);
        if(!name || strcmp(name,path)!=0) continue;
        const unsigned char *p=(const unsigned char *)_dyld_get_image_header(i)+0x819e;
        if(memcmp(p,Replacement,sizeof(Replacement))==0) {
            puts("PATCHED: expected GL blend payload bytes observed in the loaded Apple GL driver.");
            puts("This verifies loaded code only; app rendering and GPU stability still require real operation.");
            return 0;
        }
        if(memcmp(p,Original,sizeof(Original))==0) {
            puts("NATIVE: original GL function observed; GLDualQuadFix is not applied in this process.");
            return 10;
        }
        puts("UNKNOWN: loaded GL function matches neither original nor candidate; do not run the GPU test.");
        return 11;
    }
    puts("GL image not found in dyld image list.");
    return 12;
}
