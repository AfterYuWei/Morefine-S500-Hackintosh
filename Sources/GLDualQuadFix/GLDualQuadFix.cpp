// Global GL + Metal repair: original signed-page validation, then exact-page edits.
// No application-name filter, cs_allow_invalid, private process-page writes or disk edits.
#include <Headers/kern_api.hpp>
#include <Headers/kern_util.hpp>
#include <Headers/kern_devinfo.hpp>
#include <Headers/plugin_start.hpp>
#include <IOKit/pci/IOPCIDevice.h>
#include <sys/vnode.h>
#include "global_page_guard.hpp"

namespace {
using ValidatePage = void (*)(vnode_t,memory_object_t,memory_object_offset_t,const void*,int*,int*,int*);
mach_vm_address_t originalValidatePage{0};
UInt32 appliedPages[GlobalPageData::Count]{};
bool metalEnabled{false};
static const char* off="-glquadfixoff";
static const char* debug="-glquadfixdbg";

void applyPage(vnode_t vnode, memory_object_offset_t offset, const void* data) {
    char path[1024]{};
    int length=sizeof(path);
    if (vn_getpath(vnode,path,&length)!=0 || length<=0 || length>static_cast<int>(sizeof(path))) return;
    path[sizeof(path)-1]='\0';
    const auto* entry=GlobalPageGuard::find(path,offset,data,metalEnabled);
    if (!entry) return;
    if (MachInfo::setKernelWriting(true,KernelPatcher::kernelWriteLock)!=KERN_SUCCESS) {
        SYSLOG("GLQuadFix","global page write lock unavailable; native page retained"); return;
    }
    bool wrote=false;
    if (memcmp(data,entry->original,GlobalPageData::PageSize)==0) {
        lilu_os_memcpy(const_cast<void*>(data),entry->patched,GlobalPageData::PageSize);
        wrote=true;
    }
    const auto restored=MachInfo::setKernelWriting(false,KernelPatcher::kernelWriteLock);
    if (restored!=KERN_SUCCESS) SYSLOG("GLQuadFix","write protection restore returned 0x%X",restored);
    if (wrote) {
        const auto index=static_cast<unsigned>(entry-GlobalPageData::Entries);
        const auto count=__atomic_add_fetch(&appliedPages[index],1U,__ATOMIC_RELAXED);
        if (count<=8) SYSLOG("GLQuadFix","Global-v1 page=%u kind=%s count=%u offset=0x%llX",index,entry->metal?"Metal":"GL",count,static_cast<unsigned long long>(offset));
    }
}
void validatePage(vnode_t vnode,memory_object_t pager,memory_object_offset_t offset,const void* data,
                  int* validated,int* tainted,int* nx) {
    GlobalPageGuard::validate(reinterpret_cast<ValidatePage>(originalValidatePage),vnode,pager,offset,
                              data,validated,tainted,nx,applyPage);
}
void start() {
    if (getKernelVersion()!=KernelVersion::Tahoe || getKernelMinorVersion()!=6) return;
    const auto error=lilu.onPatcherLoad([](void*,KernelPatcher& patcher) {
        const auto info=DeviceInfo::create();
        if (!info) return;
        const auto gpu=OSDynamicCast(IOPCIDevice,info->videoBuiltin);
        const bool target=gpu && info->videoExternal.size()==0
            && WIOKit::readPCIConfigValue(gpu,WIOKit::kIOPCIConfigVendorID)==0x1002
            && WIOKit::readPCIConfigValue(gpu,WIOKit::kIOPCIConfigDeviceID)==0x15e7;
        DeviceInfo::deleter(info);
        if (!target) { SYSLOG("GLQuadFix","Global-v1 refused: requires single 1002:15e7 GPU"); return; }
        metalEnabled=!(checkKernelArgument("-NRedLegacyBlend") || checkKernelArgument("-NRedImmediateSync")
            || checkKernelArgument("-NRedSrdShared") || checkKernelArgument("-NRedComputeScratch")
            || checkKernelArgument("-NRedTransfer1x1"));
        if (!metalEnabled) SYSLOG("GLQuadFix","Global-v1 Metal refused: old per-process patch arguments still present");
        KernelPatcher::RouteRequest requests[]={{"_cs_validate_page",validatePage,originalValidatePage}};
        if (!patcher.routeMultipleLong(KernelPatcher::KernelID,requests,arrsize(requests))) {
            patcher.clearError(); SYSLOG("GLQuadFix","Global-v1 page hook unavailable"); return;
        }
        SYSLOG("GLQuadFix","Global-v1 registered; Metal=%u, no application filter; native validation outputs preserved",metalEnabled?1U:0U);
    });
    if (error!=LiluAPI::Error::NoError) SYSLOG("GLQuadFix","Global-v1 registration error %u",static_cast<unsigned>(error));
}
}
PluginConfiguration ADDPR(config){
    xStringify(PRODUCT_NAME),parseModuleVersion(xStringify(MODULE_VERSION)),
    LiluAPI::AllowNormal,&off,1,&debug,1,nullptr,0,
    KernelVersion::Tahoe,KernelVersion::Tahoe,[](){start();},
};
