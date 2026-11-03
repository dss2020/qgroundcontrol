#pragma once

#include "FirmwarePlugin.h"

/// Fallback plugin for autopilots without a dedicated FirmwarePlugin (e.g. MAV_AUTOPILOT_GENERIC)
class GenericFirmwarePlugin : public FirmwarePlugin
{
    Q_OBJECT

public:
    explicit GenericFirmwarePlugin(QObject* parent = nullptr);
    ~GenericFirmwarePlugin() override;

    bool isCapable(const Vehicle* vehicle, FirmwareCapabilities capabilities) const override;
    void startTakeoff(Vehicle* vehicle) const override;
    void startMission(Vehicle* vehicle) const override;

    QString pauseFlightMode() const override;
    QString missionFlightMode() const override;
    QString rtlFlightMode() const override;
    QString landFlightMode() const override;
    QString takeOffFlightMode() const override;

private:
    /// Name of the AVAILABLE_MODES entry tagged with @a standardMode, empty if the vehicle reported none
    QString _standardModeName(MAV_STANDARD_MODE standardMode) const;
};
