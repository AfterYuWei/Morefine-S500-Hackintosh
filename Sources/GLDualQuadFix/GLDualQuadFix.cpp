// Local, exact-page diagnostic repair for the 1002:15e7 APU GL blend path.
// No system files, boot arguments, app commands or signing-policy flags are changed.
#include <Headers/kern_api.hpp>
#include <Headers/kern_util.hpp>
#include <Headers/plugin_start.hpp>
#include <sys/vnode.h>
#include "gl_page_validation.hpp"

namespace {
using ValidatePage = void (*)(vnode_t, memory_object_t, memory_object_offset_t,
                              const void *, int *, int *, int *);
mach_vm_address_t originalValidatePage{0};
UInt32 appliedPages{0};
static const char *off = "-glquadfixoff";
static const char *debug = "-glquadfixdbg";

void applyPage(vnode_t vnode, const void *data)
{
        const auto *page = static_cast<const UInt8 *>(data);
        char path[1024]{};
        int length = sizeof(path);
        if (vn_getpath(vnode, path, &length) != 0 || length <= 0 || length > static_cast<int>(sizeof(path))) {
            return;
        }
        path[sizeof(path)-1] = '\0';
        if (strcmp(path, GLPageData::Path) != 0) { return; }
        if (MachInfo::setKernelWriting(true, KernelPatcher::kernelWriteLock) != KERN_SUCCESS) {
            SYSLOG("GLQuadFix", "write lock unavailable; original page retained");
            return;
        }
        bool wrote = false;
        if (memcmp(page, GLPageData::Page, PAGE_SIZE) == 0) {
            lilu_os_memcpy(const_cast<UInt8 *>(page) + GLPageData::FunctionInPage,
                           GLPageData::Replacement, sizeof(GLPageData::Replacement));
            wrote = true;
        }
        const auto restored = MachInfo::setKernelWriting(false, KernelPatcher::kernelWriteLock);
        if (restored != KERN_SUCCESS) {
            SYSLOG("GLQuadFix", "kernel write protection restoration returned 0x%X", restored);
        }
        if (wrote) {
            const auto count = __atomic_add_fetch(&appliedPages, 1U, __ATOMIC_RELAXED);
            if (count <= 8) { SYSLOG("GLQuadFix", "exact GL page patched; count=%u fileOffset=0x8000", count); }
        }
}

void validatePage(vnode_t vnode, memory_object_t pager, memory_object_offset_t offset,
                  const void *data, int *validated, int *tainted, int *nx)
{
    GLPageValidation::run(reinterpret_cast<ValidatePage>(originalValidatePage),
                          vnode,pager,offset,data,validated,tainted,nx,applyPage);
}

void start()
{
    if (getKernelVersion() != KernelVersion::Tahoe || getKernelMinorVersion() != 6) { return; }
    const auto error = lilu.onPatcherLoad([](void *, KernelPatcher &patcher) {
        KernelPatcher::RouteRequest requests[] = {
            {"_cs_validate_page", validatePage, originalValidatePage},
        };
        if (!patcher.routeMultipleLong(KernelPatcher::KernelID, requests, arrsize(requests))) {
            SYSLOG("GLQuadFix", "page hook unavailable; repair inactive");
            patcher.clearError();
            return;
        }
        SYSLOG("GLQuadFix", "candidate 2 registered at cs_validate_page; exact GL page and PCI guard 15e7");
    });
    if (error != LiluAPI::Error::NoError) {
        SYSLOG("GLQuadFix", "callback registration failed: %u", static_cast<unsigned>(error));
    }
}
}

PluginConfiguration ADDPR(config){
    xStringify(PRODUCT_NAME), parseModuleVersion(xStringify(MODULE_VERSION)),
    LiluAPI::AllowNormal, &off, 1, &debug, 1, nullptr, 0,
    KernelVersion::Tahoe, KernelVersion::Tahoe, []() { start(); },
};
