/*
 *    Copyright (C) 2003-2013, Jules Colding <jcolding@gmail.com>.
 *
 *    All Rights Reserved.
 */

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     (1) Redistributions of source code must retain the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer.
 *
 *     (2) Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *     (3) Neither the name of the copyright holder nor the names of
 *     its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "argopt.h"
#include <stdlib.h>
#include <string.h>

/*
 * Is it an option?
 */
static int
is_option(const char * const str)
{
        return ((str[0] == '-') && str[1]) ? 1 : 0;
}


/*
 * Push the specified arg to the end of argv
 */
static void
push_to_end(const int argc,
            char *str[],
            const int idx)
{
        int i;
        char *tmp = str[idx];

        for (i=idx; i < argc-1; i++)
                str[i] = str[i+1];

        str[argc-1] = tmp;
}


/*
 * Return matching number of characters in the beginning of two zero
 * terminated strings that match.
 */
static int
match_meassure(const char *str1,
               const char *str2)
{
        int retv = 0;

        if (!str1 || !str2)
                return 0;

        do {
                if (*str1 != *str2)
                        return retv;
                retv++;
        } while (*(++str1) && *(++str2));

        return retv;
}

/*
 * Find index of option 'opt' in option array.
 * Match unique abreviations or return ARGOPT_AMBIGIOUS_OPTION if the
 * result is ambiguous. Return ARGOPT_UNKNOWN_OPTION if not found.
 */
static int
find_option(const char *opt,
            const struct option_t * const options)
{
        int i = 0;
        int best = 0;
        int match = 0;
        int best_idx = 0;
        int min_match = strlen(++opt);
        char amb = 1;
        char found = 0;

        while (options[i].name) {
                if (!strcmp(opt, options[i].name))
                        return i;

                match = match_meassure(options[i].name, opt);
                if (match) {
                        if (match != min_match)
                                goto next;

                        found = 1;
                        if (match == best)
                                amb = 1;
                        else if (match > best) {
                                best_idx = i;
                                best = match;
                                amb = 0;
                        }
                }
        next:
                i++;
        }

        if (!found)
                return ARGOPT_UNKNOWN_OPTION;
        if (amb)
                return ARGOPT_AMBIGIOUS_OPTION;

        return best_idx;
}

/*
 * Make a duplicate of arg and return the pointer.
 * A parameter must not begin with a '-' unless
 * escaped by a '\'. You need to pass '\-arg' to the
 * command line to make the slash go pass the shell
 * expansion
 */
static char*
is_param(const char *param)
{
        if (param[0] == '-')
                return NULL;

        if ((param[0] == '\\') && (param[1] == '-'))
                param++;

        return strdup(param);
}

/*
 * Specify 'index = -1' to reset the internal static variable
 * 'argv_idx' and make ready for a new parse run.
 *
 * Return ARGOPT_AMBIGIOUS_OPTION if an option is ambiguous.
 * Return ARGOPT_UNKNOWN_OPTION if an unknown option is found.
 * Return ARGOPT_MISSING_PARAM if a required parameter is missing.
 * Return ARGOPT_OPTION_FOUND if an option has been found
 * Return ARGOPT_DONE when no more options remains in argv[]
 */
