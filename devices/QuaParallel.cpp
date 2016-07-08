#include "qua_version.h"

#if defined(WIN32)


#define WIN32_LEAN_AND_MEAN
#include <stdafx.h>

#endif
#include "QuaParallel.h"
#include "Stream.h"
//#include "include/Quapp.h"
#include "Qua.h"
#include "QuaInsert.h"
#include "QuaDisplay.h"

QuaParallelPort::QuaParallelPort(char *devnm, QuaParallelManager *qj):
	QuaPort(devnm, QUA_DEV_PARALLEL, QUA_DEV_GENERIC, QUA_PORT_IO)
{
	quaParallel = qj;
	AddInsert("Joy!", 0, INPUT_INSERT, 1, 0);	
}

status_t
QuaParallelPort::Open()
{
	return B_NO_ERROR;
}

status_t
QuaParallelPort::Close()
{
	return B_NO_ERROR;
}

bool
QuaParallelPort::GetStreamItems(Stream *S)
{
	if (recv.nItems > 0) {	

		schlock.lock();
		StreamItem	*p=recv.head,
					**pp = &recv.head,
					*prev = nullptr;
		while (p!=nullptr) {
			bool	add_this = false;
			switch (p->type) {
			case TypedValue::S_JOY: {
				add_this = true;
			    break;
			}
	
			default:	// whatever the fuck this is!
				break;
		    }
		    
		    if (add_this) {
		    	if (S->tail)
		    		S->tail->next = p;
		    	else
		    		S->head = p;
		    	S->tail = p;
		    	S->nItems++;
				recv.nItems--;
		    	*pp = p->next;
			    p = p->next;
		    } else {
		    	prev = p;
		    	pp = &p->next;
		    	p = p->next;
		    }
	    }
	    recv.tail = prev;
	
		schlock.unlock();
		return true;
	}
	return false;
}

bool
QuaParallelPort::Update()
{
	return true;
}

bool
QuaParallelPort::CheckPortOpen()
{
	return true;
}

bool
QuaParallelPort::CheckPortClose()
{
	return Close() == B_NO_ERROR;
}

//ControlPanel *
//QuaJoystickPort::PortControlPanel(BRect r, float maxw)
//{
//	if (!controlPanel) {
//		controlPanel = new JoyPanel(r, maxw, "Joy",	this);
//	}
//	return controlPanel;
//}
//

bool
QuaParallelPort::HasStreamItems()
{
	return recv.nItems > 0;
}


QuaParallelManager::QuaParallelManager()
{
}

QuaParallelManager::~QuaParallelManager()
{
	auto pi = ports.begin();
	while (ports.size() > 0) {
		QuaParallelPort *p =  (QuaParallelPort *)ports[0];
		pi = ports.erase(pi);
	}

	fprintf(stderr, "deleted parallel manager\n");
}


long
QuaParallelManager::updateWrapper( void* userData)
{
	return ((QuaParallelManager*)userData)->update();
}

long
QuaParallelManager::update()
{
	return B_ERROR;
}

status_t
QuaParallelManager::connect(Input *s)
{
	return B_OK;
}

status_t
QuaParallelManager::connect(Output *s)
{
	return B_OK;
}

status_t
QuaParallelManager::disconnect(Input *s)
{
	return B_OK;
}

status_t
QuaParallelManager::disconnect(Output *s)
{
	return B_OK;
}
