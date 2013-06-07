#ifndef PLAT_CLCD_H
#define PLAT_CLCD_H

struct clcd_panel *cst_cb_clcd_get_panel(const char *);
int cst_cb_clcd_setup_dma(struct clcd_fb *, unsigned long);
int cst_cb_clcd_mmap_dma(struct clcd_fb *, struct vm_area_struct *);
void cst_cb_clcd_remove_dma(struct clcd_fb *);

#endif
