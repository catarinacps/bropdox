#define MAXNAME 255
#define MAXFILES 65536

struct file_info {
    char name[MAXNAME];
    char extension[MAXNAME];
    char last_modified[MAXNAME];
    int size;
};

struct client {
    int devices[2];
    char userid[MAXNAME];
    struct file_info files[MAXFILES];
    int logged_in;
};