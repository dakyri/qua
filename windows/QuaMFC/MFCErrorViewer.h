#ifndef _MFC_ERR_VIEWR
#define _MFC_ERR_VIEWR

#include "DaList.h"

#define ERRV_ALLOC_INC	256

class MFCErrorViewer
{
public:
	MFCErrorViewer();
	~MFCErrorViewer();

	void		AddLine(char *line);
	void		Clear();
	void		Show();

	BList		err_lines;
	long		err_chars;
};

#endif