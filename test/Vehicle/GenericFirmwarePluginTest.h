#pragma once

#include "BaseClasses/VehicleTestManualConnect.h"

class GenericFirmwarePluginTest : public VehicleTestManualConnect
{
    Q_OBJECT

private slots:
    void _takeoffWithoutAltitudeSupported();
    void _flightModeNamesFromStandardModes();
    void _startMissionSendsMissionStart();
    void _startMissionRequiresArmedVehicle();

private:
    void _connectGenericVehicle();
};