int
argopt(int argc,
       char *argv[],
       const struct option_t * const options,
       int *index,
       char **parameter) /* must be free()'ed if not NULL on return */
{
        static int argv_idx = 0;
        static int push_count = 0;
        int opt_index = 0;

        if (!options)
                return ARGOPT_DONE;

        /* reset counter */
        if (*index == -1) {
                argv_idx = 0;
                push_count = 0;
        }

        *parameter = NULL;
        if (++argv_idx < argc) {

                /* check for no remaining options */
                if (push_count + argv_idx >= argc) {
                        *index = argc - push_count;
                        return ARGOPT_DONE;
                }

                /* set index to point at current argv entry */
                *index = argv_idx;

                /* push non-options and unknow parameters to the end */
                if (!is_option(argv[argv_idx])) {

                        /* check for no remaining options */
                        if (push_count + argv_idx == argc)
                                return ARGOPT_DONE;

                        push_to_end(argc, argv, argv_idx);
                        push_count++;
                        argv_idx--;

                        *index = argc - 1;
                        return ARGOPT_NOT_OPTION;
                }

                /* find index in option_t array of options */
                opt_index = find_option(argv[argv_idx], options);
                if (opt_index < 0) {

                        /* push the erroneous option to the end */
                        push_to_end(argc, argv, argv_idx);
                        push_count++;
                        *index = argc - 1;

                        /* check for no remaining options */
                        if (push_count + argv_idx == argc) {
                                argv_idx--;
                                return opt_index;
                        }

                        /* push an eventual unknown parameter as well */
                        if (!is_option(argv[argv_idx])) {
                                push_to_end(argc, argv, argv_idx);
                                *index = argc - 2;
                                push_count++;
                        }
                        argv_idx--;

                        return opt_index;
                }
                *index = argv_idx;

                /* handle the parameter */
                switch (options[opt_index].param) {
                case NO_PARAM :
                        break;
                case NEED_PARAM :
                        if (push_count + argv_idx + 1 == argc)
                                return ARGOPT_MISSING_PARAM;

                        *parameter = is_param(argv[argv_idx+1]);
                        if (!*parameter)
                                return ARGOPT_MISSING_PARAM;
                        *index = ++argv_idx;

                        break;
                case OPTIONAL_PARAM :
                        if (push_count + argv_idx + 1 == argc)
                                break;

                        *parameter = is_param(argv[argv_idx+1]);
                        if (*parameter)
                                *index = ++argv_idx;

                        break;
                }

                /* get return value */
                if (options[opt_index].flag == NULL)
                        return options[opt_index].val;
                else {
                        *(options[opt_index].flag) = options[opt_index].val;
                        return ARGOPT_OPTION_FOUND;
                }
        }
        *index = argc - push_count;

        return ARGOPT_DONE;
}

/*
 * Will print the help text.
 */
void
argopt_help(FILE *output,
            const char * const notice,
            const char * const prog,
            const struct option_t * const options)
{
        size_t n = 0;
        size_t lmax = 0;
        const size_t space = 4;
        int opt_index = 0;

        if (!options)
                return;

        if (NULL == output)
                output = stdout;

        fprintf(output, "\n");
        if (notice)
                fprintf(output, "%s\n", notice);
        fprintf(output, "Usage: %s <OPTIONS>\n", prog);
        fprintf(output, "Options:\n");

        opt_index = 0;
        while (options[opt_index].name) {
                if (options[opt_index].help)
                        lmax = (strlen(options[opt_index].name) > lmax) ? strlen(options[opt_index].name) : lmax;
                opt_index++;
        }
        lmax += space;

        opt_index = 0;
        while (options[opt_index].name) {
                if (!options[opt_index].help) {
                        opt_index++;
                        continue;
                }

                fprintf(output, "   -%s", options[opt_index].name);
                for (n = strlen(options[opt_index].name); n < lmax; n++)
                        fprintf(output, " ");

                fprintf(output, "%s\n", options[opt_index].help);
                opt_index++;
        }
}

/*
 * List possible matches for ambigious option. "opt" is the option
 * without any leading '-'.
 */
void
argopt_completions(FILE *output,
                   const char * const notice,
                   const char * const opt,
                   const struct option_t * const options)
{
        int i = 0;
        int opt_count = 0;
        size_t n = 0;
        size_t lmax = 0;
        const int space = 4;
        size_t opt_len = 0;
        int *amb_array = NULL;

        if (!options || !opt)
                return;

        i = 0;
        while (options[i].name) {
                opt_count++;
                i++;
        }
        if (!opt_count)
                return;

        if (NULL == output)
                output = stdout;

        fprintf(output, "\n");
        if (notice)
                fprintf(output, "%s\n\n", notice);

        amb_array = (int*)malloc((opt_count + 1)*sizeof(char*));
        for (i = 0; i <= opt_count; i++)
                amb_array[i] = -1;

        n = 0;
        opt_len = strlen(opt);
        for (i = 0; i < opt_count; i++) {
                if (!strncmp(opt, options[i].name, opt_len)) {
                        amb_array[n++] = i;
                }
        }

        i = 0;
        for (i = 0; i < opt_count; i++) {
                if (-1 == amb_array[i])
                        break;
                if (options[amb_array[i]].help)
                        lmax = (strlen(options[amb_array[i]].name) > lmax) ? strlen(options[amb_array[i]].name) : lmax;
        }
        lmax += space;

        i = 0;
        for (i = 0; i < opt_count; i++) {
                if (-1 == amb_array[i])
                        break;

                fprintf(output, "   -%s", options[amb_array[i]].name);
                if (!options[amb_array[i]].help) {
                        fprintf(output, "\n");
                        continue;
                }
                for (n = strlen(options[amb_array[i]].name); n < lmax; n++)
                        fprintf(output, " ");

                fprintf(output, "%s\n", options[amb_array[i]].help);
        }

	free(amb_array);
}
