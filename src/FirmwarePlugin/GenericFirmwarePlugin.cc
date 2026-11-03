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

void GenericFirmwarePlugin::startMission(Vehicle* vehicle) const
{
    if (!vehicle) {
        return;
    }

    // Arming is not attempted: generic autopilots may not accept MAVLink arming (the pilot arms by RC)
    if (!vehicle->armed()) {
        QGC::showAppMessage(tr("Unable to start mission: arm the vehicle first."));
        return;
    }

    vehicle->sendMavCommand(vehicle->defaultComponentId(), MAV_CMD_MISSION_START,
                            true,  // show error if fails
                            0,     // first item
                            0);    // last item: 0 runs to the end of the mission
}

QString GenericFirmwarePlugin::pauseFlightMode() const
{
    return _standardModeName(MAV_STANDARD_MODE_POSITION_HOLD);
}

QString GenericFirmwarePlugin::missionFlightMode() const
{
    return _standardModeName(MAV_STANDARD_MODE_MISSION);
}

QString GenericFirmwarePlugin::rtlFlightMode() const
{
    return _standardModeName(MAV_STANDARD_MODE_SAFE_RECOVERY);
}

QString GenericFirmwarePlugin::landFlightMode() const
{
    return _standardModeName(MAV_STANDARD_MODE_LAND);
}

QString GenericFirmwarePlugin::takeOffFlightMode() const
{
    return _standardModeName(MAV_STANDARD_MODE_TAKEOFF);
}

QString GenericFirmwarePlugin::_standardModeName(MAV_STANDARD_MODE standardMode) const
{
    for (const FirmwareFlightMode& flightMode : _flightModeList) {
        if (flightMode.standard_mode == standardMode) {
            return flightMode.mode_name;
        }
    }
    return QString();
}
