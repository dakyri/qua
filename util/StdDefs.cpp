#include "StdDefs.h"

#include <stdio.h>

#include <iostream>
using namespace std;

char	*glibReplies[] = {
	"Yes, and?",
	"And you want me to tell my mum?",
	"Could you run that past me again?",
	"Could you drive that over me again?",
	"Did anybody get the number of that truck?",
	"Can't you see I'm busy dancing my brains to mulch?",
	"And I thought I'd heard everything!",
	"It does my head in",
	"I'll have none of that, then",
	"Yeah... whatever",
	"But that trick never works!",
	"Well, if that's what turns you on...",
	"Is that your best pickup line?",
	"Have you had too much of the fruit punch?",
	"Would you mind repeating that in ancient Greek?",
	"Would antacid help?",
	"I suppose you want me to apologise.",
	"I've got the T-shirt for that one.",
	"Maybe I'll leave that one for somebody else.",
	"Why me?",
	"Did you spike Toto's dog chow with that?",
	"Did I really need to know that?"
};


status_t
tab(FILE *fp, short in)
{
	for (short i=0; i<in; i++)
		fprintf(fp, "\t");
	return B_NO_ERROR;
}


long
WriteChunkId(FILE *fp,	uint32 dataId, uint32 dataLen)
{
	if (fwrite( &dataId, sizeof(dataId), 1, fp) != 1) {
		return B_ERROR;
	}
	if (fwrite( &dataLen, sizeof(dataLen), 1, fp) != 1) {
		return B_ERROR;
	}
	return B_NO_ERROR;
}

long
ReadChunkId(FILE *fp,	uint32 &dataId, uint32 &dataLen)
{
	if (fread( &dataId, sizeof(dataId), 1, fp) != 1) {
		return B_ERROR;
	}
	if (fread( &dataLen, sizeof(dataLen), 1, fp) != 1) {
		return B_ERROR;
	}
	return B_NO_ERROR;
}

void
memfill(void *buf, long len, uchar stuff)
{
	for (long i=0; i<len; i++) {
		((char *)buf)[i] = stuff;
	}
}

void
memswab(void *buf, long len)
{
	char *bp = (char *)buf;
	len /= 2;
	for (long i=0; i<len; i++) {
		char c = bp[0];
		bp[0] = bp[1];
		bp[1] = c;
		bp += 2;
	}
}



