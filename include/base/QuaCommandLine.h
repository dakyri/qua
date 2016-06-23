#pragma once

#include <vector>
#include <string>

using namespace std;

class QuaCommandLine
{
public:
	QuaCommandLine();

	int processCommandLine(int argc, char **argv);
	bool processCommandLineWord(long argno, string arg, bool cmd);

	void openListFile();
	void closeListFile();
	void listAsio(FILE *fp);
	void listMidi(FILE *fp);
	void listJoy(FILE *fp);
	void loadAsio(int n, FILE *fp);

	enum {
		SET_VST = 1,
		ADD_VST = 1 << 1,
		LOAD_ASIO = 1 << 2,
		LINT_QUA = 1 << 3
	};

	vector<string> vstList;
	vector<string> loadNames;
	vector<string> loaded;

	FILE *listFile;

	static char	 *usage_str;
	long last_command;
};

