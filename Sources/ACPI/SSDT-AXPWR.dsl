// AX200 trial for this machine's GPP4 bridge only.
// Requires exact DSDT _PS0 -> XPS0 and _PS3 -> XPS3 renames.
// macOS: expose no ACPI power-switching methods for this bridge.
// Other operating systems: call the unchanged original firmware methods.
DefinitionBlock ("", "SSDT", 2, "LOCAL", "AXPWRISO", 0x00000001)
{
    External (\_SB_.PCI0.GPP4, DeviceObj)
    External (\_SB_.PCI0.GPP4.XPS0, MethodObj)
    External (\_SB_.PCI0.GPP4.XPS3, MethodObj)

    Scope (\_SB.PCI0.GPP4)
    {
        If (LNot (_OSI ("Darwin")))
        {
            Method (_PS0, 0, Serialized)
            {
                XPS0 ()
            }

            Method (_PS3, 0, Serialized)
            {
                XPS3 ()
            }
        }
    }
}
