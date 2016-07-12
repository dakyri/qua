#pragma once

#include "qua_version.h"

#include "QuaPort.h"

class QuaOSCPort : public QuaPort {

};

class QuaOSCManager : public QuaPortManager<QuaOSCPort>
{
public:
	QuaOSCManager();
	virtual ~QuaOSCManager();

	virtual QuaPort *findPortByName(string nm) override;
	virtual status_t connect(Input *) override;
	virtual status_t connect(Output *) override;
	virtual status_t disconnect(Input *) override;
	virtual status_t disconnect(Output *) override;
};