#include "PoolEditor.h"
#include "Pool.h"
#include "Q3dStuff.h"
#include "QuaQua.h"
#include "LalaLand.h"

Editor::Editor(BRect r, char *title)
	: BWindow(r, title, B_TITLED_WINDOW, nullptr)
{
}

PoolEditor::PoolEditor(BRect inRect, Pool *P)
	: Editor( inRect, "Test editor")
{
	pool = P;

	BRect	Box = Bounds();

	edit = new PoolEditView(Box, P);
	AddChild(edit);
	Show();
}

PoolEditor::~PoolEditor()
{
	pool->EditWindow = nullptr;
}

bool
PoolEditor::QuitRequested()
{
	bool		succ;
	short		val;
	BAlert*		about = new BAlert( "", "Save this shgite?",
					"Cancel", "Don't Save", "Save" );
	if ((val=about->Go()) == 2) {
		succ = TRUE;
		return succ;
	} else if (val == 1) {
		return TRUE;
	} else {
		return FALSE;
	}
}



PoolEditView::PoolEditView(BRect rect, Pool *P)
	: BView(rect, "vd", B_FOLLOW_ALL, B_WILL_DRAW)
{
	pool = P;
	StreamView = new Stream3dView(Bounds(), &P->Stream, lalaLand);
	AddChild(StreamView);
//	StreamView->World()->SetUpdateRate(20.0);
}

RGBAColor         Colors[] = {	{ 0.8, 0.8, 0.8, 0},	// 0. dark red
								{ 1.0, 0.1, 0.1, 0},	// 1. red
								{ 1.0, 0.1, 1.0, 0},	// 2. mauve	
								{ 0.6, 0.1, 0.6, 0},	// 3. purple
								{ 0.1, 0.6, 0.6, 0},	// 4. dark aqua
								{ 0.1, 0.6, 1.0, 0},	// 5. gun blue
								{ 0.1, 0.6, 0.3, 0},	// 6. dark green
								{ 0.1, 1.3, 1.0, 0},	// 7. soft blue green
								{ 0.4, 1.0, 0.6, 0},	// 8. leaf green
								{ 0.8, 1.0, 0.4, 0},	// 9. yellow green
								{ 1.1, 1.0, 0.1, 0},	// 10. yellow
								{ 1.6, 0.6, 0.3, 0},	// 11. orange
								{ 0.1, 0.1, 1.3, 0}};
								
								
B3dVector *
Item3dModel::ItemCube(StreamItem *p)
{
	static B3dVector	shite;
	
	shite.Set(.1*((StreamTone*)p)->Tone.duration, .1, .1);
	
	return &shite;
}

Item3dModel::Item3dModel(B3dWorld *w, StreamItem *p):
	B3dCube("note", w, ItemCube(p))
{
	B3dMaterial material;
	
	material.SetColor( &Colors[12]);
	material.SetProperties(material.Properties() & 
		~(B_MATERIAL_IS_SMOOTH|B_MATERIAL_IS_SHINY));
	WriteFaceMaterials(B_OBJECT_SELECTION_ID,
		&material,B_ONE_TO_ALL_FACES);
}

void	
Item3dModel::TouchDown(
							B3dTouchDesc *	touch,
							B3dVector *		touchOnScreen,
							uint32			buttons)

{
	fprintf(stderr, "touch at %g %g %g\n",
		touch->touchInWorld.x,
		touch->touchInWorld.y,
		touch->touchInWorld.z);
	fprintf(stderr, "touch scr %g %g %g\n",
		touchOnScreen->x,
		touchOnScreen->y,
		touchOnScreen->z);
}


void
Item3dModel::TouchMoved(
			B3dTouchDesc *	touch,
			B3dVector *		touchOnScreen,
			uint32			buttons)
{
	fprintf(stderr, "touch at %g %g %g\n",
		touch->touchInWorld.x,
		touch->touchInWorld.y,
		touch->touchInWorld.z);
	fprintf(stderr, "touch scr %g %g %g\n",
		touchOnScreen->x,
		touchOnScreen->y,
		touchOnScreen->z);
}



