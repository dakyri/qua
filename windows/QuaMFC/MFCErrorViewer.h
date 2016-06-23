#ifndef _MFC_ERR_VIEWR
#define _MFC_ERR_VIEWR

#define ERRV_ALLOC_INC	256

#include <string>
#include <vector>
using namespace std;

class MFCErrorViewer
{
public:
	MFCErrorViewer();
	~MFCErrorViewer();

	void		AddLine(string line);
	void		Clear();
	void		Show();

	vector<string> err_lines;
	long		err_chars;
};

#endif