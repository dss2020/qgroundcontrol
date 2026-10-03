#include "GenericFirmwarePluginTest.h"

#include "GenericFirmwarePlugin.h"
#include "Vehicle.h"
#include "VehicleSupports.h"

void GenericFirmwarePluginTest::_takeoffWithoutAltitudeSupported()
{
    Vehicle vehicle(MAV_AUTOPILOT_GENERIC, MAV_TYPE_GENERIC);
    QVERIFY(qobject_cast<GenericFirmwarePlugin*>(vehicle.firmwarePlugin()));

    VehicleSupports* const supports = vehicle.supports();
    QVERIFY(supports);
    QVERIFY(supports->guidedTakeoffWithoutAltitude());
    QVERIFY(supports->takeoffMissionCommand());
    QVERIFY(!supports->guidedTakeoffWithAltitude());
    QVERIFY(!supports->guidedMode());
}

UT_REGISTER_TEST(GenericFirmwarePluginTest, TestLabel::Unit, TestLabel::Vehicle)
