/************* cd_ls_pwd.c file **************/
#include "cd_ls_pwd.h"
#include "globals.h"

static char mode_buffer[128];
static char *t1 = "xwrxwrxwr-------"; // modes
static char *t2 = "----------------"; // dashes

static void get_mode_string(int mode)
{
        for (int i = 8; i >= 0; i--) {
                if (mode & (1 << i)) // print r|w|x
                        printf("%c", t1[i]);
                else
                        printf("%c", t2[i]); // or print -
        }
}

int chdir(char *pathname)
{
        printf("chdir %s\n", pathname);
        int ino = getino(pathname);
        if (ino == 0) {
                printf("chdir: path does not exist.\n");
                return -1;
        }

        MINODE *mip = iget(dev, ino);
        if (is_dir(&mip->INODE) == 0) {
                printf("chdir: not a directory.\n");
                return -1;
        }

        iput(running->cwd);

        running->cwd = mip;
        return 0;
}

int ls_file(MINODE *mip, char *name)
{
        const long atime = mip->INODE.i_atime;
        char *time = ctime(&atime);
        time[strlen(time) - 1] = '\0';
        get_mode_string(mip->INODE.i_mode);
        printf("%s %d %d %s %s \n", mode_buffer, mip->INODE.i_links_count, mip->INODE.i_size, time, name);
        return 0;
}

int ls_dir(MINODE *mip)
{
        printf("ls_dir: list CWD's file names; YOU do it for ls -l\n");

        char buf[BLKSIZE], temp[256];
        DIR *dp;
        char *cp;
        MINODE *dir_mip = NULL;

        // Assume DIR has only one data block i_block[0]
        for (int i = 0; i < 12; i++) {
                if (mip->INODE.i_block[i] == 0)
                        continue;
                get_block(dev, mip->INODE.i_block[i], buf);
                dp = (DIR *)buf;
                cp = buf;

                while (cp < buf + BLKSIZE) {
                        memset(temp, 0, 256);
                        strncpy(temp, dp->name, dp->name_len);
                        temp[dp->name_len] = '\0';

                        // printf("[%d %s]  ", dp->inode, temp); // print [inode# name]
                        dir_mip = iget(dev, dp->inode);
                        ls_file(dir_mip, temp);

                        iput(dir_mip);
                        dir_mip = NULL;

                        cp += dp->rec_len;
                        dp = (DIR *)cp;
                }
        }
        printf("\n");
        return 0;
}

int ls(char *pathname)
{
        int ino = 0;
        printf("ls %s\n", pathname);
        printf("ls CWD only! YOU do it for ANY pathname\n");
        if (strcmp(pathname, "") == 0)
                ino = running->cwd->ino;
        else
                ino = getino(pathname);
        MINODE *mip = iget(dev, ino);
        ls_dir(iget(dev, ino));
        iput(mip);
        return 0;
}

int pwd(MINODE *wd)
{
        if (wd->ino == root->ino) {
                printf("/\n");
        } else {
                char buffer[256];
                rpwd(wd, buffer);
                tokenize(buffer);
                printf("/");
                for (int i = nname - 1; i >= 0; i--) {
                        printf("%s", name[i]);
                        printf("/");
                }
                printf("\n");
        }
        return 0;
}

int rpwd(MINODE *wd, char *buffer)
{
        char buf[BLKSIZE];
        DIR *temp;
        char dirname[BLKSIZE];
        u32 my_ino = 0;
        int parent = 0;
        MINODE *pip;
        get_block(wd->dev, wd->INODE.i_block[0], buf);
        temp = (DIR *)buf;
        char *cp = buf;
        if(wd->ino == root->ino){
                return 0;
        }
        while (cp < buf + BLKSIZE) {
                if (strcmp(temp->name, "..") == 0) {
                        parent = temp->inode;
                        break;
                }
                if (strcmp(temp->name, ".") == 0) {
                        my_ino = temp->inode;
                }
                cp = temp->rec_len + (char *)cp; // advancing cp
                temp = (DIR *)cp;
        }
        pip = iget(dev, parent);
        get_block(dev, pip->INODE.i_block[0], buf);
        temp = (DIR *)buf; // storing temp into buff
        cp = buf;
        while (cp < buf + BLKSIZE) {
                strcpy(dirname, temp->name);
                dirname[temp->name_len] = 0;
                if (my_ino == temp->inode) {
                        if (strcmp(dirname, "/") != 0)
                                strcat(buffer, "/");
                        strcat(buffer, dirname);
                        break;
                }
                cp = (char *)cp + temp->rec_len; // advancing cp
                temp = (DIR *)cp;
        }
        return rpwd(pip, buffer);

}

int is_dir(INODE *ino)
{
        int mode = ino->i_mode;
        mode = mode >> 12; // Shift 12 bits right.
        if (mode == DIR_MODE_INO) {
                return 1;
        }
        return 0;
}
