#ifndef _QUA_MESSID_H

// header for message id's for qua mfc interface.
#define	QM_VAL_CHANGED		(WM_USER+0x0000)
#define	QM_CTL_CHANGED		(WM_USER+0x0001)	// WPARAM = an id for the ctl to parent, LPARAM = ptr to ctl
#define	QM_CTL_BEGIN_MOVE	(WM_USER+0x0002)	// WPARAM = an id for the ctl to parent, LPARAM = ptr to ctl
#define	QM_CTL_END_MOVE		(WM_USER+0x0003)	// WPARAM = an id for the ctl to parent, LPARAM = ptr to ctl
#define QM_SYM_CTL_CHANGED	(WM_USER+0x0004)	// WPARAM = a pointer to a symbol, LPARAM = ptr to ctl
#define QM_SYM_CTL_BEGIN_MOVE	(WM_USER+0x0005)	// WPARAM = a pointer to a symbol, LPARAM = ptr to ctl
#define QM_SYM_CTL_END_MOVE	(WM_USER+0x0006)	// WPARAM = a pointer to a symbol, LPARAM = ptr to ctl
#define QM_BLOCK_COMPILE	(WM_USER+0x0007)	// WPARAM = an id for the ctl to parent, LPARAM = ptr to ctl
#define QM_BLOCK_PARSE		(WM_USER+0x0008)	// WPARAM = an id for the ctl to parent, LPARAM = ptr to ctl
#define QM_BLOCK_REVERT		(WM_USER+0x0009)	// WPARAM = an id for the ctl to parent, LPARAM = ptr to ctl
#define QM_ARRANGE_VIEW		(WM_USER+0x000a)
#define QM_CLOSE_VIEW		(WM_USER+0x000b)	// WPARAM = id for the ctl to close, LPARAM = pointer to the window
#define QM_OPEN_VIEW		(WM_USER+0x000c)	// WPARAM = id for the ctl to open, LPARAM = pointer to the window
#define QM_MINIMIZE_VIEW	(WM_USER+0x000d)	// WPARAM = id for the ctl to minimize, LPARAM = pointer to the window
#define QM_MAXIMIZE_VIEW	(WM_USER+0x000e)	// WPARAM = id for the ctl to maximize, LPARAM = pointer to the window
#define QM_OPEN_EXT_ED		(WM_USER+0x000f)	// WPARAM = id for the ctl to maximize, LPARAM = pointer to the window
#define QM_CLOSE_EXT_ED		(WM_USER+0x0010)	// WPARAM = id for the ctl to maximize, LPARAM = pointer to the window
#define QM_EXT_ED_CLOSED	(WM_USER+0x0011)	// WPARAM = id for the ctl to maximize, LPARAM = pointer to the window

#endif