B3dVector *
Grid3dModel::GridCube()
{
	static B3dVector	shice;
	
	shice.Set(2000, 0.01, 0.01);
	
	return &shice;
}


Grid3dModel::Grid3dModel(B3dWorld *w, int Note):
	B3dCube("not", w, GridCube())
{
	B3dMaterial material;
	
	myNote = Note;
	material.SetColor( &Colors[Note%12]);
	material.SetProperties(material.Properties() & 
		~(B_MATERIAL_IS_SMOOTH|B_MATERIAL_IS_SHINY));
	WriteFaceMaterials(B_OBJECT_SELECTION_ID,
		&material,B_ONE_TO_ALL_FACES);
}


void	
Grid3dModel::TouchDown(
							B3dTouchDesc *	touch,
							B3dVector *		touchOnScreen,
							uint32			buttons)

{
}

void
Grid3dModel::TouchMoved(
			B3dTouchDesc *	touch,
			B3dVector *		touchOnScreen,
			uint32			buttons)
{
}


void
Stream3dView::KeyDown(const char * ch, int32 nb)
{
	long	mods = modifiers();
	B3dRadialOptions  *LensOpts;

	B3dVector *at = Camera()->ViewPoint()->Origin();
	B3dMatrix *rot = Camera()->ViewPoint()->Rotation();
	switch (ch[0]) {
	case B_LEFT_ARROW:
		if (mods & B_SHIFT_KEY) {
			B3dVector	rotv;
			float		alpha;
			rot->GetAxialRotateZ(&rotv, &alpha);
			rotv.x -= 0.1;
			rot->Set(&rotv, alpha);
			Camera()->ViewPoint()->SetRotation(rot);
		} else if (mods & B_CONTROL_KEY) {
			Camera()->Lens()->GetOptions(&LensOpts);
			LensOpts->zoom *= 1.05;
			Camera()->Lens()->SetOptions();
		} else {
			at->x -= 0.1;
		}
		break;
	case B_RIGHT_ARROW:
		if (mods & B_SHIFT_KEY) {
			B3dVector	rotv;
			float		alpha;
			rot->GetAxialRotateZ(&rotv, &alpha);
			rotv.x += 0.1;
			rot->Set(&rotv, alpha);
			Camera()->ViewPoint()->SetRotation(rot);
		} else if (mods & B_CONTROL_KEY) {
			Camera()->Lens()->GetOptions(&LensOpts);
			LensOpts->zoom *= 1.05;
			Camera()->Lens()->SetOptions();
		} else {
			at->x += 0.1;
		}
		break;
	case B_UP_ARROW:	
		if (mods & B_SHIFT_KEY) {
			B3dVector	rotv;
			float		alpha;
			rot->GetAxialRotateZ(&rotv, &alpha);
			fprintf(stderr, "%g %g %g\n", rotv.x, rotv.y, rotv.z);
			rotv.x -= 0.1;
			rot->Set(&rotv, alpha);
			rot->GetAxialRotateZ(&rotv, &alpha);
			fprintf(stderr, "%g %g %g\n", rotv.x, rotv.y, rotv.z);
			Camera()->ViewPoint()->SetRotation(rot);
		} else if (mods & B_CONTROL_KEY) {
			Camera()->Lens()->GetOptions(&LensOpts);
			LensOpts->zoom *= 1.05;
			Camera()->Lens()->SetOptions();
		} else {
			at->y += 0.1;
		}
		break;
	case B_DOWN_ARROW:
		if (mods & B_SHIFT_KEY) {
			B3dVector	rotv;
			float		alpha;
			rot->GetAxialRotateZ(&rotv, &alpha);
			fprintf(stderr, "%g %g %g\n", rotv.x, rotv.y, rotv.z);
			rotv.x += 0.1;
			rot->Set(&rotv, alpha);
			rot->GetAxialRotateZ(&rotv, &alpha);
			fprintf(stderr, "%g %g %g\n", rotv.x, rotv.y, rotv.z);
			Camera()->ViewPoint()->SetRotation(rot);
		} else if (mods & B_CONTROL_KEY) {
			Camera()->Lens()->GetOptions(&LensOpts);
			LensOpts->zoom /= 1.05;
			Camera()->Lens()->SetOptions();
		} else {
			at->y -= 0.1;
			
		}
		break;
	default: {
//		B3dView::KeyDown(ch);
	}
	}
}

