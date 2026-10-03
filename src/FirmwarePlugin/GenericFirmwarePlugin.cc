#include "GenericFirmwarePlugin.h"

#include <cmath>

#include "AppMessages.h"
#include "MAVLinkLib.h"
#include "Vehicle.h"

GenericFirmwarePlugin::GenericFirmwarePlugin(QObject* parent) : FirmwarePlugin(parent) {}

GenericFirmwarePlugin::~GenericFirmwarePlugin() = default;

bool GenericFirmwarePlugin::isCapable(const Vehicle* vehicle, FirmwareCapabilities capabilities) const
{
    Q_UNUSED(vehicle);

    // Takeoff without an altitude only: guided mode stays off so unsupported guided actions are not offered
    constexpr int available = TakeoffVehicleCapability;
    return (capabilities & available) == capabilities;
}

void GenericFirmwarePlugin::startTakeoff(Vehicle* vehicle) const
{
    if (!vehicle) {
        return;
    }

    if (vehicle->flying()) {
        QGC::showAppMessage(tr("Unable to start takeoff: Vehicle is already in the air."));
        return;
    }

    if (!vehicle->armed() && !_armVehicleAndValidate(vehicle)) {
        QGC::showAppMessage(tr("Unable to start takeoff: Vehicle failed to arm."));
        return;
    }

    // NaN altitude lets the autopilot use its own default takeoff altitude
    vehicle->sendMavCommand(vehicle->defaultComponentId(), MAV_CMD_NAV_TAKEOFF,
                            true,  // show error if fails
                            NAN, NAN, NAN, NAN, NAN, NAN, NAN);
}
