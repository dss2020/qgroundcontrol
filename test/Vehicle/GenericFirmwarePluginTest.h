#pragma once

#include "UnitTest.h"

class GenericFirmwarePluginTest : public UnitTest
{
    Q_OBJECT

private slots:
    void _takeoffWithoutAltitudeSupported();
};
