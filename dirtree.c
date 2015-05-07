#include "dirtree.h"
#include <avr/interrupt.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "os.h"

#define FWIDTH 6
#define FHEIGHT 9

uint8_t showing = 0; // start disabled
uint8_t ignoreNotShowing = 0;

// x1 x2 y1 y2
const rectangle top_left = {0, 2, 0, 2};
const rectangle middleness = {LCDHEIGHT / 2 - 1, LCDHEIGHT / 2 + 1, LCDWIDTH / 2 - 1, LCDWIDTH / 2 + 1};

#define HEADERROWS 1
#define COLS LCDHEIGHT / FWIDTH
#define ROWS LCDWIDTH / FHEIGHT - HEADERROWS
#define SCROLLDELTA 5

#define FALSE 0
#define TRUE 1

uint16_t dirtree_fg = WHITE;
uint16_t dirtree_bg = BLACK;

// all important selection event (takes back-slash delimitated file name)
void (*dirtree_select)(const char*);

void dirtree_set_select_func(void (*select_func)(const char*))
{
	dirtree_select = select_func;
}

// tree contruct
typedef struct treen
{
	struct treen* parent; // I love c
	struct treen* sister;
	struct treen* brother;
	struct treen* child;
	char* str;
	bool isdir;
} treen;

// top level thing
treen top_treen = {NULL, NULL, NULL, NULL, "\\", TRUE};
treen* sel_treen = &top_treen;
int sel_depth = 0;

// string rendering construct
typedef struct
{
	uint8_t old_inset; // in FWIDTHs
	uint8_t old_len; // in FWIDTHs
	uint8_t new_inset; // in FWIDTHs
	bool redraw;
	treen* ttt;
} drwstr;

// rendering state
drwstr drwstrs[ROWS];
uint8_t drwsel = 0;
int scroll_pos = 0;

void P(int i)
{
	int k=2,z=i;
	for(;(z/=10)>0;k++);
	
	char*b=malloc(k+1);
	itoa(i,b,10);
	b[k-1]=10;
	b[k]=0; // no faith
	display_string(b);
	free(b);
}

// doesn't bother de-linking, just frees
void prune_traverse(treen* cur)
{
	treen* tcur;
again:
	tcur = cur;

	if (tcur->child != NULL)
	{
		cur = tcur->child;
		tcur->child = NULL;
		// we get freed when we move onto our brother
		goto again;
	}
	else if (tcur->brother != NULL)
	{
		cur = tcur->brother;
		free(tcur->str);
		free(tcur);
		goto again;
	}
	else if (tcur->parent != NULL && tcur->parent->child != NULL) // reasons
	{
		cur = tcur->parent;
		free(tcur->str);
		free(tcur);
		goto again;
	}
	else
	{
		free(tcur->str);
		free(tcur);
		return;
	}
}

void prune(treen* parent)
{
	if (parent->child == NULL)
		return; // wasted trip

	treen* cur = parent->child;
	parent->child = NULL; // unlink
	cur->parent = NULL;

	prune_traverse(cur);
}

bool traverse_down(treen** cur, int* depth)
{
	treen* tcur = *cur;

	if (tcur->child != NULL)
	{
		*cur = tcur->child;
		*depth = *depth + 1;
		return TRUE;
	}
	
again:
	if (tcur->brother != NULL)
	{
		*cur = tcur->brother;
		return TRUE;
	}
	else if (tcur->parent != NULL) // reasons
	{
		tcur = tcur->parent;
		*depth = *depth - 1;
		goto again;
	}
	else
	{
		return FALSE;
	}
}

void traverse_down_right(treen** cur, int* depth)
{
	if ((*cur)->child != NULL)
	{
		*cur = (*cur)->child;
		*depth = *depth + 1;
		goto again;
	}
	return;

again:
	if ((*cur)->brother != NULL)
	{
		*cur = (*cur)->brother;
		goto again;
	}

	if ((*cur)->child != NULL)
	{
		*cur = (*cur)->child;
		*depth = *depth + 1;
		goto again;
	}
}

