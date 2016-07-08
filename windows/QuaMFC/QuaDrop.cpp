
//#define _AFXDLL
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include "stdafx.h"
#include "qua_version.h"
#include "afxole.h"

#include "QuaDrop.h"
#include "Sym.h"

// registered format codes for pointers to qua internal structures
UINT		QuaDrop::clipFormat;
UINT		QuaDrop::instanceFormat;
UINT		QuaDrop::voiceFormat;
UINT		QuaDrop::sampleFormat;
UINT		QuaDrop::schedulableFormat;
UINT		QuaDrop::portFormat;
UINT		QuaDrop::builtinFormat;
UINT		QuaDrop::templateFormat;
UINT		QuaDrop::vstpluginFormat;
UINT		QuaDrop::methodFormat;

QuaDrop::QuaDrop(void)
{
	type = DropType::NOTHING;
}

QuaDrop::~QuaDrop(void)
{
}

bool
QuaDrop::Initialize()
{
	if (!AfxOleInit()) 	{
		//OleInitialize(NULL)))
		return false;
	}
	clipFormat = ::RegisterClipboardFormat("QuaDragDropClip");
	instanceFormat = ::RegisterClipboardFormat("QuaDragDropInstance");
	voiceFormat = ::RegisterClipboardFormat("QuaDragDropVoice");
	sampleFormat = ::RegisterClipboardFormat("QuaDragDropSample");
	schedulableFormat = ::RegisterClipboardFormat("QuaDragDropSchedulable");
	portFormat = ::RegisterClipboardFormat("QuaDragDropPort");
	builtinFormat = ::RegisterClipboardFormat("QuaDragDropBuiltin");
	templateFormat = ::RegisterClipboardFormat("QuaDragDropTemplate");
	vstpluginFormat = ::RegisterClipboardFormat("QuaDragDropVstPlugin");
	methodFormat = ::RegisterClipboardFormat("QuaDragDropVstPlugin");

	return true;
}

bool
QuaDrop::SetSymbolSource(COleDataSource *src, UINT format, StabEnt *sym)
{
	HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE,sizeof(qua_symbol_clip));
	if (hData == NULL) {
		return false;
	}
	qua_symbol_clip *clip = (qua_symbol_clip *)GlobalLock(hData);
	clip->symbol = sym;
	GlobalUnlock(hData);
	src->CacheGlobalData(format,hData);
	return true;
}

// the following looks like a really neat way of doing something that windross makes mucky
// http://www.codeproject.com/combobox/oledragdroplistbox.asp
/*
void CMyDragAndDropWnd::StartDrag()
{
   //create the COleDataSource, and attach the data to it
   COleDataSource DataSource;
 
   //create a chunck of memory that will hold "This is a test" 15 chars
   HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE,15);
   char *pChar = (char *)GlobalLock(hData);
   strcpy(pChar,"This is a test");
   GlobalUnlock(hData);
 
   if (hData)
   {
      //attach the data to the COleDataSource Object
      DataSource.CacheGlobalData(CF_TEXT,hData);
 
      //allow the user to drag it.
      DROPEFFECT DropEffect = 
         DataSource.DoDragDrop(GetDragItemEffects(m_DraggedIndex));
 
      //Once DoDragDrop returns we can check the return value stored in DropEffect
      //to see what kind of dropping happened. Like move, copy or shortcut
   }
}
*/

// a neat way to drag and drop alpha blended bitmaps
// http://www.thecodeproject.com/shell/dropsourcehelpermfc.asp
/*
CComPtr<IDragSourceHelper> pHelper;
HRESULT hr = CoCreateInstance(CLSID_DragDropHelper,NULL,
  CLSCTX_ALL,IID_IDragSourceHelper,(LPVOID*)&pHelper);
if SUCCEEDED(hr) {
  // load a testbitmap
  CBitmap bmp;
  GetBitmap(bmp);
  // prepare the SHDRAGIMAGE structure
  BITMAP bmpInfo;
  bmp.GetBitmap(&bmpInfo);
  // fill the drag&drop structure  
  SHDRAGIMAGE info;
  info.sizeDragImage.cx = bmpInfo.bmWidth;
  info.sizeDragImage.cy = bmpInfo.bmHeight;
  info.ptOffset.x = 0;
  info.ptOffset.y = 0;
  info.hbmpDragImage = (HBITMAP)bmp.Detach();
  info.crColorKey = GetSysColor(COLOR_WINDOW);
  // this call assures that the bitmap will be dragged around
  hr = pHelper->InitializeFromBitmap(
    // drag&drop settings
    &info,
    // a pointer to the data object, the helper will store 
    // a reference to itself into this object
    (IDataObject*)source.GetInterface(&IID_IDataObject)
  );
  // in case of an error we need to destroy the image, 
  // else the helper object takes ownership
  if FAILED(hr)
    DeleteObject(info.hbmpDragImage);
}
Class COleDataSourceEx : public COleDataSource
{
public:
  // helper methods to fix IDropSourceHelper
  DECLARE_INTERFACE_MAP()
  // handle name events
  BEGIN_INTERFACE_PART(DataObj, IDataObject)
    INIT_INTERFACE_PART(COleDataSourceEx, DataObject)
    // IDataObject
    …
    STDMETHOD(SetData)(LPFORMATETC, LPSTGMEDIUM, BOOL);
  END_INTERFACE_PART(DataObj)
};
STDMETHODIMP COleDataSourceEx::XDataObj::SetData(LPFORMATETC pFormatetc, 
    LPSTGMEDIUM pmedium, BOOL fRelease)
{
  METHOD_PROLOGUE(COleDataSourceEx, DataObj)
  // normal processing
  HRESULT hr = pThis->m_xDataObject.SetData(pFormatetc,pmedium,fRelease);
  if (hr==DATA_E_FORMATETC) {
    // cache the data explicitly
    pThis->CacheData(pFormatetc->cfFormat,pmedium,pFormatetc);
    return S_OK;
  }
  // normal error
  return hr;
}
*/
// other helpful referenences
// http://web1.codeproject.com/file/cppfilesdragdrop.asp?print=true
// http://www.codeproject.com/clipboard/dragsource.asp
void
QuaDrop::Clear()
{
	switch (type) {
		case DropType::NOTHING:
			break;
		case DropType::FILES:
			delete data.filePathList;
			break;
		case DropType::AUDIOFILES:
			delete data.filePathList;
			break;
		case DropType::MIDIFILES:
			delete data.filePathList;
			break;
		case DropType::APPLICATION:
			break;
		case DropType::VOICE:
			break;
		case DropType::SAMPLE:
			break;
		case DropType::SCHEDULABLE:
			break;
		case DropType::INSTANCE:
			break;
		case DropType::PORT:
			break;
	}
	type = DropType::NOTHING;
	count = 0;
}

