#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <taglib/tag_c.h>
#include <readline/readline.h>

#define TAGSID "taAcgyT"
const char *delim = " │";
#define PROMPT_SIZE (strlen(delim) + strlen("comment"))
#define BUFFER_SIZE 1024

void make_prompt(char *p, const char *n)
{
    for (unsigned int i = 0; i < PROMPT_SIZE - strlen(n) - strlen(delim); i++)
        p[i] = ' ';
    p[PROMPT_SIZE - strlen(n) - strlen(delim)] = '\0';
    strncat(p, n, strlen(n));
    for (unsigned int i = 0; i < strlen(delim); i++)
        p[PROMPT_SIZE - strlen(delim) + i] = delim[i];
    p[PROMPT_SIZE] = '\0';
}

char *rl_buffer;
int set_rl(void)
{
    rl_insert_text(rl_buffer);
    return 0;
}
void str_edit(const char *name, char *buf)
{
    char rlb[BUFFER_SIZE];
    strcpy(rlb, buf);

    char prompt[PROMPT_SIZE];
    make_prompt(prompt, name);

    rl_buffer       = rlb;
    rl_startup_hook = &set_rl;

    char *str;
    if ((str = readline(prompt)))
    {
        strncpy(buf, str, BUFFER_SIZE);
        free(str);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s file [%s]\n", argv[0], TAGSID);
        return 1;
    }
    char *tagslist = (argc == 2) ? TAGSID : argv[2];

    taglib_set_strings_unicode(1);
    TagLib_File *file = taglib_file_new(argv[1]);
    if (file == NULL)
    {
        fprintf(stderr, "cannot read tags in file «%s»\n", argv[1]);
        return 1;
    }
    TagLib_Tag *tag = taglib_file_tag(file);

#define EDIT_TAG(name)\
    {\
        char *buf = malloc(BUFFER_SIZE);\
        strncpy(buf, taglib_tag_##name(tag), BUFFER_SIZE);\
        str_edit(#name, buf);\
        taglib_tag_set_##name(tag, buf);\
        free(buf);\
    }
#define EDIT_TAG_INT(name)\
    {\
        char *buf = malloc(BUFFER_SIZE);\
        snprintf(buf, BUFFER_SIZE, "%u", taglib_tag_##name(tag));\
        str_edit(#name, buf);\
        errno = 0;\
        unsigned int res = strtoul(buf, NULL, 10);\
        taglib_tag_set_##name(tag, errno == 0 ? res : 0);\
        free(buf);\
    }

    char *p = malloc(PROMPT_SIZE);
    make_prompt(p, "file");
    printf("%s%s\n", p, argv[1]);
    free(p);
    for (char *c = tagslist; *c != '\0'; c++)
        switch (*c)
        {
        case 't': EDIT_TAG(title);     break;
        case 'a': EDIT_TAG(artist);    break;
        case 'A': EDIT_TAG(album);     break;
        case 'c': EDIT_TAG(comment);   break;
        case 'g': EDIT_TAG(genre);     break;
        case 'y': EDIT_TAG_INT(year);  break;
        case 'T': EDIT_TAG_INT(track); break;
        default :
                  fprintf(stderr, "Warning: tag has to be in [%s]. Skipping «%c».\n", TAGSID, c[0]);
        }

    taglib_file_save(file);
    taglib_tag_free_strings();
    taglib_file_free(file);
    return 0;
}
