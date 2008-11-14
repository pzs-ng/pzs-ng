#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "../conf/zsconfig.h"
#include "zsfunctions.h"
#include "audiosort.h"

int main(int argc, char *argv[])
{
	char targetDir[PATH_MAX];
	char cwdDir[PATH_MAX];

#if ( program_uid > 0 )
	setegid(program_gid);
	seteuid(program_uid);
#endif

	umask(0666 & 000);

	if (argc > 1)
	{
		if (argv[1][strlen(argv[1])-1] == '/')
			argv[1][strlen(argv[1])-1] = '\0';

		if (*argv[1] == '/')
		{
			snprintf(targetDir, PATH_MAX, "%s", argv[1]);
			d_log("audioSort: using argv[1].\n");
		}
		else
		{
			if (!getcwd(cwdDir, PATH_MAX)) {
				d_log("audioSort: Failed to getcwd: %s\n", strerror(errno));
			}
#if (debug_announce)
			printf("debug info: argv[1] = '%s'\n", argv[1]);
			printf("debug info: PWD = '%s'\n", getenv("PWD"));
			printf("debug info: getcwd() = '%s'\n", cwdDir);
#else
			d_log("audioSort-bin: argv[1] = '%s'\n", argv[1]);
			d_log("audioSort-bin: PWD = '%s'\n", getenv("PWD"));
			d_log("audioSort-bin: getcwd() = '%s'\n", cwdDir);
#endif
			if (getenv("PWD") != NULL) {
				snprintf(targetDir, PATH_MAX, "%s/%s", getenv("PWD"), argv[1]);
				d_log("audioSort: using PWD + argv[1].\n");
			} else {
				d_log("audioSort: using getcwd() + argv[1].\n");
				if (getcwd(cwdDir, PATH_MAX) == NULL) {
					d_log("audioSort: could not get current working dir: %s\n", strerror(errno));
					printf("Something bad happened when trying to decide what dir to resort.\n");
					return 1;
				}
				snprintf(targetDir, PATH_MAX, "%s/%s", cwdDir, argv[1]);
			}
		}
	}
	else
	{
		if (getenv("PWD") != NULL)
		{
			snprintf(targetDir, PATH_MAX, "%s", getenv("PWD"));
			d_log("audioSort: using PWD.\n");
		}
		else
		{
			d_log("audioSort: using getcwd().\n");
			if (getcwd(targetDir, PATH_MAX) == NULL)
			{
				d_log("audioSort: could not get current working dir: %s\n", strerror(errno));
				printf("Something bad happened when trying to decide what dir to resort.\n");
				return 1;
			}
		}
	}
	
	d_log("audioSort: resorting dir %s.\n", targetDir);

	printf("Resorting %s. :-)\n", targetDir);
	audioSortDir(targetDir);
	printf("Done!\n");

	return 0;
}

