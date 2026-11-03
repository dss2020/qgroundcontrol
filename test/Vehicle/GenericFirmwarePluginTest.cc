#include "GenericFirmwarePluginTest.h"

#include <QtCore/QRegularExpression>

#include "GenericFirmwarePlugin.h"
#include "MockLink.h"
#include "Vehicle.h"
#include "VehicleSupports.h"

void GenericFirmwarePluginTest::_connectGenericVehicle()
{
    // Generic MockLink has no component metadata source
    ignoreLogMessage("ComponentInformation.RequestMetaDataTypeStateMachine", QtWarningMsg,
                     QRegularExpression(QStringLiteral("failed to load metadata")));
    _connectMockLink(MAV_AUTOPILOT_GENERIC);
}

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

void GenericFirmwarePluginTest::_flightModeNamesFromStandardModes()
{
    // A private instance: the plugin FirmwarePluginManager hands to vehicles is shared across tests
    GenericFirmwarePlugin plugin;
    QVERIFY(plugin.missionFlightMode().isEmpty());
    QVERIFY(plugin.rtlFlightMode().isEmpty());
    QVERIFY(plugin.landFlightMode().isEmpty());
    QVERIFY(plugin.takeOffFlightMode().isEmpty());
    QVERIFY(plugin.pauseFlightMode().isEmpty());

    // Mode names and custom modes as StandardModes builds them from AVAILABLE_MODES
    const auto mode = [](const QString& name, MAV_STANDARD_MODE standardMode, uint32_t customMode) {
        return FirmwareFlightMode(name, static_cast<uint8_t>(standardMode), customMode, true /* canBeSet */);
    };
    FlightModeList modes{
        mode(QStringLiteral("Manual"), MAV_STANDARD_MODE_NON_STANDARD, 1),
        mode(QStringLiteral("Position"), MAV_STANDARD_MODE_POSITION_HOLD, 2),
        mode(QStringLiteral("Mission"), MAV_STANDARD_MODE_MISSION, 3),
        mode(QStringLiteral("Safe Recovery"), MAV_STANDARD_MODE_SAFE_RECOVERY, 4),
        mode(QStringLiteral("Land"), MAV_STANDARD_MODE_LAND, 5),
        mode(QStringLiteral("Takeoff"), MAV_STANDARD_MODE_TAKEOFF, 6),
    };
    plugin.updateAvailableFlightModes(modes);

    QCOMPARE(plugin.missionFlightMode(), QStringLiteral("Mission"));
    QCOMPARE(plugin.rtlFlightMode(), QStringLiteral("Safe Recovery"));
    QCOMPARE(plugin.landFlightMode(), QStringLiteral("Land"));
    QCOMPARE(plugin.takeOffFlightMode(), QStringLiteral("Takeoff"));
    QCOMPARE(plugin.pauseFlightMode(), QStringLiteral("Position"));
    QCOMPARE(plugin.flightMode(MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, 3), plugin.missionFlightMode());
}

void GenericFirmwarePluginTest::_startMissionSendsMissionStart()
{
    _connectGenericVehicle();
    QVERIFY(vehicle());
    QVERIFY(qobject_cast<GenericFirmwarePlugin*>(vehicle()->firmwarePlugin()));

    mockLink()->setArmed(true);
    QVERIFY_TRUE_WAIT(vehicle()->armed(), TestTimeout::mediumMs());
    mockLink()->clearReceivedMavCommandCounts();

    vehicle()->startMission();

    QVERIFY_TRUE_WAIT(mockLink()->receivedMavCommandCount(MAV_CMD_MISSION_START, vehicle()->defaultComponentId()) == 1,
                      TestTimeout::mediumMs());
    mavlink_message_t message{};
    QVERIFY(mockLink()->lastReceivedMavlinkMessage(MAVLINK_MSG_ID_COMMAND_LONG, message));
    mavlink_command_long_t command{};
    mavlink_msg_command_long_decode(&message, &command);
    QCOMPARE(command.command, static_cast<uint16_t>(MAV_CMD_MISSION_START));
    QCOMPARE(command.param1, 0.0f);
    QCOMPARE(command.param2, 0.0f);
    // MAVLink arming is never attempted
    QCOMPARE(mockLink()->receivedMavCommandCount(MAV_CMD_COMPONENT_ARM_DISARM), 0);
}

void GenericFirmwarePluginTest::_startMissionRequiresArmedVehicle()
{
    _connectGenericVehicle();
    QVERIFY(vehicle());
    QVERIFY(!vehicle()->armed());
    mockLink()->clearReceivedMavCommandCounts();

    expectAppMessage(QRegularExpression(QStringLiteral("arm the vehicle first")));
    vehicle()->startMission();
    verifyExpectedLogMessage();

    // Flush the command queue with a tracked no-op so any MISSION_START or arm request would already be counted
    vehicle()->sendMavCommand(vehicle()->defaultComponentId(), MockLink::MAV_CMD_MOCKLINK_ALWAYS_RESULT_ACCEPTED,
                              false /* showError */);
    QVERIFY_TRUE_WAIT(mockLink()->receivedMavCommandCount(MockLink::MAV_CMD_MOCKLINK_ALWAYS_RESULT_ACCEPTED) == 1,
                      TestTimeout::mediumMs());
    QCOMPARE(mockLink()->receivedMavCommandCount(MAV_CMD_MISSION_START), 0);
    QCOMPARE(mockLink()->receivedMavCommandCount(MAV_CMD_COMPONENT_ARM_DISARM), 0);
}

UT_REGISTER_TEST(GenericFirmwarePluginTest, TestLabel::Integration, TestLabel::Vehicle)
