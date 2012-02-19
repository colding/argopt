/*
 *  Copyright (C) 2003-2012 Jules Colding <jcolding@gmail.com>
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *
 *  You can use, modify and redistribute it in any way you want.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "argopt.h"

int 
main(int argc, char **argv)
{
	/* current argv index */
	int index = 0;

	/* options with arguments */
	int count;
	int offset;
	
	/* option flags */
	int opt_rw = 0;
	int opt_rb = 0;

	/* get options */
	while (1) {
		int c;
		char *parameter;
		struct option_t options[] = {
			{"rb", "-rb will help you rb-fy your source", NO_PARAM, NULL, 'r'},
			{"rw", "-rw is great to foobalooze the bazee", NO_PARAM, &opt_rw, 1},
			{"abc123", "-abc123 alphabetizes a lower set", NO_PARAM, NULL, 0},
			{"abc456", "-abc456 alphabetizes a higher set", NO_PARAM, NULL, 0},
			{"abc", NULL, NO_PARAM, NULL, 0},
			{"count", "-count <PARAMETER> enumerates the parameter", NEED_PARAM, NULL, 'c'},
			{"offset", "-offset [PARAMETER] set the offset for the optional paramater", OPTIONAL_PARAM, NULL, 'o'},
			{0, 0, 0, 0}
		};

		c = argopt(argc,
			   argv,
			   options,
			   &index,
			   &parameter);

		switch (c) {
		case ARGOPT_OPTION_FOUND :
			fprintf(stdout, "%d - Option found:\t\t%s (*flag is not NULL)\n", __LINE__, argv[index]);
			break;
		case ARGOPT_AMBIGIOUS_OPTION :
			argopt_completions(stdout,
					   "Ambigious option found. Possible completions:",
					   ++argv[index],
					   options);
			break;
		case ARGOPT_UNKNOWN_OPTION :
			fprintf(stdout, "%d - Unknown option found:\t%s\n", __LINE__, argv[index]);
			argopt_help(stdout, 
				    "Unknown option found",
				    argv[0],
				    options);
			break;
		case ARGOPT_NOT_OPTION :
			fprintf(stdout, "%d - Bad or malformed option found:\t%s\n", __LINE__, argv[index]);
			argopt_help(stdout, 
				    "Bad or malformed option found",
				    argv[0],
				    options);
			break;
		case ARGOPT_MISSING_PARAM :
			fprintf(stdout, "%d - Option missing parameter:\t%s\n", __LINE__, argv[index]);
			argopt_help(stdout, 
				    "Option missing parameter",
				    argv[0],
				    options);
			break;
		case ARGOPT_DONE :
			fprintf(stdout, "\nargv[] parsed.\n");
			goto opt_done;
		case 0 :
			fprintf(stdout, "%d - Option found:\t\t%s\n", __LINE__, argv[index]);
			break;
		case 'c' :
			count = strtol(parameter, NULL, 10);
			fprintf(stdout, "%d - Count option found:\t%s\n", __LINE__, argv[index-1]);
			fprintf(stdout, "%d - Count parameter is:\t\"%s\"\n", __LINE__, parameter);
			break;
		case 'o' :
			if (parameter) {
				fprintf(stdout, "%d - Offset option found:\t%s\n", __LINE__, argv[index-1]);
				fprintf(stdout, "%d - Offset parameter is:\t\"%s\"\n", __LINE__, parameter);
				offset = strtol(parameter, NULL, 16);
			} else {
				fprintf(stdout, "%d - Offset option found:\t%s\n", __LINE__, argv[index]);
			}
			break;
		case 'r' :
			fprintf(stdout, "%d - Option found:\t\t%s\n", __LINE__, argv[index]);
			opt_rb = 1;
			break;
		default:
			printf ("?? get_option() returned character code 0%o ??\n", c);
		}
		if (parameter)
			free(parameter);
		
		if (opt_rw) {
			fprintf(stdout, "-rw option found:\t%s\n", argv[index]);
			opt_rw = 0; /* purely cosmetic for this test progam */
		}
	}
opt_done:
	if ((index) && (index < argc)) {
		printf ("non-option ARGV-elements: ");
		while (index < argc)
			printf("%s ", argv[index++]);
		printf ("\n");

		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}


