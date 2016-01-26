#include "version.h"

#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "JoyPanel.h"
#include "QuaJoystick.h"
#include "ControlVariable.h"
#include "messid.h"
#include "XYView.h"
#include "Colors.h"


JoyPanel::JoyPanel(BRect rect, float maxw, char *nm, QuaJoystickPort *A):
	ControlPanel(rect, maxw, "joy",
				CP_TYPE_NOT,
				"Joy",
				nm,
				STATUS_RUNNING)
{
	myJoy = A;
	nAxles = 0;
	short	na = (A->nAxes+1)/2;
	axes = new (XYView*)[na];
	for (short i=0; i<na; i++) {
		AddAxleRect(i);
	}
}

void
JoyPanel::AddAxleRect(short i)
{
	for (short j=i; j<nAxles; j++) {
		axes[j+1] = axes[j];
	} 
	BRect	n(	0,0,
				2*MIXER_MARGIN,2*MIXER_MARGIN);
	GetItemRect(MIXER_MARGIN,
				myHeight-2*MIXER_MARGIN,
				myHeight-2*MIXER_MARGIN,
				MIXER_MARGIN,
				n);
	nAxles++;
	axes[i] = new XYView(n, maxWidth, &myJoy->joy.x, &myJoy->joy.y,
						BPoint(myJoy->minx, myJoy->miny),
						BPoint(myJoy->maxx, myJoy->maxy));
						
	AddChild(axes[i]);
}


void
JoyPanel::MouseDown(BPoint pt)
{
}

void
JoyPanel::MessageReceived(BMessage *msg)
{
	switch(msg->what) {

	case CTRL_BANG:
		break;
		
	case SET_GRANULARITY:
		break;
		
	case SET_OFFSET:
		break;
		
	case PARAM_SET: {
		ControlVariable *p;
		status_t err = msg->FindPointer("control var", (void **)&p);
		if (p != nullptr) {
			p->SetSymbolValue();
		}
		break;
	}
	default:
		Panel::MessageReceived(msg);
	};
}

void
JoyPanel::AttachedToWindow()
{
	ControlPanel::AttachedToWindow();
}



JoyPanel::~JoyPanel()
{
	delete [] axes;
}



void
JoyPanel::Draw(BRect bnd)
{
	BRect	b(1,1,myWidth-1, myHeight-1);
	RaisedBox(this, b, ViewColor(), false);
}

void
JoyPanel::FrameResized(float x, float y)
{
	Panel *v;
	fprintf(stderr,"joy panel resized %g %g\n", y, x);
	myHeight = y;
	myWidth = x;
	if ((v=(Panel*)Parent()) != nullptr) {
		v->ArrangeChildren();
	}
	Invalidate();
}

void
JoyPanel::ArrangeChildren()
{
}
