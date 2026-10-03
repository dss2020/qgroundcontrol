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
};
