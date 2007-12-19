/*
 * scr_clm.c
 *
 * Defines functions for initializing and displaying the column screen.
 * This also includes keyboard actions for the screen.
 *
 */

#include "scr_clm.h"

#include "globals.h"
#include "output_scr.h"
#include "input_kb.h"
#include "scr_actn.h"
#include "obj_slabs.h"
#include "obj_column.h"
#include "lev_data.h"

/*
 * Initializes variables for the column screen.
 */
short init_mdclm(void)
{
    return true;
}

/*
 * Deallocates memory for the column screen.
 */
void free_mdclm(void)
{
}


/*
 * Covers actions from the column screen.
 */
void actions_mdclm(int key)
{
    int sx, sy;
    sx = (mapmode->mapx+mapmode->screenx)*3+mapmode->subtl_x;
    sy = (mapmode->mapy+mapmode->screeny)*3+mapmode->subtl_y;
    message_release();
    
    if (!cursor_actions(key))
    if (!subtl_select_actions(key))
    {
      switch (key)
      {
        case KEY_TAB:
        case KEY_ESCAPE:
          scrmode->mode=MD_SLB;
          message_info("Returned to Slab mode.");
          break;
        case 'u': // Update all things/dat/clm/w?b
          update_slab_owners(lvl);
          update_datclm_for_whole_map(lvl);
          message_info("All DAT/CLM/W?B entries updated.");
          break;
        case 'a': // update dat/clm/w?b of selected tile
          //TODO: add 'enable autoupdate' function
          update_datclm_for_slab(lvl, sx/3,sy/3);
          message_info("Updated DAT/CLM/W?B entries of slab %d,%d.",sx/3,sy/3);
          break;
        case 'm': // manual-set mode
          //TODO: display slab selection screen
          message_info("Sorry, function unfinished.");
          break;
        default:
          message_info("Unrecognized clm key code: %d",key);
          speaker_beep();
      }
    }
}

/*
 * Action function - start the column mode.
 */
short start_mdclm()
{
    scrmode->mode=MD_CLM;
    return true;
}

/*
 * Action function - end the column mode.
 */
void end_mdclm()
{
    mapmode->panel_mode=PV_MODE;
}

int display_dat_subtiles(int scr_row, int scr_col,int ty,int tx)
{
    int i, k;
    int color;
    // Display .dat stuff for this tile
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    set_cursor_pos(scr_row, scr_col);
    scr_row+=2;
    screen_printf("%s",".dat entries");
    for (k=0; k<3; k++)
    {
        int sy=ty*3+k;
        for (i=0; i<3; i++)
        {
            int sx=tx*3+i;
            set_cursor_pos(scr_row, scr_col+i*5);
            
            if ((i==mapmode->subtl_x) && (k==mapmode->subtl_y) && (scrmode->mode==MD_CLM))
                color=PRINT_COLOR_BLACK_ON_LGREY;
            else
                color=PRINT_COLOR_LGREY_ON_BLACK;
            screen_setcolor(color);
            if (dat_view_mode==2)
                screen_printf("%4u",
                        (unsigned int)get_dat_subtile(lvl,sx,sy));
            else
                screen_printf("%02X%02X",
                        (unsigned int)(lvl->dat_high[sx][sy]),
                        (unsigned int)(lvl->dat_low[sx][sy]));
        }
        scr_row+=2;
    }
    return scr_row;
}

/*
 * Draws screen for the column mode.
 */
void draw_mdclm(void)
{
    draw_map_area(lvl,true,true,false);
    if (mapmode->panel_mode!=PV_MODE)
      draw_forced_panel(lvl,mapmode->panel_mode);
    else
      draw_mdclm_panel();
    draw_map_cursor(lvl,true,true,false);
}

void draw_mdclm_panel(void)
{
    int tx, ty;
    tx = mapmode->screenx+mapmode->mapx;
    ty = mapmode->screeny+mapmode->mapy;
    int sx, sy;
    sx = tx*3+mapmode->subtl_x;
    sy = ty*3+mapmode->subtl_y;
    int clm_idx=get_dat_subtile(lvl,sx,sy);
    char *clmentry;
    clmentry = lvl->clm[clm_idx%COLUMN_ENTRIES];
    struct COLUMN_REC *clm_rec;
    clm_rec=create_column_rec();
    get_clm_entry(clm_rec, clmentry);
    screen_setcolor(PRINT_COLOR_LGREY_ON_BLACK);
    int i;
    int k=0;
    int scr_col1=scrmode->cols+3;
    int scr_col2=scrmode->cols+23;

    set_cursor_pos(k++, scr_col1);
    screen_printf("Use: %04X (dec: %d)", clm_rec->use,clm_rec->use);
    set_cursor_pos(k, scr_col1);
    screen_printf("Lintel: %d", clm_rec->lintel);
    set_cursor_pos(k++, scr_col2);
    screen_printf("CLM index: %4d", clm_idx);
    set_cursor_pos(k, scr_col1);
    screen_printf("Solid mask: %04X", clm_rec->solid);
    set_cursor_pos(k++, scr_col2);
    screen_printf("Height: %X", clm_rec->height);
    set_cursor_pos(k, scr_col1);
    screen_printf("Base block: %03X", clm_rec->base);
    set_cursor_pos(k++, scr_col2);
    screen_printf("Permanent: %d", clm_rec->permanent);
    set_cursor_pos(k, scr_col2);
    screen_printf("Orientation: %02X", clm_rec->orientation);
    for (i=0; i < 8; i++)
    {
      set_cursor_pos(k++, scr_col1);
      screen_printf("Cube %d: %03X", i, clm_rec->c[i]);
    }
    free_column_rec(clm_rec);
    set_cursor_pos(k++, scr_col1);
    screen_printf_toeol("WIB animate: %03X", get_subtl_wib(lvl,sx,sy));
//    set_cursor_pos(k++, cols+3);
//    screen_printf("WLB tile info: %03X", get_subtl_wlb(lvl, tx, ty));

    display_tngdat();
}