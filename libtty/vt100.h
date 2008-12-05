#ifndef _VT100_H
#define _VT100_H
#include <stdio.h>

typedef unsigned int ucs;

typedef struct 
{
    ucs ch;
    int attr;
} attrchar;

typedef unsigned short *vt100_charset;

#define VT100_MAXTOK 10

#define VT100_FLAG_RESIZABLE	0	/* should the input be allowed to resize? */
#define VT100_FLAG_CURSOR	1	/* visible cursor */
#define VT100_FLAG_KPAD		2	/* application keypad mode */
#define VT100_FLAG_AUTO_WRAP	3	/* auto wrap at right margin */

typedef struct vt100
{
    /*=[ basic data ]=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
    int sx,sy;	           /* screen size */
    int cx,cy;             /* cursor position */
    attrchar *scr;         /* screen buffer */
    int attr;              /* current attribute */
    /*=[ private stuff ]=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
    int s1,s2;             /* scrolling region */
    int save_cx,save_cy;   /* saved cursor position */
    vt100_charset G[2];    /* G0 and G1 charsets */
    vt100_charset transl;  /* current charset */
    int curG;              /* current charset as G# */
    /* UTF-8 state */
    int utf;               /* UTF-8 on/off */
    ucs utf_char;
    ucs utf_surrogate;
    int utf_count;
    /* parser state */
    int ntok;
    int tok[VT100_MAXTOK];
    int state;
    /* flags */
#define opt_allow_resize flags[VT100_FLAG_RESIZABLE] /* is input allowed to resize? */
#define opt_auto_wrap flags[VT100_FLAG_AUTO_WRAP] /* ?7: auto wrap at right margin */
#define opt_cursor flags[VT100_FLAG_CURSOR]	/* ?25: show/hide cursor */
#define opt_kpad flags[VT100_FLAG_KPAD]	/* keypad: application/numeric */
    int flags[10];
    /*=[ listeners ]=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
    void *l_data;
        /* any private data */
    void (*l_char)(struct vt100 *vt, int x, int y, ucs ch, int attr);
        /* after a character has been written to the screen */
    void (*l_cursor)(struct vt100 *vt, int x, int y);
        /* after the cursor has moved */
    void (*l_clear)(struct vt100 *vt, int x, int y, int len);
        /* after a chunk of screen has been cleared
           If an endpoint spills outside of the current line, it
           must go all the way to an end of screen.
           If the cursor moves, you'll get a separate l_cursor, although
           it is already in place during the l_clear call. */
    void (*l_scroll)(struct vt100 *vt, int nl);
        /* after the region s1<=y<s2 is scrolled nl lines
            * nl<0 -> scrolling backwards
            * nl>0 -> scrolling forwards
           The cursor is already moved.	*/
    void (*l_flag)(struct vt100 *vt, int f, int v);
        /* when a flag changes to v */
    void (*l_resize)(struct vt100 *vt, int sx, int sy);
        /* after the tty has been resized */
    void (*l_flush)(struct vt100 *vt);
        /* when a write chunk ends */
    void (*l_free)(struct vt100 *vt);
        /* before the tty is destroyed */
} *vt100;

#define VT100_ATTR_BOLD		0x010000
#define VT100_ATTR_DIM		0x020000
#define VT100_ATTR_ITALIC	0x040000
#define VT100_ATTR_UNDERLINE	0x080000
#define VT100_ATTR_BLINK	0x100000
#define VT100_ATTR_INVERSE	0x200000

vt100	vt100_init(int sx, int sy, int resizable, int utf);
int	vt100_resize(vt100 vt, int nsx, int nsy);
void	vt100_reset(vt100 vt);
void	vt100_free(vt100 vt);
void	vt100_write(vt100 vt, char *buf, int len);
void	vt100_printf(vt100 vt, const char *fmt, ...);
vt100	vt100_copy(vt100 vt);

void	vtvt_dump(vt100 vt);
void	vtvt_resize(vt100 vt, int sx, int sy);
void	vtvt_attach(vt100 vt, FILE *tty, int dump);

#endif
