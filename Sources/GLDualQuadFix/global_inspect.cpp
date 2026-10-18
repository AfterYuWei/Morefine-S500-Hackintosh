// Read our own shared-cache mapping. No GPU submission or process-signing changes.
#include <dlfcn.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <mach-o/dyld_images.h>
#include <mach-o/dyld.h>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "global_page_data.hpp"
int main() {
    task_dyld_info_data_t ti{};mach_msg_type_number_t count=TASK_DYLD_INFO_COUNT;
    if(task_info(mach_task_self(),TASK_DYLD_INFO,reinterpret_cast<task_info_t>(&ti),&count)!=KERN_SUCCESS) return 3;
    const auto* info=reinterpret_cast<const dyld_all_image_infos*>(ti.all_image_info_addr);
    if(!info || info->version<15 || !info->sharedCacheBaseAddress) return 4;
    const unsigned char expectedUUID[]={0x31,0x37,0x47,0x56,0x89,0xb4,0x34,0xe8,0xa3,0x4f,0x56,0xc5,0xd5,0x72,0x16,0xa1};
    if(memcmp(info->sharedCacheUUID,expectedUUID,16)!=0) {puts("UNKNOWN: shared cache UUID differs; no GPU test.");return 11;}
    bool all=true,unknown=false;
    for(unsigned i=0;i<GlobalPageData::Count;++i) {
        const auto& e=GlobalPageData::Entries[i];
        if(!e.metal) continue;
        const auto address=info->sharedCacheBaseAddress+e.sharedOffset;
        unsigned char bytes[4096];mach_vm_size_t read=0;
        if(mach_vm_read_overwrite(mach_task_self(),address,sizeof(bytes),reinterpret_cast<mach_vm_address_t>(bytes),&read)!=KERN_SUCCESS || read!=sizeof(bytes)) return 5;
        const bool patched=memcmp(bytes,e.patched,4096)==0,native=memcmp(bytes,e.original,4096)==0;
        printf("Metal page %u: %s\n",i,patched?"PATCHED":native?"NATIVE":"UNKNOWN");
        all&=patched;unknown|=!patched&&!native;
    }
    if(unknown) {puts("UNKNOWN: page differs from both exact states; stop GPU reproduction.");return 11;}
    if(all) {puts("PATCHED: both Metal pages match the global candidate. App rendering and stability are not yet verified.");return 0;}
    puts("NATIVE_OR_MIXED: global Metal candidate is not fully active; reboot and inspect before reproducing.");return 10;
}
