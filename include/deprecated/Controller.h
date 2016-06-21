#ifndef _GENERIC_CONTROLLER
#define _GENERIC_CONTROLLER

class Qua;
class ControlPanel;
class QuasiStack;
class ControlVariable;
class StabEnt;
class GroupPanel;

#include "Panel.h"

enum {
	CP_TYPE_NOT = 0,
	CP_TYPE_NORM = 1,
	CP_TYPE_TOGGLE = 2,
	CP_TYPE_HELD = 3,
	CP_TYPE_ONOFF = 4
};

class ControlPanel: public Panel
{
public:
					ControlPanel(BRect rect,
								float maxw,
								char *panelname,
								short ct_type,
								char *lblact,
								char *nm,
								short status);
					~ControlPanel();
	virtual void	AttachedToWindow();
	virtual void	MouseDown(BPoint where);
	virtual void	ArrangeChildren();
	
	void			AddControlVariables(
								ControlVariable *Par,
								Qua *q,
								QuasiStack *stack);
	void			AddHigherFrame(BList &higherFrame,
								Qua	*qua);
	void			RemoveHigherFrames();
	void			RemoveControlVariables();
	void			DisplayStatus(short st);
	
	GroupPanel		*GroupPanelFor(StabEnt *s);
	void			AddGroupPanel(StabEnt*s);
	void			RemoveGroupPanels();

	short			bBangType;
	BControl		*bBangButton;
	BStringView		*vName;
	ControlVariable	*parameters;
	
	BList			groupPanels;
};

#endif

