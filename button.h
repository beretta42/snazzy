

extern struct vmt_s button_vmt;

struct button_data {
    void (*clicked)(widget *w);
};