string errorStr(int no)
{
	string err("no error");

	switch (no) {
	case B_NO_ERROR: return "absence of error";
	case B_ERROR: return "generic error";

	case ERRNO_NO_MEMORY: return "not enough memory to do something";
	case ERRNO_IO_ERROR: return "general io error";
	case ERRNO_PERMISSION_DENIED: return "permission denied";
	case ERRNO_BAD_INDEX: return "bad index";
	case ERRNO_BAD_TYPE: return "bad type";
	case ERRNO_BAD_VALUE: return "bad value";
	case ERRNO_MISMATCHED_VALUES: return "mismatched values";
	case ERRNO_NAME_NOT_FOUND: return "name not found";
	case ERRNO_NAME_IN_USE: return "for named token creation functions";
	case ERRNO_TIMED_OUT: return "time-out limit expired";
	case ERRNO_INTERRUPTED: return "call was interrupted by a signal";
	case ERRNO_WOULD_BLOCK: return "call would have blocked";
	case ERRNO_CANCELED: return "canceled";
	case ERRNO_NO_INIT:	 return "no init";
	case ERRNO_BUSY:	 return "busy";
	case ERRNO_NOT_ALLOWED: return "not allowed";
		/*
		case ERRNO_BAD_SEM_ID: return "bad sem id";	// = ERRNO_OS_ERROR_BASE,
		case ERRNO_NO_MORE_SEMS: return "no more sems";

		case ERRNO_BAD_THREAD_ID: return "bad thread id"; // = ERRNO_OS_ERROR_BASE + 0x100,
		case ERRNO_NO_MORE_THREADS: return "no more threads";
		case ERRNO_BAD_THREAD_STATE:str = "bad thread state";
		case ERRNO_BAD_TEAM_ID: return "bad team id";
		case ERRNO_NO_MORE_TEAMS: return "no more teams";

		case ERRNO_BAD_PORT_ID: return "bad port id"; // = ERRNO_OS_ERROR_BASE + 0x200,
		case ERRNO_NO_MORE_PORTS: return "no more ports";

		case ERRNO_BAD_IMAGE_ID: return "nbad image id";	// = ERRNO_OS_ERROR_BASE + 0x300,
		case ERRNO_BAD_ADDRESS: return "bad address";
		case ERRNO_NOT_AN_EXECUTABLE: return "not an executable";
		case ERRNO_MISSING_LIBRARY: return "missing library";
		case ERRNO_MISSING_SYMBOL: return "missing  symbol";

		case ERRNO_DEBUGGER_ALREADY_INSTALLED: return "debugger installed"; // = ERRNO_OS_ERROR_BASE + 0x400

		case ERRNO_BAD_REPLY:	 return "bad reply"; // = ERRNO_APP_ERROR_BASE,
		case ERRNO_DUPLICATE_REPLY: return "duplicate reply";
		case ERRNO_MESSAGE_TO_SELF: return "message to self";
		case ERRNO_BAD_HANDLER: return "bad handler";
		case ERRNO_ALREADY_RUNNING: return "already running";
		case ERRNO_LAUNCH_FAILED: return "launch failed";
		case ERRNO_AMBIGUOUS_APP_LAUNCH: return "ambiguous app launch";
		case ERRNO_UNKNOWN_MIME_TYPE: return "unknown mime type";
		case ERRNO_BAD_SCRIPT_SYNTAX: return "bad script syntax";
		case ERRNO_LAUNCH_FAILED_NO_RESOLVE_LINK: return "launch fail: no resolve link";
		case ERRNO_LAUNCH_FAILED_EXECUTABLE: return "launch failed: bad executable";
		case ERRNO_LAUNCH_FAILED_APP_NOT_FOUND: return "launch failed: app not found";
		case ERRNO_LAUNCH_FAILED_APP_IN_TRASH: return "launch failed: app in trash";
		case ERRNO_LAUNCH_FAILED_NO_PREFERRED_APP: return "launch failed: no preferred app";
		case ERRNO_LAUNCH_FAILED_FILES_APP_NOT_FOUND: return "launch failed: app not found";

		case ERRNO_FILE_ERROR: return "general file error";	// ERRNO_STORAGE_ERROR_BASE,
		case ERRNO_FILE_NOT_FOUND: return "inobvious file";
		case ERRNO_FILE_EXISTS: return "file exists";
		case ERRNO_ENTRY_NOT_FOUND: return "inobvious entry";
		case ERRNO_NAME_TOO_LONG: return "name too long";
		case ERRNO_NOT_A_DIRECTORY: return "not a directory";
		case ERRNO_DIRECTORY_NOT_EMPTY: return "directory not empty";
		case ERRNO_DEVICE_FULL: return "device full";
		case ERRNO_READ_ONLY_DEVICE: return "read only device";
		case ERRNO_IS_A_DIRECTORY: return "is a directory";
		case ERRNO_NO_MORE_FDS: return "no more fd's";
		case ERRNO_CROSS_DEVICE_LINK: return "cross device link";
		case ERRNO_LINK_LIMIT: return "link limit";
		case ERRNO_BUSTED_PIPE: return "busted pipe";
		case ERRNO_UNSUPPORTED: return "unsupported";
		case ERRNO_PARTITION_TOO_SMALL: return "partition too small";

		case ERRNO_STREAM_NOT_FOUND: return "stream not found";
		case ERRNO_SERVER_NOT_FOUND: return "no server?";
		case ERRNO_RESOURCE_NOT_FOUND: return "resource not found";
		case ERRNO_BAD_SUBSCRIBER: return "bad subscriber";
		case ERRNO_SUBSCRIBER_NOT_ENTERED: return "subscriber not entered";
		case ERRNO_BUFFER_NOT_AVAILABLE: return "buffer not available";
		case ERRNO_LAST_BUFFER_ERROR: return "last buffer error";

		case ERRNO_MEDIA_SYSTEM_FAILURE: return "media kit, system failure"; //= (int)ERRNO_MEDIA_ERROR_BASE+0x100,	// 80004100
		case ERRNO_MEDIA_BAD_NODE: return "media kit, bad node";
		case ERRNO_MEDIA_NODE_BUSY: return "media kit, node busy";
		case ERRNO_MEDIA_BAD_FORMAT: return "media kit, bad format";
		case ERRNO_MEDIA_BAD_BUFFER: return "media kit, bad buffer";
		case ERRNO_MEDIA_TOO_MANY_NODES: return "media kit, too many nodes";
		case ERRNO_MEDIA_TOO_MANY_BUFFERS: return "media kit, too many buffers";
		case ERRNO_MEDIA_NODE_ALREADY_EXISTS: return "media kit, node already exists";
		case ERRNO_MEDIA_BUFFER_ALREADY_EXISTS: return "media kit, buffer already exists";
		case ERRNO_MEDIA_CANNOT_SEEK: return "media kit, cannot seek";
		case ERRNO_MEDIA_CANNOT_CHANGE_RUN_MODE: return "media kit, cannot change run mode";
		case ERRNO_MEDIA_APP_ALREADY_REGISTERED: return "media kit, already registered";
		case ERRNO_MEDIA_APP_NOT_REGISTERED: return "media kit, not registered";
		case ERRNO_MEDIA_CANNOT_RECLAIM_BUFFERS: return "media kit, can't reclaim buffer";
		case ERRNO_MEDIA_BUFFERS_NOT_RECLAIMED: return "media kit, buffer not reclaimed";
		case ERRNO_MEDIA_TIME_SOURCE_STOPPED: return "media kit, time source stopped";
		case ERRNO_MEDIA_TIME_SOURCE_BUSY: return "media kit, time source busy";					// 80004110
		case ERRNO_MEDIA_BAD_SOURCE: return "media kit, bad source";
		case ERRNO_MEDIA_BAD_DESTINATION: return "media kit, bad destination";
		case ERRNO_MEDIA_ALREADY_CONNECTED: return "media kit, already connected";
		case ERRNO_MEDIA_NOT_CONNECTED: return "media kit, not connected";
		case ERRNO_MEDIA_BAD_CLIP_FORMAT: return "media kit, bad clip format";
		case ERRNO_MEDIA_ADDON_FAILED: return "media kit, addon failed";
		case ERRNO_MEDIA_ADDON_DISABLED: return "media kit, addon disabled";
		case ERRNO_MEDIA_CHANGE_IN_PROGRESS: return "media kit, change in progress";
		case ERRNO_MEDIA_STALE_CHANGE_COUNT: return "media kit, stale change count";
		case ERRNO_MEDIA_ADDON_RESTRICTED: return "media kit, addon restricted";
		case ERRNO_MEDIA_NO_HANDLER: return "media kit, no handler";
		case ERRNO_MEDIA_DUPLICATE_FORMAT: return "media kit, duplicate format";
		case ERRNO_MEDIA_REALTIME_DISABLED: return "media kit, realtime disabled";
		case ERRNO_MEDIA_REALTIME_UNAVAILABLE: return "media kit, realtime unavailable";

		case ERRNO_NO_REPLY: return "absence of reply";

		case ERRNO_DEV_INVALID_IOCTL: return "device invalid ioctl"; // = ERRNO_DEVICE_ERROR_BASE,
		case ERRNO_DEV_NO_MEMORY: return "device memory error";
		case ERRNO_DEV_BAD_DRIVE_NUM: return "bad device drive number";
		case ERRNO_DEV_NO_MEDIA: return "no device media";
		case ERRNO_DEV_UNREADABLE: return "device unreadable";
		case ERRNO_DEV_FORMAT_ERROR: return "device format error";
		case ERRNO_DEV_TIMEOUT: return "device timeout";
		case ERRNO_DEV_RECALIBRATE_ERROR: return "device recalibration error";
		case ERRNO_DEV_SEEK_ERROR: return "device seek error";
		case ERRNO_DEV_ID_ERROR: return "device id error";
		case ERRNO_DEV_READ_ERROR: return "device read error";
		case ERRNO_DEV_WRITE_ERROR: return "device write error";
		case ERRNO_DEV_NOT_READY: return "device not ready";
		case ERRNO_DEV_MEDIA_CHANGED: return "device media changed";
		case ERRNO_DEV_MEDIA_CHANGE_REQUESTED: return "device media change req";
		case ERRNO_DEV_RESOURCE_CONFLICT: return "device resource conflict";
		case ERRNO_DEV_CONFIGURATION_ERROR: return "device config error";
		case ERRNO_DEV_DISABLED_BY_USER: return "device disabled";
		case ERRNO_DEV_DOOR_OPEN: return "device door open";
		default:
		if (no > ERRNO_DEVICE_ERROR_BASE)
		sprintf(buf, "device error 0x%x\n", no);
		else if (no > ERRNO_PRINT_ERROR_BASE)
		sprintf(buf, "print error 0x%x\n", no);
		else if (no > ERRNO_MAIL_ERROR_BASE)
		sprintf(buf, "mail error 0x%x\n", no);
		else if (no > ERRNO_POSIX_ERROR_BASE)
		sprintf(buf, "posix error 0x%x\n", no);
		else if (no > ERRNO_STORAGE_ERROR_BASE)
		sprintf(buf, "storage error 0x%x\n", no);
		else if (no > ERRNO_MIDI_ERROR_BASE)
		sprintf(buf, "midi error %d\n", no);
		else if (no > ERRNO_TRANSLATION_ERROR_BASE)
		sprintf(buf, "translation err 0x%x\n", no);
		else if (no > ERRNO_MEDIA_ERROR_BASE) {
		sprintf(buf, "media error 0x%x\n", no);
		}
		else if (no > ERRNO_INTERFACE_ERROR_BASE)
		sprintf(buf, "interface error 0x%x\n", no);
		else if (no > ERRNO_APP_ERROR_BASE)
		sprintf(buf, "app error 0x%x\n", no);
		else if (no > ERRNO_OS_ERROR_BASE)
		sprintf(buf, "os error 0x%x\n", no);
		else
		sprintf(buf, "unknown err 0x%d\n", no);
		return buf;
		*/
	}
	return err;
}