bool traverse_up(treen** cur, int* depth)
{
	if ((*cur)->sister != NULL)
	{
		*cur = (*cur)->sister;

		traverse_down_right(cur, depth);
	}
	else if ((*cur)->parent != NULL) // reasons
	{
		*cur = (*cur)->parent;
		//*depth--;
		*depth = *depth - 1;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void update_drwsel()
{
	uint8_t drwsel_old = drwsel;

	int i = ROWS;

	for (; i-->0 ;)
	{
		if (drwstrs[i].ttt == sel_treen)
		{
			drwsel = i;
			goto set;
		}
	}

	// didn't find one
	drwsel = 0;
	sel_treen = drwstrs[0].ttt;

set:
	drwstrs[drwsel_old].redraw = TRUE;
	drwstrs[drwsel].redraw = TRUE;
}

void bulk_rebuild()
{
	if (scroll_pos < 0)
		scroll_pos = 0;

	treen* cur = &top_treen;

	int depth = 0;
	int i = scroll_pos;
	for (; i-->0 ;)
	{
		if (!traverse_down(&cur, &depth))
		{
			scroll_pos -= i;
			break;
		}
	}

	bool clearing = FALSE;
	for (i = 0; i < ROWS; i++)
	{
		if (clearing)
		{
			drwstrs[i].new_inset = 0;
			drwstrs[i].ttt = NULL;
			drwstrs[i].redraw = TRUE;
		}
		else
		{
			drwstrs[i].new_inset = depth;
			drwstrs[i].ttt = cur;
			drwstrs[i].redraw = TRUE;
		}

		clearing = !traverse_down(&cur, &depth);
	}

	update_drwsel();
}

char* assemble_fname(treen* t)
{
	int slen = strlen(t->str);

	treen* cur = t;
	while (cur->parent != NULL)
	{
		cur = cur->parent;
		slen += strlen(cur->str) + 1;
	}

	char* res = malloc(slen+1);

	int i = slen;
	cur = t;
	while (cur != NULL)
	{
		res[i--] = '\\';

		int j = strlen(cur->str);
		for (; j-->0 ;)
		{
			res[i--] = cur->str[j];
		}

		cur = cur->parent;
	}
	
	res[slen] = 0; // do this after (replaces a \)
	return res;
}

//void iffy_startup_dirness()
//{
//	FATFS FatFs;
//	FIL File;  						/* FAT File */
//
//	f_mount(&FatFs, "", 0);
//		if (f_open(&File, "myfile.txt", FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {
//			//f_lseek(&File, f_size(&File));
//			//f_printf(&File, "Encoder position is: %d \r\n", position);
//			f_close(&File);
//		} else {
//		}
//}

void dirtree_repaint()
{
	int i = ROWS;

	for (; i-->0; )
	{
		drwstrs[i].redraw = TRUE;
	}
	
	display_string_xy("VoulanViewer (CyanMod)", FWIDTH * 6, 0);
}

void dirtree_init()
{
	int i = ROWS;

	for (; i-->0; )
	{
		drwstrs[i].old_inset = 0;
		drwstrs[i].old_len = 0;
		drwstrs[i].new_inset = 0;
		drwstrs[i].redraw = FALSE;
		drwstrs[i].ttt = NULL;
	}

	set_orientation(West); // TODO check if this can be removed

	bulk_rebuild();
	ignoreNotShowing = 1;
	dirtree_expand();
	ignoreNotShowing = 0;
}

void dirtree_show(uint8_t ns)
{
	if (ns)
	{
		if (showing) {
			return;
		}
		clear_screen();
		dirtree_repaint(); // invalidates everything

		// enable draw int
		EIMSK |= _BV(INT6);
	}
	else
	{
		// disable draw int
		EIMSK &= ~_BV(INT6);
	}

	showing = ns;
}

bool dirtree_showing()
{
	return showing;
}

void dirtree_move_up()
{
	if (!ignoreNotShowing && !showing)
		return;

	if (traverse_up(&sel_treen, &sel_depth))
	{
		if (drwsel <= 0)
		{
			scroll_pos -= SCROLLDELTA;
			bulk_rebuild();
		}
		else
		{
			update_drwsel();
		}
	}
}

void dirtree_move_down()
{
	if (!ignoreNotShowing && !showing)
		return;

	if (traverse_down(&sel_treen, &sel_depth))
	{
		if (drwsel >= ROWS - 1)
		{
			scroll_pos += SCROLLDELTA;
			bulk_rebuild();
		}
		else
		{
			update_drwsel();
		}
	}
}

void dirtree_expand()
{
	if (!ignoreNotShowing && !showing)
		return;

	if (sel_treen->isdir)
	{
		if (sel_treen->child == NULL)
		{
			char* fname = assemble_fname(sel_treen);


			FATFS FatFs;
			DIR dir;
			FRESULT mountres = f_mount(&FatFs, "", 1);
			if (mountres == FR_OK)
			{
				FRESULT dirres = f_opendir(&dir, fname);
				if (dirres == FR_OK)
				{
					treen* sister = NULL;

					FILINFO finf;
					while (f_readdir(&dir, &finf) == FR_OK)
					{
						if (dir.sect == 0)
							break;

						treen* temp = malloc(sizeof(treen));
						temp->parent = sel_treen;
						temp->sister = sister;
						temp->brother = NULL;
						temp->child = NULL;
						temp->isdir = finf.fattrib == AM_DIR ? TRUE : FALSE;
						int l = 13;
						temp->str = malloc(l + 1);
						temp->str[l] = 0;
						for (; l-->0 ;)
							temp->str[l] = finf.fname[l];

						if (sister != NULL)
							sister->brother = temp;
						else
							sel_treen->child = temp;
						sister = temp;
					}

					f_closedir(&dir);
				}
				else
				{
					display_string("Can't open dir! ");
					P(dirres);
				}
			}
			else
			{
				display_string("Can't mount card! ");
				P(mountres);
			}

			free(fname);

			bulk_rebuild();

			dirtree_move_down(); // need to do this afterwards, so that the drwstr exists
		}
		else
		{
			dirtree_move_down();
		}
	}
	else
	{
		// we iz file
		if (dirtree_select != NULL)
		{
			char* fname = assemble_fname(sel_treen);
			dirtree_select(fname);

			free(fname);
		}
	}
}

void dirtree_contract()
{
	if (!ignoreNotShowing && !showing)
		return;

	if (sel_treen->child != NULL)
	{
		prune(sel_treen);

		bulk_rebuild();
	}
	else if (sel_treen->parent != NULL)
	{
		sel_treen = sel_treen->parent;
		sel_depth--;

		if (drwsel <= 0)
		{
			scroll_pos -= SCROLLDELTA;
			bulk_rebuild();
		}
		else
		{
			update_drwsel();
		}
	}
}

/*void dirtree_move_up()
{
	if (drwsel >= 1)
	{
		drwsel--;
		drwstrs[drwsel+1].redraw = TRUE;
		drwstrs[drwsel].redraw = TRUE;
	}
}

void dirtree_move_down()
{
	if (drwsel < ROWS - 1)
	{
		drwsel++;
		drwstrs[drwsel-1].redraw = TRUE;
		drwstrs[drwsel].redraw = TRUE;
	}
}

void dirtree_expand()
{
	if (drwsel >= 0 && drwsel < ROWS)
	{
		drwstrs[drwsel].new_inset++;
		drwstrs[drwsel].redraw = TRUE;
	}
}

void dirtree_contract()
{
	if (drwsel >= 0 && drwsel < ROWS)
	{
		drwstrs[drwsel].new_inset--;
		drwstrs[drwsel].redraw = TRUE;
	}
}*/

void draw_drwstrs()
{
	char tstr[COLS + 1];

	int i = ROWS;
	for (; i-->0 ;)
	{
		if (drwstrs[i].redraw)
		{
			rectangle old_rect = {drwstrs[i].old_inset * FWIDTH, (drwstrs[i].old_inset + drwstrs[i].old_len) * FWIDTH, (i + HEADERROWS) * FHEIGHT, (i + HEADERROWS + 1) * FHEIGHT - 1};
			fill_rectangle(old_rect, dirtree_bg);

			if (drwsel == i)
			{ // invert
				display_color(dirtree_bg, dirtree_fg);
			}

			if (drwstrs[i].ttt != NULL)
			{
				int slen = strlen(drwstrs[i].ttt->str);
				int nx = drwstrs[i].new_inset * FWIDTH;
				int ny = (i + HEADERROWS) * FHEIGHT;
				if (drwstrs[i].new_inset + slen > COLS)
				{
					// inject ...
					slen = COLS-drwstrs[i].new_inset;

					tstr[slen] = 0;
					tstr[slen-1] = '.';
					tstr[slen-2] = '.';
					tstr[slen-3] = '.';

					int j = slen-3;
					for (; j-->0 ;)
						tstr[j] = drwstrs[i].ttt->str[j];

					display_string_xy(tstr, nx, ny);
				}
				else
				{
					// nothing fancy
					display_string_xy(drwstrs[i].ttt->str, nx, ny);
				}

				drwstrs[i].old_inset = drwstrs[i].new_inset;
				drwstrs[i].old_len = slen;
				drwstrs[i].redraw = FALSE;
			}
			else
			{
				// no ttt
				drwstrs[i].old_inset = 0;
				drwstrs[i].old_len = 0;
				drwstrs[i].redraw = FALSE;
			}

			if (drwsel == i)
			{ // reset
				display_color(dirtree_fg, dirtree_bg);
			}
		}
	}
}

/* render interrupt */
ISR(INT6_vect)
{
	// display.background
	// do NOT clear the screen in the interrupt

	// just in-case
	if (showing)
	{
		draw_drwstrs();
	}
	else // debugging
	{
	}
}
