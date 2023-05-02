//Nick Diaz

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <iomanip>
#include <cstring>
#include <map>

using namespace std;

void list(char[]);
void getPerms(char*);
void getSizeDate(char*);

int main(int argc, char *argv[])
{
	// allocate space for array to hold path
	long size = pathconf(".", _PC_PATH_MAX);
	char *path = (char*)malloc((size_t)size);
	// too many argumnets passed
	if(argc > 2)
	{
		cerr << "Given arguments not supported\n";
		return -1;
	}
	// directory specified
	else if(argc == 2)
	{
		path = argv[1];
		list(path);
	}
	// current working directory
	else
	{
		getcwd(path, (size_t)size);
		list(path);
	}
	return 0;
}

void list(char dirname[])
{
	// to pass to opendir()
	const char *path = dirname;
	struct dirent *direntp;
	DIR *dirp;

	// open given directory
	dirp = opendir(path);

	if(dirp == NULL)
	{
		fprintf(stderr, "myls: can't open directory \"%s\"\n", dirname);
		return;
	}

	while( (direntp = readdir(dirp)) != NULL )
	{
		// buffer to pass to getPerms; passing path will not work
		char buf[100];
		// add curr file to path; if this is not included and one were to run
		// "myls ..", the directories will print, but not their dir status and perms
		strcpy(buf, path);
		strcat(buf, "/");
		strcat(buf, direntp->d_name);

		// get the current file/directory permissions
		getPerms(buf);
		//print the current file/directory name
		cout << left << setw(30) << direntp->d_name;
		// get the current file/directory size and date last modified
		getSizeDate(buf);
		cout << endl;
	}
	closedir(dirp);
}

void getPerms(char *file)
{
	struct stat sb;

	if(stat(file, &sb) == 0)
	{
		mode_t perms = sb.st_mode;
		char *fileperms = (char*)malloc(pathconf(".", _PC_PATH_MAX));

		// is directory?
		fileperms[0] = (perms & S_IFDIR) ? 'T' : 'F';

		// owner perms
		fileperms[1] = (perms & S_IRUSR) ? 'r' : '-';
		fileperms[2] = (perms & S_IWUSR) ? 'w' : '-';
		fileperms[3] = (perms & S_IXUSR) ? 'x' : '-';

		// group perms
		fileperms[4] = (perms & S_IRGRP) ? 'r' : '-';
		fileperms[5] = (perms & S_IWGRP) ? 'w' : '-';
		fileperms[6] = (perms & S_IXGRP) ? 'x' : '-';

		// other users perms
		fileperms[7] = (perms & S_IROTH) ? 'r' : '-';
		fileperms[8] = (perms & S_IWOTH) ? 'w' : '-';
		fileperms[9] = (perms & S_IXOTH) ? 'x' : '-';
		// mark end
		fileperms[10] = '\0';

		string dirstatus =  (fileperms[0] == 'T') ? "DIR	" : "	";
		cout << dirstatus;
		for(int i = 1; i < strlen(fileperms); i++)
		{
			cout << fileperms[i];
		}
	}

	cout << "\t";
}

void getSizeDate(char* file)
{
	struct stat sb;
	struct tm *tm;
	int st = stat(file, &sb);
	tm = localtime(&sb.st_mtime);

	// map to hold days of the week
	map <int, string> day = {{0, "Sun"}, {1, "Mon"}, {2, "Tues"}, {3, "Wed"}, {4, "Thurs"}, {5, "Fri"}, {6, "Sat"} };
	// map to hold months
	map <int, string> month = {{0, "Jan"}, {1, "Feb"}, {2, "Mar"}, {3, "Apr"}, {4, "May"}, {5, "Jun"}, {6, "Jul"}, {7, "Aug"}, {8, "Sep"}, {9, "Oct"}, {10, "Nov"}, {11, "Dec"}};

	// to hold time (hours, min, sec)
	char time[1024];
	strftime(time, 1024, "%H:%M:%S", tm);

	if(stat(file, &sb) == 0)
	{
		cout << left << setw(10) <<  sb.st_size;
		cout << left << setw(5) << day[tm->tm_wday];
		cout << left << setw(5) << month[tm->tm_mon];
		cout << left << setw(5) << tm->tm_mday;
		cout << left << setw(5) << time;
		cout << left << setw(5) << tm->tm_year + 1900;
	}
}