#include <stdarg.h>

void internalError(char *str, ...)
{
	char		buf[180];
	va_list		args;
	va_start(args, str);
	vsprintf(buf, str, args);
	fprintf(stderr, "Error in Baseval: %s\n", buf);
	va_end(args);
}


#include <sstream>
string wc2string(wchar_t  wa[]) {
	wchar_t *wp = wa;

	stringstream ss;
	while (*wp != L'\0') {
		ss << use_facet< ctype<wchar_t> >(locale()).narrow(*wp++, '_');
	}
	return ss.str();
}

string uintstr(uint32 x) {
	char *p = (char*)&x;
	string s;
	s.push_back(p[0]);
	s.push_back(p[1]);
	s.push_back(p[2]);
	s.push_back(p[3]);
	return s;
}

string normalizePath(const string &path) {
	string s(path);
	string::size_type i = 0;
	while ((i = s.find('\\', i)) != string::npos) {
		s[i] = '/';
	}
	return s;
}

string getLeaf(const string &path) {
	string s=normalizePath(path);
	auto it = s.rfind('/');
	if (it != string::npos) {
		s.erase(s.begin(), s.begin()+it+1);
	}
	return s;
}

string getParent(const string &path) {
	string s(path);
	auto it = s.rfind('/');
	if (it != string::npos && it != 0) {
		s.erase(s.begin()+it, s.end());
	}
	return s;
}
string getBase(const string &path) {
	string s = getLeaf(path);
	auto it = s.rfind('.');
	if (it != string::npos) {
		s.erase(s.begin()+it, s.end());
	} else {
	}
	return s;
}

string getExt(const string &path) {
	string s = getLeaf(path);
	auto it = s.rfind('.');
	cout << s << " .. " << it << endl;
	if (it != string::npos) {
		s.erase(s.begin(), s.begin() + it + 1);
	} else {
		s = "";
	}
	return s;
}

string getSupertype(const string &path) {
	string s(path);
	auto it = s.rfind('/');
	if (it != string::npos) {
		s.erase(s.begin() + it, s.end());
	}
	return s;
}
