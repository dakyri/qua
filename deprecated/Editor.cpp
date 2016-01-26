
#ifdef __INTEL__ 
#include <SupportKit.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#endif

#include "Editor.h"
#include "Sym.h"
#include "Quapp.h"
#include "SymEditPanel.h"
#include "QuaObject.h"
#include "Executable.h"
#include "PoolEditor.h"
#include "SampleEditor.h"
#include "Colors.h"
#include "Channel.h"
#include "ScrawleBar.h"
#include "DataView.h"

EditBackView::EditBackView(BRect r):
	TiledView(r, "back", B_FOLLOW_ALL, B_WILL_DRAW)
{
	tracking = MOUSE_TRACK_NOT;
}

void
EditBackView::MouseDown(BPoint p)
{
	Editor	*e = (Editor *)Window();
	if (e->dataEdit && e->ctrlEdit) {
		track_cr = e->ctrlBack->Frame();
		track_dr = e->dataEdit->Frame();
		if ( (p.y >= track_cr.bottom) && (p.y <= track_dr.top)) {

			tracking = MOUSE_TRACK_RESIZE;
			track_pt = p;
			SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY);

		} else {
			TiledView::MouseDown(p);
		}
	} else {
		TiledView::MouseDown(p);
	}
}
	

void
EditBackView::MouseMoved(BPoint pt, uint32 wh, const BMessage *msg)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_RESIZE) {
		Editor	*e = (Editor *)Window();

		float	amt = pt.y - track_pt.y;
		e->dataEdit->MoveTo(MARGIN,track_cr.bottom+MARGIN+amt);
		e->dataEdit->ReScale(
					track_dr.right-track_dr.left,
					track_dr.bottom-track_dr.top-amt);
		e->ctrlBack->ResizeTo(
					track_cr.right-track_cr.left,
					track_cr.bottom-track_cr.top+amt);
		e->ctrlScroll->ResizeTo(B_V_SCROLL_BAR_WIDTH,
					track_cr.bottom-track_cr.top+2+amt);
		
		float	u = e->ctrlEdit->Bounds().bottom -
							 (track_cr.bottom-track_cr.top + amt);
		u = Max(0,u);
		e->ctrlScroll->SetRange(0, u);
		
		float		minH, maxH, minW, maxW;
		e->GetSizeLimits(&minW,&maxW,&minH,&maxH);
		minH = 3*MARGIN + track_dr.bottom-track_dr.top-amt + 20;
		e->SetSizeLimits(minW,maxW,minH,maxH);
		Invalidate();
	}
}


void
EditBackView::MouseUp(BPoint where)
{
	if (tracking == MOUSE_TRACK_NOT)
		return;
	if (tracking == MOUSE_TRACK_RESIZE) {
	}
	tracking = MOUSE_TRACK_NOT;
}

void
EditBackView::Draw(BRect r)
{
	TiledView::Draw(r);
	if (Window()) {
		Editor	*e = (Editor *)Window();
		if (e->ctrlBack) {
			SetPenSize(2);
			LoweredBox(this, e->ctrlBack->Frame(), e->ctrlEdit->ViewColor(), false);
		}
	}
}

