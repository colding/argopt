/*
 *  argopt, a library to pass command line options.
 *  Copyright (C) 2003-2012 Jules Colding <jcolding@gmail.com>
 *
 *  This file is part of argopt.
 *
 *  argopt is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or (at your option) any later version.
 *
 *  argopt is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General
 *  Public License along with argopt. If not,
 *  see <http://www.gnu.org/licenses/>.
 */

#ifndef __ARGOPT_H__
#define __ARGOPT_H__

#include <stdio.h>

#ifdef __cplusplus
#  define BEGIN_C_DECLS extern "C" {
#  define END_C_DECLS   }
#else /* !__cplusplus */
#  define BEGIN_C_DECLS
#  define END_C_DECLS
#endif /* __cplusplus */

BEGIN_C_DECLS

#define ARGOPT_OPTION_FOUND (-1)
#define ARGOPT_AMBIGIOUS_OPTION (-2)
#define ARGOPT_UNKNOWN_OPTION (-3)
#define ARGOPT_MISSING_PARAM (-4)
#define ARGOPT_NOT_OPTION (-5)
#define ARGOPT_DONE (-6)

enum need_param_t {
        NO_PARAM,
        NEED_PARAM,
        OPTIONAL_PARAM
};

struct option_t {
        const char *name;
        const char *help;
        const enum need_param_t param;
        int *flag;
        int val;
};

extern int
argopt(int argc,
       char *argv[],
       const struct option_t * const options,
       int *index,
       char **parameter);

extern void
argopt_help(FILE *output,
            const char * const notice,
            const char * const prog,
            const struct option_t * const options);

extern void
argopt_completions(FILE *output,
                   const char * const notice,
                   const char * const opt,
                   const struct option_t * const options);

END_C_DECLS

#endif /* __ARGOPT_H__ */
