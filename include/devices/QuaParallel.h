#ifndef _QUA_PARALLEL
#define _QUA_PARALLEL

#include <mutex>

#include "Stream.h"
#include "QuaTypes.h"
#include "QuaPort.h"

class RosterView;
class QuaParallelManager;

class QuaParallelPort: public QuaPort
{
public:
	QuaParallelPort(char *nm, QuaParallelManager *);

	status_t Open();
	status_t Close();
	
	bool CheckPortOpen();
	bool CheckPortClose();
//	ControlPanel		*PortControlPanel(BRect, float);
	
	bool GetStreamItems(Stream *S);
	bool HasStreamItems();
	bool Update();
	
	QuaParallelManager	*quaParallel;
	
	Stream recv;
	std::mutex schlock;
};


class QuaParallelManager: public QuaPortManager<QuaParallelPort>
{
public:
	 QuaParallelManager();
	 ~QuaParallelManager();
					

	virtual QuaPort *findPortByName(const string nm, int direction, int nports) override;

	virtual status_t connect(Input *);
	virtual status_t connect(Output *);
	virtual status_t disconnect(Input *);
	virtual status_t disconnect(Output *);

	static long updateWrapper(void *userData);
	virtual long update();
};

#endif