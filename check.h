
extern struct vmt_s check_vmt;

struct check_data {
    unsigned char state;
    void (*clicked)(widget *w, unsigned char state);
};
