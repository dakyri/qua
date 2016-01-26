
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "Controller.h"
#include "Qua.h"
#include "Colors.h"
#include "messid.h"
#include "Sym.h"
#include "KeyVal.h"
#include "NumCtrl.h"
#include "NameCtrl.h"
#include "TimeCtrl.h"
#include "BlockCtrl.h"
#include "PoolSelect.h"
#include "StdDefs.h"
#include "ToggleButton.h"
#include "HeldButton.h"
#include "RWLock.h"
#include "QuasiStack.h"
#include "ControlVariable.h"
#include "InstancePanel.h"
#include "SchedulablePanel.h"
#include "MyCheckBox.h"
#include "Envelope.h"
#include "EnvelopePanel.h"


ControlPanel::ControlPanel(
			BRect 		rect,
			float		maxw,
			char		*panam,
			short		ct_type,
			char		*lblact,
			char		*nm,
			short		st)
	: Panel(rect, maxw, panam)
{
//	fprintf(stderr, "control panel(%d <%s> <%s> nm <%s> %x)\n", ct_type, panam, lblact, nm, nm);
	BRect wrect;
	bBangType = ct_type;
	
	SetFontSize(12);
	SetFont(be_bold_font);
	
	BMessage	*m1 = new BMessage(CTRL_BANG),
				*m2 = new BMessage(CTRL_BANG);
	m1->AddInt32("state", STATUS_RUNNING);
	m2->AddInt32("state", STATUS_SLEEPING);
	
	parameters = nullptr;

	bBangButton = nullptr;
	vName = nullptr;
	float	childHeight = 0;
	if (ct_type == CP_TYPE_TOGGLE) {
		short swid = StringWidth(*nm?nm:lblact);
		wrect.Set(thePoint.x,thePoint.y,thePoint.x + swid+15,thePoint.y+20);
		if (st == STATUS_RUNNING) {
			bBangButton = new ToggleButton( wrect, "",
				"Stop",
				 *nm? nm:lblact,
				 new BMessage(*m2),
				 new BMessage(*m1),
				 STATUS_RUNNING,
				 STATUS_SLEEPING,
				 (ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW );
		} else {
			bBangButton = new ToggleButton( wrect, "",
				*nm?nm:lblact,
				"Stop",
				 new BMessage(*m1),
				 new BMessage(*m2),
				 STATUS_SLEEPING,
				 STATUS_RUNNING,
				 (ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW );
		}
		vName = nullptr;
		bBangButton->SetFont(be_bold_font);
		bBangButton->SetFontSize( 12 );
		AddChild(bBangButton);
		thePoint.x += swid+20;
		childHeight = bBangButton->Bounds().bottom;
	} else if (ct_type == CP_TYPE_HELD) {
		short swid = StringWidth(*nm?nm:lblact);
		wrect.Set(thePoint.x,thePoint.y,thePoint.x + swid+15,thePoint.y+20);
		bBangButton = new HeldButton( wrect, "", *nm?nm:lblact,
				 new BMessage(*m1),
				 new BMessage(*m2),
				 (ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW );
		bBangButton->SetFont(be_bold_font);
		bBangButton->SetFontSize( 12 );
		AddChild(bBangButton);
		childHeight = bBangButton->Bounds().bottom;
		thePoint.x += swid+20;
		vName = nullptr;
	} else if (ct_type == CP_TYPE_NOT) {
		if (*nm) {
			short swid = StringWidth(nm);
			wrect.Set(thePoint.x, thePoint.y,thePoint.x+swid+5,thePoint.y+20);
			vName = new BStringView( wrect, "name", nm, B_FOLLOW_LEFT|B_FOLLOW_TOP);
			vName->SetViewColor(mdGray);
			vName->SetFont(be_bold_font);
			vName->SetFontSize( 12 );
			AddChild(vName);
			childHeight = vName->Bounds().bottom;
			thePoint.x = vName->Frame().right + 5;
		} else {
			vName = nullptr;
		}
	} else if (ct_type == CP_TYPE_ONOFF) {
		short swid = StringWidth(nm);
		wrect.Set(thePoint.x, thePoint.y,thePoint.x+swid+15,thePoint.y+20);
		
		BMessage	*msg = new BMessage(CTRL_BANG);
		
		bBangButton = new MyCheckBox(wrect, "", nm,
							msg, (ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW);
		bBangButton->SetFont(be_bold_font);
		bBangButton->SetFontSize(12);
		bBangButton->SetViewColor(mdGray);
		bBangButton->SetValue(st == STATUS_RUNNING);
		childHeight = bBangButton->Bounds().bottom;
		AddChild(bBangButton);
		thePoint.x += swid+25;
		vName = nullptr;
//		short swid = StringWidth(lblact);
	} else if (ct_type == CP_TYPE_NORM) {
		if (*nm) {
			short swid = StringWidth(nm);
			wrect.Set(thePoint.x, thePoint.y,thePoint.x+swid,thePoint.y+20);
			vName = new BStringView( wrect, "", nm, B_FOLLOW_LEFT|B_FOLLOW_TOP);
			vName->SetViewColor(mdGray);
			vName->SetFont(be_bold_font);
			vName->SetFontSize( 12 );
			AddChild(vName);
			childHeight = vName->Bounds().bottom;
			thePoint.x += swid+20;
		} else {
			vName = nullptr;
		}
		
		short swid = StringWidth(lblact);
		wrect.Set(thePoint.x,thePoint.y,thePoint.x + swid+15,thePoint.y+20);
		bBangButton = new BButton( wrect, "", lblact,
				 new BMessage(*m1), (ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW );
		bBangButton->SetFont(be_bold_font);
		bBangButton->SetFontSize( 12 );
		AddChild(bBangButton);
		thePoint.x += swid+20;
		childHeight = Max(childHeight, bBangButton->Bounds().bottom);
		
	//		if (addStop) {
	//			wrect.Set(thePoint.x,thePoint.y+2,thePoint.x+15+StringWidth("Stop"),thePoint.y+22);
	//			bStop = new BButton( wrect, "", "Stop",
	//					 new BMessage(m2), nullptr, B_WILL_DRAW );
	//			bStop->SetFont(be_bold_font);
	//			bStop->SetFontSize( 12 );
	//			AddChild(bStop);
	//			thePoint.x += 20+StringWidth("Stop");
	//		} else
	//			bStop = nullptr;
	}
	
	myHeight = 2*MIXER_MARGIN + childHeight;
	if (myWidth < maxWidth)
		myWidth = thePoint.x + MIXER_MARGIN;
	ResizeTo(myWidth, myHeight);
//	fprintf(stderr, "control panel: hite %g\n", hite);
	
	delete m1;
	delete m2;
}

void
ControlPanel::AddControlVariables(
	ControlVariable	*par,
	Qua		*qua,
	QuasiStack	*stack)
{
	parameters = par;
	BRect	wrect;
	
	ControlPanel	*destPanel=this;
	
	for (ControlVariable *p=par; p!=nullptr; p=p->next) {
		destPanel = GroupPanelFor(p->stabEnt);
		if (destPanel == nullptr)
			destPanel = this;
		if (p->stabEnt->controlInfo->displayMode == DISP_MODE_CTL) {
			switch (p->stabEnt->type ) {
			case S_POOL: {
				short swid = StringWidth(p->stabEnt->name);
				destPanel->GetItemRect(MIXER_MARGIN, swid + 65, 17, 5, wrect);
				
				BMessage	*msg = new BMessage(PARAM_SET);
				
				PoolSelect		*n = new PoolSelect(wrect, "control", p->stabEnt->name,
									nullptr, nullptr, qua,
									(ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW);
				SetControlDefaults(n, swid, p, msg);
				n->SetDivider(swid+5);
				destPanel->AddChild(n);
				break;
			}

			case S_BOOL: {
				short swid = StringWidth(p->stabEnt->name);
				destPanel->GetItemRect(MIXER_MARGIN, swid + 20, 17, 5, wrect);
				
				BMessage	*msg = new BMessage(PARAM_SET);
				
				MyCheckBox		*n
						= new MyCheckBox(wrect, "control", p->stabEnt->name,
									nullptr,
									(ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW);
				SetControlDefaults(n, swid, p, msg);
				
				LValue	l = p->stabEnt->LValueFor(nullptr, par->stacker, stack);
				ResultValue	r	= l.CurrentValue();
				
				n->SetValue(r.BoolValue(nullptr));
				n->SetDivider(swid+5);
				destPanel->AddChild(n);
				break;
			}
			
			case S_TIME: {
				short swid = StringWidth(p->stabEnt->name);
				destPanel->GetItemRect(MIXER_MARGIN, swid + 85, 17, 5, wrect);
				
				BMessage	*msg = new BMessage(PARAM_SET);
				
				LValue	l = p->stabEnt->LValueFor(nullptr, par->stacker, stack);
				ResultValue	v	= l.CurrentValue();

				TimeCtrl		*n
						= new TimeCtrl(wrect, "control", p->stabEnt->name,
									nullptr,
									v.TimeValue()->metric,
									DRAW_VER_LBL,
									(ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP,
									(ulong)B_WILL_DRAW);
				SetControlDefaults(n, swid, p, msg);
	
				n->SetValue(v.TimeValue()->ticks);
				int lb = p->stabEnt->minVal.IntValue(nullptr);
				int mb = p->stabEnt->iniVal.IntValue(nullptr);
				int ub = p->stabEnt->maxVal.IntValue(nullptr);
				n->SetBounds(lb, ub, ub);
				n->SetDivider(swid+5);
				destPanel->AddChild(n);
				break;
			}
			
			case S_EXPRESSION: {
				short swid = StringWidth(p->stabEnt->name);
				destPanel->GetItemRect(MIXER_MARGIN, swid + 85, 17, 5, wrect);
				
				BMessage	*msg = new BMessage(PARAM_SET);
				
				BlockCtrl		*n
						= new BlockCtrl(wrect, "control", p->stabEnt->name,
									"", nullptr, S_EXPRESSION,
									(ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW, qua);
				SetControlDefaults(n, swid, p, msg);

				LValue	l = p->stabEnt->LValueFor(nullptr, par->stacker, stack);
				ResultValue	v	= l.CurrentValue();

				n->SetValue(v.ExpressionValue());
				n->SetDivider(swid+5);
				destPanel->AddChild(n);
				break;
			}
			
			case S_FLOAT: {
				short swid = StringWidth(p->stabEnt->name);
				destPanel->GetItemRect(MIXER_MARGIN, swid + 35, 17, 5, wrect);
				
				BMessage	*msg = new BMessage(PARAM_SET);
				
				LValue	l = p->stabEnt->LValueFor(nullptr, par->stacker, stack);
				ResultValue	v	= l.CurrentValue();

				NumCtrl		*n = new NumCtrl(wrect, "control", p->stabEnt->name,
									v.FloatValue(nullptr),
									nullptr, (ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW,
									FALSE);
				if (p->stabEnt->hasBounds) {
					float		m1 = p->stabEnt->minVal.FloatValue(stack),
								m2 = p->stabEnt->maxVal.FloatValue(stack);
					n->SetRanges(m1, (m1+m2) / 2, m2);
					n->SetInc((m2-m1)/100);
				} else {
					n->SetRanges(-199999,0,199999);
				}
				SetControlDefaults(n, swid, p, msg);
				n->SetDivider(swid+5);
				destPanel->AddChild(n);
				break;
			}
			
			case S_BYTE:
			case S_SHORT:
			case S_INT:
			case S_LONG: {
				long		lb, mb, ub;
				short		ncwid;
				short swid = StringWidth(p->stabEnt->name);
				char	buf[10];
				
				LValue	l = p->stabEnt->LValueFor(nullptr, par->stacker, stack);
				ResultValue	v	= l.CurrentValue();
				float			val=0;

				lb = p->stabEnt->minVal.IntValue(nullptr);
				mb = p->stabEnt->iniVal.IntValue(nullptr);
				ub = p->stabEnt->maxVal.IntValue(nullptr);
				
				switch(p->stabEnt->type) {
				case S_BYTE:
					ncwid = 35;
					val = v.ByteValue(nullptr);
					break;
				case S_SHORT:
					ncwid = 50;
					val = v.ShortValue(nullptr);
					break;
				case S_INT:
					val = v.IntValue(nullptr);
					ncwid = 65;
					break;
				case S_LONG:
					val = v.LongValue(nullptr);
					ncwid = 80;
					break;
	 			}
				destPanel->GetItemRect(MIXER_MARGIN,
							swid + ncwid, 17, 5, wrect);
				
				BMessage	*msg = new BMessage(PARAM_SET);
				
				NumCtrl		*n = new NumCtrl(wrect, "control", p->stabEnt->name,
									val,
									nullptr, (ulong)B_FOLLOW_LEFT|B_FOLLOW_TOP, (ulong)B_WILL_DRAW,
									TRUE);
				n->SetRanges(lb, mb, ub);
	 			
				SetControlDefaults(n, swid, p, msg);
				n->SetDivider(swid+5);
				destPanel->AddChild(n);
				break;
			}
			
			default:
				reportError("unexpected type in control var %s: %d\n",
					p->stabEnt->name, p->stabEnt->type);
			}
		} else if (p->stabEnt->controlInfo->displayMode == DISP_MODE_ENV) {
			short swid = StringWidth(p->stabEnt->name);
			BRect	erect(	MIXER_MARGIN, thePoint.y,
					 		350/* myWidth-MIXER_MARGIN */, thePoint.y+52);

			destPanel->GetItemRect(MIXER_MARGIN, swid + 43, 19, 5, wrect);

			EnvelopePanel *NV = new EnvelopePanel(erect, wrect, maxWidth-2*MIXER_MARGIN, p->envelope, DISPLAY_POT);

			destPanel->AddChild(NV);
		}
	}

}

GroupPanel *
ControlPanel::GroupPanelFor(StabEnt *S)
{
	if (S == nullptr || S->context->type != S_PARAMGROUP)
		return nullptr;
	for (short i=0; i<groupPanels.CountItems(); i++) {
		GroupPanel	*a = (GroupPanel *)groupPanels.ItemAt(i);
		if (a->groupSym == S->context)
			return a;
	}
	return nullptr;
}

void
ControlPanel::AddGroupPanel(StabEnt *S)
{
	if (S == nullptr) {
		return;
	}
	for (StabEnt *p=S->children; p!=nullptr; p=p->sibling) {
		if (p->type == S_PARAMGROUP) {
			BRect		wrect(	MIXER_MARGIN, MIXER_MARGIN,
								2*MIXER_MARGIN, 2*MIXER_MARGIN);
			GroupPanel	*a = new GroupPanel(wrect,
											maxWidth-2*MIXER_MARGIN,
											p);
			wrect=a->Bounds();
			GetItemRect(MIXER_MARGIN,
						wrect.right - wrect.left,
						wrect.bottom - wrect.top,
						MIXER_MARGIN,
						wrect);
			a->MoveTo(wrect.left,wrect.top);
			AddChild(a);
			groupPanels.AddItem(a);
			groupPanels.AddList(&a->groupPanels);
		}
	} 
}

void
ControlPanel::AddHigherFrame(BList &higherFrame, Qua *qua)
{
	BRect	wrect;

	if (Window()) Window()->Lock();
	
	for (short i=0; i<higherFrame.CountItems(); i++) {
		QuasiStack		*n = (QuasiStack *)higherFrame.ItemAt(i);

		if (n->isLocus || n->controlVariables) {

			wrect.Set(	MIXER_MARGIN, MIXER_MARGIN,
						2*MIXER_MARGIN, 2*MIXER_MARGIN);
			FramePanel *NV = new FramePanel(wrect,
											maxWidth-2*MIXER_MARGIN,
											n, qua);

			
			wrect=NV->Bounds();
			GetItemRect(MIXER_MARGIN,
					wrect.right - wrect.left,
					wrect.bottom - wrect.top,
					MIXER_MARGIN,
					wrect);
			NV->MoveTo(wrect.left,wrect.top);
			AddChild(NV);
		}				
	}
	if (Window()) Window()->Unlock();
}


void
ControlPanel::AttachedToWindow()
{
	for (ControlVariable *p=parameters; p!=nullptr; p=p->next) {
		if (p->screenControl) {
			p->screenControl->SetTarget(this);
		}
	}
	if (bBangButton)
		bBangButton->SetTarget(this);
}

void
ControlPanel::DisplayStatus(short st)
{
	switch (bBangType) {
	case CP_TYPE_TOGGLE: {
		Window()->Lock();
		((ToggleButton*)bBangButton)->SetState(st);
		Window()->Unlock();
		break;
	}
	case CP_TYPE_ONOFF: {
		Window()->Lock();
		((MyCheckBox*)bBangButton)->SetValue(st == STATUS_RUNNING);
		Window()->Unlock();
		break;
	}
	}
}

void
ControlPanel::RemoveControlVariables()
{
	if (Window()) Window()->Lock();
	short cnt = CountChildren();
	short j = 0;
	for (short i=0; i<cnt; i++) {
		BView *v = ChildAt(j);
		if (strcmp(v->Name(), "control") == 0) {
			RemoveChild(v);
			delete v;
		} else {
			j++;
		}
	}
	if (Window()) Window()->Unlock();
}


void
ControlPanel::RemoveHigherFrames()
{
	if (Window()) Window()->Lock();
	short cnt = CountChildren();
	short j = 0;
	for (short i=0; i<cnt; i++) {
		BView *v = ChildAt(j);
		if (strcmp(v->Name(), "frame") == 0) {
			RemoveChild(v);
			delete v;
		} else {
			j++;
		}
	}
	if (Window()) Window()->Unlock();
}

void
ControlPanel::RemoveGroupPanels()
{
	if (Window()) Window()->Lock();
	short cnt = CountChildren();
	short j = 0;
	for (short i=0; i<cnt; i++) {
		BView *v = ChildAt(j);
		if (strcmp(v->Name(), "group") == 0) {
			RemoveChild(v);
			groupPanels.RemoveItem(v);
			delete v;
		} else {
			j++;
		}
	}
	if (Window()) Window()->Unlock();
}

void
ControlPanel::MouseDown(BPoint pt)
{
	MakeFocus(TRUE);
}

//void
//SymEditPanel::ReScale(float x)
//{
//	maxWidth = x;
//	for (short i=0; i<CountChildren(); i++) {
//		BView	*V = ChildAt(i);
//		if (strcmp(V->Name(), "symbol") == 0) {
//			SymEditPanel	*p = (SymEditPanel *)V;
//			if (p->CountChildren() > 1)
//				p->ReScale(x-2*MIXER_MARGIN);
//		}
//	}
//	ArrangeChildren();
//}
//
//void
//SymEditPanel::FrameResized(float x, float y)
//{
//	BView *v;
//	myHeight = y;
//	myWidth = x;
//	if ((v=Parent()) == nullptr) {
//	} else if (strcmp(v->Name(),"symbol") == 0) {
//		SymEditPanel	*s = (SymEditPanel*)v;
//		s->ArrangeChildren();
//	} else if (strcmp(v->Name(), "back") == 0) {
//		Window()->ResizeBy(0/*myWidth-origW*/, myHeight-origH);
//	}
//	Invalidate();
//}
//
void
ControlPanel::ArrangeChildren()
{
	if (Window()) Window()->Lock();
	BRect	wrect;
	if (bBangButton) {
		wrect = bBangButton->Frame();
		myHeight = wrect.bottom+MIXER_MARGIN;
		myWidth = wrect.right+MIXER_MARGIN;
		rowHeight = MIXER_MARGIN;
		thePoint.Set(myWidth,MIXER_MARGIN);
	} else if (vName) {
		wrect = vName->Frame();
		myHeight = wrect.bottom+MIXER_MARGIN;
		myWidth = wrect.right+MIXER_MARGIN;
		rowHeight = MIXER_MARGIN;
		thePoint.Set(myWidth,MIXER_MARGIN);
	} else {
		myHeight = myWidth = 2*MIXER_MARGIN;
		rowHeight = MIXER_MARGIN;
		thePoint.Set(MIXER_MARGIN,MIXER_MARGIN);
	}
	for (ControlVariable *p=parameters; p!=nullptr; p=p->next) {
		ControlPanel	*destPanel = GroupPanelFor(p->stabEnt);
		if (destPanel == nullptr)
			destPanel = this;
		if (p->screenControl) {
			wrect = p->screenControl->Frame();
			destPanel->GetItemRect(MIXER_MARGIN,
						wrect.right-wrect.left,
						wrect.bottom-wrect.top,
						5, wrect);
			p->screenControl->MoveTo(wrect.left,wrect.top);
		} else if (p->envelopePanel &&
					p->envelopePanel->displayMode == DISPLAY_POT) {
			wrect = p->envelopePanel->Frame();
			destPanel->GetItemRect(MIXER_MARGIN,
						wrect.right-wrect.left,
						wrect.bottom-wrect.top,
						5, wrect);
			p->envelopePanel->MoveTo(wrect.left,wrect.top);
		}
	}
	
	for (ControlVariable *p=parameters; p!=nullptr; p=p->next) {
		ControlPanel	*destPanel = GroupPanelFor(p->stabEnt);
		if (destPanel == nullptr)
			destPanel = this;
		if (	p->envelopePanel &&
				p->envelopePanel->displayMode != DISPLAY_POT) {
			wrect = p->envelopePanel->Frame();
			destPanel->GetItemRect(MIXER_MARGIN,
						wrect.right-wrect.left,
						wrect.bottom-wrect.top,
						5, wrect);
			p->envelopePanel->MoveTo(wrect.left,wrect.top);
		}
	}
	
	for (short i=0; i<CountChildren(); i++) {
		BView	*V = ChildAt(i);
		if		(strcmp(V->Name(), "frame") == 0 ||
				 strcmp(V->Name(), "instance") == 0 ||
				 strcmp(V->Name(), "schedulable") == 0 ||
				 strcmp(V->Name(), "panel") == 0) {
			Panel	*panel = (Panel *)V;
			GetItemRect(MIXER_MARGIN, panel->myWidth, panel->myHeight, 5, wrect);
			panel->MoveTo(wrect.left,wrect.top);
		} else if	(strcmp(V->Name(), "group") == 0) {
			GroupPanel	*panel = (GroupPanel *)V;
			panel->ArrangeChildren();
			GetItemRect(MIXER_MARGIN, panel->myWidth, panel->myHeight, 5, wrect);
			panel->MoveTo(wrect.left,wrect.top);
		}
	}
	ResizeTo(myWidth, myHeight);
	if (Window()) Window()->Unlock();

}

ControlPanel::~ControlPanel()
{
}