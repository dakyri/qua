#ifdef __INTEL__
#include <SupportKit.h>
#include <MediaKit.h>
#endif

#include "version.h"

#include "Quapp.h"
#include "QuaAudio.h"
#include "QuaPort.h"
#include "Toolbox.h"
#include "RosterView.h"
#include "StdDefs.h"
#include "Instance.h"
#include "Qua.h"
#include "ValList.h"
#include "Channel.h"
#include "Block.h"
#include "ArrangerObject.h"
#include "messid.h"
#include "Envelope.h"

#ifdef NEW_MEDIA

ControllableNode::ControllableNode(QuaAudioPort *qap):
	Schedulable(qap->sym, nullptr, nullptr)
{
	audioPort = qap;

	status_t		err;
	BMediaRoster	*r = BMediaRoster::Roster();
}

#endif

