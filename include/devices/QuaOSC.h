#pragma once

#include "qua_version.h"

#include "QuaPort.h"

class QuaOSCPort : public QuaPort {

};

class QuaOSCManager : public QuaPortManager<QuaOSCPort>
{

};