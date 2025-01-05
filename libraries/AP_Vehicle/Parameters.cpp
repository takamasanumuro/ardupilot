#include "AP_Vehicle.h"

#if AP_VEHICLE_ENABLED

#include <AP_Param/AP_Param.h>
#include <StorageManager/StorageManager.h>

void AP_Vehicle::load_parameters(AP_Int16 &format_version, const uint16_t expected_format_version)
{
    if (!format_version.load() || //!If parameter enum table is altered, clear EEPROM
        format_version != expected_format_version) {

        // erase all parameters
        hal.console->printf("Firmware change: erasing EEPROM...\n");
        StorageManager::erase(); //!Fills EEPROM with 0
        AP_Param::erase_all(); //!Writes header and sentinel on Parameter area after StorageManager::erase()

        // save the current format version
        format_version.set_and_save(expected_format_version);
        hal.console->printf("done.\n");
    }
    format_version.set_default(expected_format_version);

    // Load all auto-loaded EEPROM variables
    AP_Param::load_all();
}

#endif  // AP_VEHICLE_ENABLED
