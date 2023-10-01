#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char buf[512];

int same(char *s, char *p) {
    int advance = 1; 
    if (*p == 0)
        return *s == 0;
    if (*p && *(p + 1) && *(p + 1) == '*') {
        if (same(s, p + 2))
            return 1;
        advance = 0;
    }
    if ((*s && *p == '.') || *s == *p)
        return same(s + 1, p + advance);
    return 0;
}

char *fmtname(char *path) {
    static char buf[DIRSIZ + 1];
    char *p;

    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), '\0', DIRSIZ - strlen(p));
    return buf;
}

void find(char *path, char *name) {
    char *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "error", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2,"error", path);
        close(fd);
        return;
    }

    if (st.type == T_FILE) {
        if (same(fmtname(path), name) != 0) {
            printf("%s\n", path);
        }
    } else if (st.type == T_DIR) {
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            printf("path is too long");
        } else {
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                find(buf, name);
            }
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    find(argv[1], argv[2]);
    exit(0);
}