void
Stream3dView::MouseDown(BPoint pt)
{
	B3dView::MouseDown(pt);
}

Stream3dView::Stream3dView(BRect rect, Stream *S, B3dUniverse *uni):
	B3dView("stream", rect, uni, B_3D_BLACK_BACKGROUND)

{
	myStream = S;

}

void
Stream3dView::MoveTo(uchar note, Time &t)
{
}

void
Stream3dView::AttachedToWindow()
{
	B3dAmbientLight   *aLight;
	B3dParallelLight  *dLight;
	B3dVector         place, axis;
	RGBAColor         Color;
	B3dFaceBody       *Prev, *Body, *Copy, *Sphere;
	B3dMatrix         rotation;
	StreamItem			*p;
	int					i;
	B3dVector			origin;

	B3dView::AttachedToWindow();
	
//	rotation.Set(1.5708, 1.5708, 0.0);
	rotation.Set(0, 0, 0.0);

	Universe()->Lock();
	origin.Set(0,0,0);
	for (i=0; i<128; i++) {
		Body = new Grid3dModel(World(), i);
		place.Set(0.0, .1*i, 0.0);
		Body->SetOrigin(&place);
		Body->SetRotation(&rotation);
	}
	
	for (p = myStream->Head; p != nullptr; p = p->Next) {
		fprintf(dbfp, "yes, ........\n");
		place.Set(
				.1*p->Time.Value() + .05*((StreamTone*)p)->Tone.duration,
				.1*((StreamTone*)p)->Tone.pitch,
				0.1);
		fprintf(dbfp, "%g %g %g\n", place.x, place.y, place.z);
		Body = new Item3dModel(World(), p);
		Body->SetOrigin(&place);
		Body->SetRotation(&rotation);
		Prev = Body;
	}
	fprintf(dbfp, "yes, ...and ...\n");
	
// Create the ambient light
// define the color of the ambient light		
// create the ambient light
// add the ambient light to the world of the 3dView
	Color.Set(1.0, 1.0, 1.0, 0.0);
	aLight = new B3dAmbientLight("Ambient white", World(), 0.8, &Color);
//	World()->AddLight(aLight);
		
// Create a parallel light (like a sun)
// define the initial direction of the parallel light
// create the parallel light, using the same color than the ambient light
// define an spherical rotation attache to that light, without random speed
// parameters to obtain a irregular rotation
// add the parallel light to the world of the 3dView 
	axis.Set(-0.5, -0.5, -0.5);
	dLight = new B3dParallelLight(
			"Distant white",
			World(),
			0.8,
			&Color,
			&Color,
			&axis);
//	dLight->LinkTo(1.7, -2.2, 0.7);
//	World()->AddLight(dLight);
		
// Set the position of the 3dCamera into the world of the 3d View
// move the 3dCamera to specific coordinates
// Point the camare to the rocketry

	Camera()->ViewPoint()->SetOrigin(0, 0.0, 20.0);
	origin.z = 3.6;
	Camera()->ViewPoint()->LookAt(&origin);
	
// That define the size in pixel of an object of 1.0 unit width seen at a distant
// of 1.0 unit (in that case, a square of 1x1, look at from the front at a distant
// of 1 will be seen as 400x400 pixels).
	B3dRadialOptions *opt_ptr;
	Camera()->Lens()->GetOptions(&opt_ptr);
	opt_ptr->zoom = 300.0;
	Camera()->Lens()->SetOptions();
	Universe()->Unlock();
	fprintf(dbfp, "done\n");
}
