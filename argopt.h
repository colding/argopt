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
