/*
    GUILIB:  An example GUI framework library for use with SDL
    Copyright (C) 1997  Sam Lantinga

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    5635-34 Springhouse Dr.
    Pleasanton, CA 94588 (USA)
    slouken@devolution.com
*/

/* These are return codes for widget event functions */

#ifndef _GUI_status_h
#define _GUI_status_h

typedef enum {
	GUI_QUIT,			/* Close the GUI */
	GUI_REDRAW,			/* The GUI needs to be redrawn */
	GUI_YUM,			/* The event was eaten by the widget */
	GUI_PASS			/* The event should be passed on */
} GUI_status;

typedef enum {
	WIDGET_VISIBLE,
	WIDGET_HIDDEN,
	WIDGET_DELETED
} WIDGET_status;

/* GUI idle function -- run when no events pending */
typedef GUI_status (*GUI_IdleProc)(void);

#endif /* _GUI_status_h */