// this does the basic analysis of a qua drop event, either internally or externally initiated
bool
QuaDrop::SetTo(COleDataObject *object, DWORD ks)
{
	string	name;
	FORMATETC	dataFormat;
	STGMEDIUM	medium;

	Clear();

	type = DropType::UNKNOWN;
	keyState = ks;
	dropEffect = DROPEFFECT_MOVE;
	// check for a file drag/drop
	fprintf(stderr, "qua drop::set to()\n");
	if (object->GetData(CF_HDROP, &medium) && medium.tymed == TYMED_HGLOBAL) { 
		int cnt = DragQueryFile((HDROP)medium.hGlobal, 0xFFFFFFFF, 0, 0);
		type = DropType::FILES;
		data.filePathList = new vector<string>();
		count = 0;
		for (short i=0; i<cnt; i++) {
			int n = DragQueryFile((HDROP)medium.hGlobal, i, NULL, 0);
			char *buffer = new char[n];
			DragQueryFile((HDROP)medium.hGlobal, i, buffer, n);
			data.filePathList->push_back(buffer);
			delete buffer;
		}
		return true;
	} else if (object->IsDataAvailable(clipFormat)) {
		fprintf(stderr, "qua drop::clip\n");
		if (!object->GetData(clipFormat, &medium)) {
			fprintf(stderr, "cant get clip format data\n");
			return false;
		}
		if (medium.tymed != TYMED_HGLOBAL) {
			fprintf(stderr, "expected global data\n");
			return false;
		}
		type = DropType::CLIP;
		qua_symbol_clip *clip = (qua_symbol_clip *)GlobalLock(medium.hGlobal);
		data.symbol = clip->symbol;
		GlobalUnlock(medium.hGlobal);
		// ??? should GlobalFree
		fprintf(stderr, "qua drop::voice %s\n", data.symbol->name.c_str());
		return true;
	} else if (object->IsDataAvailable(instanceFormat)) {
		fprintf(stderr, "qua drop::instance\n");
		if (!object->GetData(instanceFormat, &medium)) {
			fprintf(stderr, "cant get instance format data\n");
			return false;
		}
		if (medium.tymed != TYMED_HGLOBAL) {
			fprintf(stderr, "expected global data\n");
			return false;
		}
		type = DropType::INSTANCE;
		qua_symbol_clip *clip = (qua_symbol_clip *)GlobalLock(medium.hGlobal);
		data.symbol = clip->symbol;
		GlobalUnlock(medium.hGlobal);
		// ??? should GlobalFree
		fprintf(stderr, "qua drop::voice %s\n", data.symbol->name.c_str());
		return true;
	} else if (object->IsDataAvailable(voiceFormat)) {
		fprintf(stderr, "qua drop::voice\n");
		if (!object->GetData(voiceFormat, &medium)) {
			fprintf(stderr, "cant get voice format data\n");
			return false;
		}
		if (medium.tymed != TYMED_HGLOBAL) {
			fprintf(stderr, "expected global data\n");
			return false;
		}
		type = DropType::VOICE;

		qua_symbol_clip *clip = (qua_symbol_clip *)GlobalLock(medium.hGlobal);
		data.symbol = clip->symbol;
		GlobalUnlock(medium.hGlobal);
		// ??? should GlobalFree
		fprintf(stderr, "qua drop::voice %s\n", data.symbol->name.c_str());
		return true;
	} else if (object->IsDataAvailable(sampleFormat)) {
		if (!object->GetData(sampleFormat, &medium)) {
			fprintf(stderr, "cant get sample format data\n");
			return false;
		}
		if (medium.tymed != TYMED_HGLOBAL) {
			fprintf(stderr, "expected global data\n");
			return false;
		}
		type = DropType::SAMPLE;

		qua_symbol_clip *clip = (qua_symbol_clip *)GlobalLock(medium.hGlobal);
		data.symbol = clip->symbol;
		GlobalUnlock(medium.hGlobal);
		// ??? should GlobalFree
		fprintf(stderr, "qua drop::sample %s\n", data.symbol->name.c_str());
		return true;
	} else if (object->IsDataAvailable(builtinFormat)) {
		if (!object->GetData(builtinFormat, &medium)) {
			fprintf(stderr, "cant get builtin format data\n");
			return false;
		}
		if (medium.tymed != TYMED_HGLOBAL) {
			fprintf(stderr, "expected global data\n");
			return false;
		}
		type = DropType::BUILTIN;

		qua_symbol_clip *clip = (qua_symbol_clip *)GlobalLock(medium.hGlobal);
		data.symbol = clip->symbol;
		GlobalUnlock(medium.hGlobal);
		// ??? should GlobalFree
		fprintf(stderr, "qua drop::builtin %s\n", data.symbol->name.c_str());
		return true;
	} else if (object->IsDataAvailable(vstpluginFormat)) {
		if (!object->GetData(vstpluginFormat, &medium)) {
			fprintf(stderr, "cant get vst plugin format data\n");
			return false;
		}
		if (medium.tymed != TYMED_HGLOBAL) {
			fprintf(stderr, "expected global data\n");
			return false;
		}
		type = DropType::VSTPLUGIN;

		qua_symbol_clip *clip = (qua_symbol_clip *)GlobalLock(medium.hGlobal);
		data.symbol = clip->symbol;
		GlobalUnlock(medium.hGlobal);
		// ??? should GlobalFree
		fprintf(stderr, "qua drop::audio plugin %s\n", data.symbol->name.c_str());
		return true;
	} else {
		// what have we got then .... it might well be a custom thing meant for a control
		// of which this is a child
		object->BeginEnumFormats();
		while (object->GetNextFormat(&dataFormat)) {
			switch (dataFormat.cfFormat) {
				case CF_TEXT:
					fprintf(stderr, "CF_TEXT\n");
					break;
				case CF_BITMAP:
					fprintf(stderr, "CF_BITMAP\n");
					break;
				case CF_METAFILEPICT:
					fprintf(stderr, "CF_METAFILEPICT\n");
					break;
				case CF_SYLK:
					fprintf(stderr, "CF_SYLK\n");
					break;
				case CF_DIF:
					fprintf(stderr, "CF_DIF\n");
					break;
				case CF_TIFF:
					fprintf(stderr, "CF_TIFF\n");
					break;
				case CF_OEMTEXT:
					fprintf(stderr, "CF_OEMTEXT\n");
					break;
				case CF_DIB:
					fprintf(stderr, "CF_DIB\n");
					break;
				case CF_PALETTE :
					fprintf(stderr, "CF_PALETTE\n");
					break;
				case CF_PENDATA:
					fprintf(stderr, "CF_PENDATA\n");
					break;
				case CF_RIFF:
					fprintf(stderr, "CF_RIFF\n");
					break;
				case CF_WAVE:
					fprintf(stderr, "CF_WAVE\n");
					break;
				case CF_UNICODETEXT:
					fprintf(stderr, "CF_UNICODETEXT\n");
					break;
				case CF_ENHMETAFILE:
					fprintf(stderr, "CF_ENHMETAFILE\n");
					break;
				case CF_HDROP: {
					fprintf(stderr, "CF_HDROP\n");
				}
				case CF_LOCALE:
					fprintf(stderr, "CF_LOCALE\n");
					break;
				case CF_DIBV5:
					fprintf(stderr, "CF_DIBV5\n");
					break;
				default:
					fprintf(stderr, "data format %d\n", dataFormat.cfFormat);
					break;
			}
			/*
			switch (dataFormat.tymed) {
				case TYMED_HGLOBAL:
					fprintf(stderr, "TYMED_HGLOBAL\n");
					break;
				case TYMED_FILE:
					fprintf(stderr, "TYMED_FILE\n");
					break;
				case TYMED_ISTREAM:
					fprintf(stderr, "TYMED_ISTREAM\n");
					break;
				case TYMED_ISTORAGE:
					fprintf(stderr, "TYMED_ISTORAGE\n");
					break;
				case TYMED_GDI:
					fprintf(stderr, "TYMED_GDI\n");
					break;
				case TYMED_MFPICT:
					fprintf(stderr, "TYMED_MFPICT\n");
					break;
				case TYMED_ENHMF:
					fprintf(stderr, "TYMED_ENHMF\n");
					break;
				case TYMED_NULL:
					fprintf(stderr, "TYMED_NULL\n");
					break;
				default:
					fprintf(stderr, "tymed %d\n", dataFormat.tymed);
					break;
			}
			*/
		}
	}
	return false;
}