Editor::Editor(BRect r, Qua *q, StabEnt *sym):
	BWindow(r, sym->name, B_TITLED_WINDOW,
			B_WILL_ACCEPT_FIRST_CLICK|B_NOT_ZOOMABLE)
{
	BRect	bnd = Bounds();
	back = new EditBackView(bnd);
	back->SetTilingPattern(*quapp->backgroundPattern);
	AddChild(back);
	
	ctrlEdit = nullptr;
	dataEdit = nullptr;
	ctrlScroll = nullptr;
	ctrlBack = nullptr;

	switch (sym->type) {
	case S_POOL: {
		ctrlEdit = new SchedulableEditPanel(BRect(0,0,
												2*MIXER_MARGIN,
												2*MIXER_MARGIN),
								bnd.right-2*MARGIN-B_V_SCROLL_BAR_WIDTH-2,
								sym, q);
		BRect	cr = ctrlEdit->Frame();
		
		dataEdit = new PoolEditor(BRect(MARGIN,cr.bottom+MARGIN,
										bnd.right-MARGIN,bnd.bottom-MARGIN),
								bnd.right-2*MARGIN, sym->PoolValue());
		BRect	dr = dataEdit->Frame();
		ctrlBack = new ScrawleView(BRect(MARGIN,MARGIN,
									bnd.right-MARGIN-B_V_SCROLL_BAR_WIDTH-2,
									dr.top-MARGIN),
							"back", B_FOLLOW_TOP|B_FOLLOW_LEFT, (uint32)nullptr);
	    ctrlScroll = new ScrawleBar(BRect(bnd.right-MARGIN-B_V_SCROLL_BAR_WIDTH,MARGIN-1,
	    								bnd.right-MARGIN,dr.top-MARGIN+1),
	    							"", ctrlBack, ctrlBack,
	    							0, cr.top, 2, B_VERTICAL);							
	    ctrlScroll->SetScalable(false);
	    ctrlScroll->SetResizingMode(B_FOLLOW_RIGHT);
	    
		float	u = cr.bottom-(dr.top-2*MARGIN);
		u = Max(0,u);
		ctrlBack->AddChild(ctrlEdit);
		back->AddChild(ctrlBack);
		back->AddChild(ctrlScroll);
		back->AddChild(dataEdit);
		ctrlBack->SetViewColor(ctrlEdit->ViewColor());
		ctrlScroll->SetRange(0,u);
		break;
	}

	case S_SAMPLE: {
		ctrlEdit = new SchedulableEditPanel(BRect(0,0,
											2*MIXER_MARGIN,2*MIXER_MARGIN),
								bnd.right-2*MARGIN-B_V_SCROLL_BAR_WIDTH-2,
								sym, q);
		BRect	cr = ctrlEdit->Frame();
		dataEdit = new SampleEditor(BRect(MARGIN,cr.bottom+2*MARGIN,
										bnd.right-MARGIN,bnd.bottom-MARGIN),
								bnd.right-2*MARGIN, sym->SampleValue());
		BRect	dr = dataEdit->Frame();
		ctrlBack = new ScrawleView(BRect(MARGIN,MARGIN,
									bnd.right-MARGIN-B_V_SCROLL_BAR_WIDTH-2, dr.top-MARGIN),
							"back", B_FOLLOW_TOP|B_FOLLOW_LEFT, (uint32)nullptr);
							
	    ctrlScroll = new ScrawleBar(BRect(bnd.right-MARGIN-B_V_SCROLL_BAR_WIDTH,MARGIN-1,
	    								bnd.right-MARGIN,dr.top-MARGIN+1),
	    							"", ctrlBack, ctrlBack,
	    							0, cr.top, 2, B_VERTICAL);
	    ctrlScroll->SetScalable(false);
	    ctrlScroll->SetResizingMode(B_FOLLOW_RIGHT);
	    							
		float	u = cr.bottom-(dr.top-2*MARGIN);
		u = Max(0,u);
		ctrlBack->AddChild(ctrlEdit);
		back->AddChild(ctrlBack);
		back->AddChild(ctrlScroll);
		back->AddChild(dataEdit);
		ctrlBack->SetViewColor(ctrlEdit->ViewColor());
		ctrlScroll->SetRange(0,u);
		break;
	}
	
	case S_VOICE:
	case S_APPLICATION: {
		ctrlEdit = new SchedulableEditPanel(
									BRect(0,0,
										2*MIXER_MARGIN,2*MIXER_MARGIN),
									bnd.right-2*MARGIN-B_V_SCROLL_BAR_WIDTH-2,
									sym, q);
								
		BRect	cr = ctrlEdit->Frame();
		ctrlBack = new ScrawleView(BRect(MARGIN,MARGIN,
									bnd.right-MARGIN-B_V_SCROLL_BAR_WIDTH-2,
									bnd.bottom-MARGIN),
							"back", B_FOLLOW_ALL, (uint32)nullptr);
		ctrlBack->SetViewColor(ctrlEdit->ViewColor());
		float	u = cr.bottom-(bnd.bottom-2*MARGIN);
		u = Max(0,u);
	    ctrlScroll = new ScrawleBar(BRect(bnd.right-MARGIN-B_V_SCROLL_BAR_WIDTH,MARGIN-1,
	    								bnd.right-MARGIN,bnd.bottom-MARGIN+1),
	    							"", ctrlBack, ctrlBack,
	    							0, u, 2, B_VERTICAL);
	    ctrlScroll->SetScalable(false);
	    ctrlScroll->SetResizingMode(B_FOLLOW_RIGHT);

		ctrlBack->AddChild(ctrlEdit);
		back->AddChild(ctrlBack);
		back->AddChild(ctrlScroll);
		ctrlScroll->SetRange(0,u);
		
		break;
	}

	case S_METHOD: {
		ctrlEdit = new ExecutableEditPanel(BRect(0,0,
											2*MIXER_MARGIN,2*MIXER_MARGIN),
								bnd.right-2*MARGIN-B_V_SCROLL_BAR_WIDTH-2,
								sym, q);
		BRect	cr = ctrlEdit->Frame();
		ctrlBack = new ScrawleView(BRect(MARGIN,MARGIN,
									bnd.right-MARGIN-B_V_SCROLL_BAR_WIDTH-2,
									bnd.bottom-MARGIN),
							"back", B_FOLLOW_ALL, (uint32)nullptr);
		ctrlBack->SetViewColor(ctrlEdit->ViewColor());
		float	u = cr.bottom-(bnd.bottom-2*MARGIN);
		u = Max(0,u);
	    ctrlScroll = new ScrawleBar(BRect(bnd.right-MARGIN-B_V_SCROLL_BAR_WIDTH,MARGIN-1,
	    								bnd.right-MARGIN,bnd.bottom-MARGIN+1),
	    							"", ctrlBack, ctrlBack,
	    							0, u, 2, B_VERTICAL);
	    ctrlScroll->SetScalable(false);
	    ctrlScroll->SetResizingMode(B_FOLLOW_RIGHT);
	    
		ctrlBack->AddChild(ctrlEdit);
		back->AddChild(ctrlBack);
		back->AddChild(ctrlScroll);
		ctrlScroll->SetRange(0,u);
		break;
	}

	case S_CHANNEL: {
		ctrlEdit = new ChannelEditPanel(BRect(0,0,
											2*MIXER_MARGIN,2*MIXER_MARGIN),
								bnd.right-2*MARGIN-B_V_SCROLL_BAR_WIDTH-2,
								sym, q);
		BRect	cr = ctrlEdit->Frame();
		ctrlBack = new ScrawleView(BRect(MARGIN,MARGIN,
									bnd.right-MARGIN-B_V_SCROLL_BAR_WIDTH-2,
									bnd.bottom-MARGIN),
							"back", B_FOLLOW_ALL, (uint32)nullptr);
		ctrlBack->SetViewColor(ctrlEdit->ViewColor());
		float	u = cr.bottom-(bnd.bottom-2*MARGIN);
		u = Max(0,u);
	    ctrlScroll = new ScrawleBar(BRect(bnd.right-MARGIN-B_V_SCROLL_BAR_WIDTH,MARGIN-1,
	    								bnd.right-MARGIN,bnd.bottom-MARGIN+1),
	    							"", ctrlBack, ctrlBack,
	    							0, u, 2, B_VERTICAL);
	    ctrlScroll->SetScalable(false);
	    ctrlScroll->SetResizingMode(B_FOLLOW_RIGHT);

		ctrlBack->AddChild(ctrlEdit);
		back->AddChild(ctrlBack);
		back->AddChild(ctrlScroll);
		ctrlScroll->SetRange(0,u);
		break;
	}
	
	}

	Show();
}

