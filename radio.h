
extern struct vmt_s radio_vmt;

struct radio_data {
    unsigned char state;
    widget *sel;            /* currently selected */
    widget *group;
};
