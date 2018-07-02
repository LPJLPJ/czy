#include <stdlib.h>
#include <stdio.h>
#include "privateInclude.h"
#include "getopt.h"


struct globalArgs_t globalArgs;

static const char *optstring = "W:w:H:h:F:f:";

void GetGlobalArgs()
{
	char** argv = __argv;
	int    argc = __argc;
	int    opt  = 0;
	//≥ı ºªØ
	globalArgs.ScreenHeigth = 768;
	globalArgs.ScreenWidth = 1024;
	globalArgs.FileName = NULL;

	opt = getopt(argc,argv,optstring);
	while(opt!= -1)
	{
		switch(opt)
		{
		case'W':
		case'w':
			sscanf_s(optarg,"%d",&globalArgs.ScreenWidth);
			break;
		case'H':
		case'h':
			sscanf_s(optarg,"%d",&globalArgs.ScreenHeigth);
			break;
		case'F':
		case'f':
			globalArgs.FileName = optarg;
			break;
		default:  break;
		}
		opt = getopt(argc,argv,optstring);
	}
}