Editor::~Editor()
{
	if (ctrlEdit) {
		if (Executable *E=ctrlEdit->mySym->ExecutableValue()) {
			E->symObject->editor = nullptr;
		} else if (Channel *C=ctrlEdit->mySym->ChannelValue()) {
			C->editor = nullptr;
		}
	}
}

void
Editor::MessageReceived(BMessage *inMsg)
{
	BWindow::MessageReceived(inMsg);
}

bool
Editor::QuitRequested()
{
	if (ctrlEdit) {
		Executable	*E = ctrlEdit->mySym->ExecutableValue();
		if (E && E->symObject)
			E->symObject->editor = nullptr;
	}
//	if (ctrlEdit->mySym->type == S_SAMPLE) {
//		((SampleEditor*)dataEdit)->sampleView->stillHere = false;
//	}

	return dataEdit? dataEdit->QuitRequested():true;
}

void
Editor::FrameResized(float x, float y)
{
	if (ctrlEdit) {
		float		ncw = x - 2*MARGIN-B_V_SCROLL_BAR_WIDTH-2;
		if (dataEdit) {
			if (Abs(ncw-ctrlEdit->maxWidth) > 20)
				ctrlEdit->ReScale(ncw);
			BRect	dr = dataEdit->Frame();
			ctrlBack->ResizeTo(ncw,
								y - 3*MARGIN - (dr.bottom - dr.top));
			ctrlScroll->ResizeTo(B_V_SCROLL_BAR_WIDTH, y - 3*MARGIN - (dr.bottom - dr.top)+2);
			BRect	cr = ctrlBack->Frame();
			dataEdit->MoveTo(MARGIN,cr.bottom+MARGIN);
			dataEdit->ReScale(x - 2*MARGIN, y-cr.bottom-2*MARGIN);
			BRect	cer = ctrlEdit->Frame();
			float	u = cer.bottom-(cr.bottom - cr.top);
			u = Max(0,u);
			ctrlScroll->SetRange(0, u);
		} else {
			if (Abs(ncw-ctrlEdit->maxWidth) > 20)
				ctrlEdit->ReScale(ncw);
			ctrlScroll->ResizeTo(B_V_SCROLL_BAR_WIDTH, y - 2*MARGIN+2);
			BRect	cr = ctrlEdit->Frame();
			float	u = cr.bottom-(y-2*MARGIN);
			u = Max(0,u);
			ctrlScroll->SetRange(0, u);
		}

		back->Invalidate();
	}
}