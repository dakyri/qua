
__declspec(dllexport) void
NumCtrl::Draw(BRect reg)
{
//	BRect	Box(textrect.left-2,textrect.top-2,
//				textrect.right+1,textrect.bottom+1);
	BPoint	Pos(0,textrect.bottom-2);
	
//	SetViewColor(Parent()->ViewColor());
	
	SetLowColor(ViewColor());
	SetHighColor(black);
	DrawString(Label(), Pos);
	textview->Draw(textrect);
	Pos.Set(textrect.left-2,textrect.bottom+1);	MovePenTo(Pos);
	SetHighColor(Darker(ViewColor()));SetPenSize(1.0);
	Pos.Set(textrect.left-2,textrect.top-2);	StrokeLine(Pos);
	Pos.Set(textrect.right+1,textrect.top-2);	StrokeLine(Pos);
	SetHighColor(Lighter(ViewColor()));SetPenSize(1.0);
	Pos.Set(textrect.right+1,textrect.bottom+1);StrokeLine(Pos);
	Pos.Set(textrect.left-2,textrect.bottom+1);	StrokeLine(Pos);
//	StrokeRect(Box);
}
