/*
 Author: José Bollo <jobol@nonadev.net>
 Author: José Bollo <jose.bollo@iot.bzh>

 https://gitlab.com/jobol/mustach

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#ifdef _WIN32
    #include <stdio.h>

    #define STRNDUPA(s, len, dest) { \
        int len2 = s ? strlen(s) : 0; \
        if (len2 > len) \
            len2 = len; \
        dest = (char *)alloca(len2 + 1);\
        memcpy(dest, s, len2);\
        dest[len2] = 0;\
    }
#else
    #define STRNDUPA(s, len, dest) dest = strndupa(s, len);
    #define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "mustach.h"

#define NAME_LENGTH_MAX   1024
#define DEPTH_MAX         256

struct MUST_FILE {
    char *buffer;
    size_t size;
    size_t pos;
};

static int template_write(const char *buf, size_t size, struct MUST_FILE *file) {
    size_t new_size;
    if ((!size) || (!buf))
        return 0;

    if (file->pos + size + 1 > file->size) {
        new_size = ((file->pos + size + 1) / 0x10000 + 1) * 0x10000;
        file->buffer = realloc(file->buffer, new_size);
    }

    if (file->buffer) {
        memcpy(file->buffer + file->pos, buf, size);
        file->pos += size;
        file->buffer[file->pos] = 0;
    }
    return size;
}

static int getpartial(struct mustach_itf *itf, void *closure, const char *name, char **result)
{
	*result = itf->put(closure, name, 0);
	return 0;
}

static int process(const char *tpl, struct mustach_itf *itf, void *closure, struct MUST_FILE *file, char *opstr, char *clstr)
{
	char name[NAME_LENGTH_MAX + 1], *partial, c;
	const char *beg, *term;
    char *temp;
	struct { const char *name, *again; size_t length; int emit, entered; } stack[DEPTH_MAX];
	size_t oplen, cllen, len, l;
	int depth, rc, emit;

	emit = 1;
	oplen = strlen(opstr);
	cllen = strlen(clstr);
	depth = 0;
	for(;;) {
		beg = strstr(tpl, opstr);
		if (beg == NULL) {
			/* no more mustach */
			if (emit)
				template_write(tpl, strlen(tpl), file);
			return depth ? MUSTACH_ERROR_UNEXPECTED_END : 0;
		}
		if (emit)
			template_write(tpl, (size_t)(beg - tpl), file);
		beg += oplen;
		term = strstr(beg, clstr);
		if (term == NULL)
			return MUSTACH_ERROR_UNEXPECTED_END;

		tpl = term + cllen;
		len = (size_t)(term - beg);
		c = *beg;
		switch(c) {
		case '!':
		case '=':
			break;
		case '{':
			for (l = 0 ; clstr[l] == '}' ; l++);
			if (clstr[l]) {
				if (!len || beg[len-1] != '}')
					return MUSTACH_ERROR_BAD_UNESCAPE_TAG;
				len--;
			} else {
				if (term[l] != '}')
					return MUSTACH_ERROR_BAD_UNESCAPE_TAG;
				tpl++;
			}
			c = '&';
			/*@fallthrough@*/
		case '^':
		case '#':
		case '/':
		case '&':
		case '>':
#if !defined(NO_EXTENSION_FOR_MUSTACH) && !defined(NO_COLON_EXTENSION_FOR_MUSTACH)
		case ':':
#endif
			beg++; len--;
		default:
			while (len && isspace(beg[0])) { beg++; len--; }
			while (len && isspace(beg[len-1])) len--;
			if (len == 0)
				return MUSTACH_ERROR_EMPTY_TAG;
			if (len > NAME_LENGTH_MAX)
				return MUSTACH_ERROR_TAG_TOO_LONG;
			memcpy(name, beg, len);
			name[len] = 0;
			break;
		}
		switch(c) {
		case '!':
			/* comment */
			/* nothing to do */
			break;
		case '=':
			/* defines separators */
			if (len < 5 || beg[len - 1] != '=')
				return MUSTACH_ERROR_BAD_SEPARATORS;
			beg++;
			len -= 2;
			for (l = 0; l < len && !isspace(beg[l]) ; l++);
			if (l == len)
				return MUSTACH_ERROR_BAD_SEPARATORS;
			STRNDUPA(beg, l, opstr);
			while (l < len && isspace(beg[l])) l++;
			if (l == len)
				return MUSTACH_ERROR_BAD_SEPARATORS;
			STRNDUPA((beg + l), (len - l), clstr);
			oplen = strlen(opstr);
			cllen = strlen(clstr);
			break;
		case '^':
		case '#':
			/* begin section */
			if (depth == DEPTH_MAX)
				return MUSTACH_ERROR_TOO_DEPTH;
			rc = emit;
			if (rc) {
				rc = itf->enter(closure, name);
				if (rc < 0)
					return rc;
			}
			stack[depth].name = beg;
			stack[depth].again = tpl;
			stack[depth].length = len;
			stack[depth].emit = emit;
			stack[depth].entered = rc;
			if ((c == '#') == (rc == 0))
				emit = 0;
			depth++;
			break;
		case '/':
			/* end section */
			if (depth-- == 0 || len != stack[depth].length || memcmp(stack[depth].name, name, len))
				return MUSTACH_ERROR_CLOSING;
			rc = emit && stack[depth].entered ? itf->next(closure) : 0;
			if (rc < 0)
				return rc;
			if (rc) {
				tpl = stack[depth++].again;
			} else {
				emit = stack[depth].emit;
				if (emit && stack[depth].entered)
					itf->leave(closure);
			}
			break;
		case '>':
			/* partials */
			if (emit) {
				rc = getpartial(itf, closure, name, &partial);
				if (rc == 0) {
					rc = process(partial, itf, closure, file, opstr, clstr);
				}
				if (rc < 0)
					return rc;
			}
			break;
		default:
			/* replacement */
			if (emit) {
                temp = itf->put(closure, name, c != '&');
                if (temp) {
                    template_write(temp, strlen(temp), file);
                } else
                    return MUSTACH_ERROR_UNDEFINED;
			}
			break;
		}
	}
}

int fmustach(const char *tpl, struct mustach_itf *itf, void *closure, struct MUST_FILE *file)
{
	int rc = itf->start ? itf->start(closure) : 0;
	if (rc == 0)
		rc = process(tpl, itf, closure, file, "{{", "}}");
	return rc;
}

int mustach(const char *tpl, struct mustach_itf *itf, void *closure, char **result, size_t *size)
{
	int rc;
    struct MUST_FILE file = {NULL, 0, 0};
	size_t s;

	*result = NULL;
	if (size == NULL)
		size = &s;
	rc = fmustach(tpl, itf, closure, &file);
	if (rc >= 0) {
		*size = file.size;
        *result = file.buffer;
    } else {
		free(file.buffer);
		*result = NULL;
		*size = 0;
	}
	return rc;
}

