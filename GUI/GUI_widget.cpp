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

#include "nuvieDefs.h"
#include "GUI.h"
#include "GUI_widget.h"


/* Widget constructors */
GUI_Widget:: GUI_Widget(void *data)
{
	Init(data, 0, 0, 0, 0);
}
GUI_Widget:: GUI_Widget(void *data, int x, int y, int w, int h)
{
	Init(data, x, y, w, h);
}

GUI_Widget::~GUI_Widget()
{
 for(;!children.empty();)
    {
     GUI_Widget *child = children.front();
     
     children.pop_front();
     delete child;
    }

 return;
}
  
void GUI_Widget::Init(void *data, int x, int y, int w, int h)
{
  focused = false;
  gui_drag_manager = NULL; //set from placeOnScreen method
	widget_data = data;
	screen = NULL;
  surface = NULL;
	SetRect(0, 0, w, h);
  offset_x = x;
  offset_y = y;
	Show();
	error = NULL;
	for (int n=0;n<3; ++n ) {
		pressed[n]=0;
	}
 parent = NULL;
 
 update_display = true;
 mousedouble_delay = 300; // SB-X
 last_mousedown_time = last_mouseup_time = 0;
 last_mousedown_x = last_mousedown_y = 0;
 last_mousedown_button = 0;
 last_mouseup_x = last_mouseup_y = 0;
 last_mouseup_button = 0;

 int mx, my;
 SDL_GetMouseState(&mx, &my);
 mouse_over = HitRect(mx, my);
}

int GUI_Widget::AddWidget(GUI_Widget *widget)
{
 children.push_back(widget);
 widget->setParent(this);
 
 return 0; //success.
}

  
/* Mark the widget as visible -- this is the default state */
void GUI_Widget::Show(void)
{
	status = WIDGET_VISIBLE;
}

/* Mark the widget as hidden;  no display, no events */
void GUI_Widget::Hide(void)
{
	status = WIDGET_HIDDEN;
}

/* Mark the widget as free, so it will be deleted by the GUI */
void GUI_Widget:: Delete(void)
{
	status = WIDGET_DELETED;
}

void GUI_Widget::MoveRelative(int dx,int dy)
{
 std::list<GUI_Widget *>::iterator child;

 area.x += dx;
 area.y += dy;
 
 for(child = children.begin(); child != children.end(); child++)
    (*child)->MoveRelative(dx,dy);

 return;
}

void GUI_Widget::Move(int new_x,int new_y)
{
 std::list<GUI_Widget *>::iterator child;

 area.x = new_x + offset_x;
 area.y = new_y + offset_y;
 
 for(child = children.begin(); child != children.end(); child++)
    (*child)->Move(area.x, area.y);

 return;
}

void GUI_Widget::grab_focus()
{
 if(GUI::get_gui()->set_focus(this))
   focused = true;
}

void GUI_Widget::release_focus()
{
 GUI::get_gui()->clear_focus();
 focused = false;
}

void GUI_Widget::PlaceOnScreen(Screen *s, GUI_DragManager *dm, int x, int y)
{
 std::list<GUI_Widget *>::iterator child;

 if(screen != NULL)
   return;

 area.x = x + offset_x;
 area.y = y + offset_y;
 
 gui_drag_manager = dm;
 
 SetDisplay(s);
 
 /* place our children relative to ourself */
 for(child = children.begin(); child != children.end(); child++)
    (*child)->PlaceOnScreen(screen, dm, area.x, area.y);
 return;
}

/* Report status to GUI */
int
GUI_Widget:: Status(void)
{
	return(status);
}

/* Set the bounds of the widget.
   If 'w' or 'h' is -1, that parameter will not be changed.
 */
void 
GUI_Widget:: SetRect(int x, int y, int w, int h)
{
	area.x = x;
	area.y = y;
	if ( w >= 0 ) {
		area.w = w;
	}
	if ( h >= 0 ) {
		area.h = h;
	}
}
void 
GUI_Widget:: SetRect(SDL_Rect **bounds)
{
	int minx, maxx;
	int miny, maxy;
	int i, v;

	maxx = 0;
	maxy = 0;
	for ( i=0; bounds[i]; ++i ) {
		v = (bounds[i]->x+bounds[i]->w-1);
		if ( maxx < v ) {
			maxx = v;
		}
		v = (bounds[i]->y+bounds[i]->h-1);
		if ( maxy < v ) {
			maxy = v;
		}
	}
	minx = maxx;
	miny = maxy;
	for ( i=0; bounds[i]; ++i ) {
		v = bounds[i]->x;
		if ( minx > v ) {
			minx = v;
		}
		v = bounds[i]->y;
		if ( miny > v ) {
			miny = v;
		}
	}
	SetRect(minx, miny, (maxx-minx+1), (maxy-miny+1));
}

/* Check to see if a point intersects the bounds of the widget.
 */
int GUI_Widget::HitRect(int x, int y)
{
	return(HitRect(x, y, area));
}

int GUI_Widget::HitRect(int x, int y, SDL_Rect &rect)
{
	int hit;

	hit = 1;
	if ( (x < rect.x) || (x >= (rect.x+rect.w)) ||
	     (y < rect.y) || (y >= (rect.y+rect.h)) ) {
		hit = 0;
	}
	return(hit);
}

/* Set the display surface for this widget */
void GUI_Widget::SetDisplay(Screen *s)
{
	screen = s;
  surface = screen->get_sdl_surface();
}

void GUI_Widget::setParent(GUI_Widget *widget)
{
 parent = widget;
}

/* Show the widget.
   If the surface needs to be locked, it will be locked
   before this call, and unlocked after it returns.

****************NO, NOT AT ALL IF I'M NOT TOO DUMB TO LOOK******
******OTHERWISE YOU COULDN'T FILLRECT in Display(), ETC!!!! ***********
 */
void GUI_Widget::Display(bool full_redraw)
{
 DisplayChildren(full_redraw);
}

void GUI_Widget::DisplayChildren(bool full_redraw)
{
 if(update_display)
   full_redraw = true;

 if(children.empty() == false)
  {
   std::list<GUI_Widget *>::iterator child;

   /* display our children */
   for(child = children.begin(); child != children.end(); child++)
      {
       if((*child)->Status() == WIDGET_VISIBLE)
         (*child)->Display(full_redraw);
      }
  }

 return;
}

/* Redraw the widget and only the widget */
void GUI_Widget::Redraw(void)
{
 
  if (status==WIDGET_VISIBLE)
  {
   update_display = true;
   if(parent != NULL)
     parent->Redraw();
    //Display();
    //SDL_UpdateRects(screen,1,&area);
  }
}

/* GUI idle function -- run when no events pending */
GUI_status GUI_Widget::Idle(void)
{
	return(GUI_PASS);
}


/* Widget event handlers.
   These functions should return a status telling the GUI whether
   or not the event should be passed on to other widgets.
   These are called by the default HandleEvent function.
*/
GUI_status GUI_Widget::KeyDown(SDL_keysym key)
{
	return(GUI_PASS);
}

GUI_status GUI_Widget::KeyUp(SDL_keysym key)
{
	return(GUI_PASS);
}

GUI_status GUI_Widget::MouseDown(int x, int y, int button)
{
	return(GUI_PASS);
}

GUI_status GUI_Widget::MouseUp(int x, int y, int button)
{
	return(GUI_PASS);
}


GUI_status GUI_Widget::MouseMotion(int x, int y, Uint8 state)
{
	return(GUI_PASS);
}

/* Main event handler function.
   This function gets raw SDL events from the GUI.
 */
GUI_status GUI_Widget::HandleEvent(const SDL_Event *event)
{
 if(children.empty() == false)
  {
   std::list<GUI_Widget *>::iterator child;

   /* display our children */
   for(child = children.begin(); child != children.end(); child++)
     {
      GUI_status status = (*child)->HandleEvent(event);
      if(status != GUI_PASS)
        return status;
     }
  }

	switch (event->type) {
		case SDL_KEYDOWN: {
			return(KeyDown(event->key.keysym));
		}
		break;
		case SDL_KEYUP: {
			return(KeyUp(event->key.keysym));
		}
		break;
		case SDL_MOUSEBUTTONDOWN: {
			int x, y, button;
			x = event->button.x;
			y = event->button.y;
			if ( focused || HitRect(x, y) ) {
				button = event->button.button;
                                RegisterMouseDown(x, y, button);
				return(MouseDown(x, y, button));
			}
		}
		break;
		case SDL_MOUSEBUTTONUP: {
			int x, y, button;
			x = event->button.x;
			y = event->button.y;
			button = event->button.button;
			if ( focused || HitRect(x, y) )
			{
				button = event->button.button;
				GUI_status ret = MouseUp(x, y, button);
				if(ret != GUI_QUIT)
					ret = RegisterMouseUp(x, y, button);
				return(ret);
			}
			/* if widget was clicked before we must let it deactivate itself*/
			else if (ClickState(1)) return(MouseUp(-1,-1,button));
		}
		break;
		case SDL_MOUSEMOTION: {
			int x, y;
			Uint8 state;
			x = event->motion.x;
			y = event->motion.y;
			state = event->motion.state;
			MouseIdle(); /* reduce [double]click delay, don't worry about return (FIXME: might use a new method for this) */
			if ( focused || HitRect(x, y) )
			{
  			  if ( !mouse_over )
  			  {
  			  	mouse_over = true;
  			  	MouseEnter(state);
  			  }
  			  return(MouseMotion(x, y, state));
			}
			else
			{
				if ( mouse_over )
				{
					mouse_over = false;
					MouseLeave(state);
				}
				/* if widget was clicked before we must let it react*/
				if (ClickState(1)) return(MouseMotion(-1,-1,state));
			}
		}
		break;
		default: {
			/* Pass it along.. */;
		}
		break;
	}
	return(GUI_PASS);
}

// iterate through children if present to hit the correct drag area.
bool GUI_Widget::drag_accept_drop(int x, int y, int message, void *data)
{
 if(children.empty() == false)
   {
    std::list<GUI_Widget *>::iterator child;
      
    for(child = children.begin(); child != children.end(); child++)
      {
       if((*child)->HitRect(x,y))
         {
          if((*child)->drag_accept_drop(x,y,message,data))
            return true;
         }
      }
   }

 return false;
}

void GUI_Widget::drag_perform_drop(int x, int y, int message, void *data)
{
 if(children.empty() == false)
   {
    std::list<GUI_Widget *>::iterator child;
      
    for(child = children.begin(); child != children.end(); child++)
      {
       if((*child)->HitRect(x,y))
         {
          if((*child)->drag_accept_drop(x,y,message,data))
            {
             (*child)->drag_perform_drop(x,y,message,data);
             break;
            }
         }
      }
   }

 return;
}


// SB-X
GUI_status GUI_Widget::MouseIdle()
{
	// idle children
	std::list<GUI_Widget *>::iterator child;
	for(child = children.begin(); child != children.end(); child++)
		(*child)->MouseIdle();

	if(mousedouble_delay && last_mouseup_time)
		if((SDL_GetTicks() - last_mouseup_time) > mousedouble_delay)
		{
			// delay passed with no second click
			cancel_mouseclick(); // don't try MouseDouble()
			if(last_mouseup_x == last_mousedown_x && last_mouseup_y == last_mousedown_y)
				return(MouseClick(last_mouseup_x, last_mouseup_y, last_mouseup_button));
		}
	return(GUI_PASS);
}


GUI_status GUI_Widget::RegisterMouseDown(int x, int y, int button)
{
	last_mousedown_time = SDL_GetTicks();
	last_mousedown_x = x;
	last_mousedown_y = y;
	last_mousedown_button = button;
	return(GUI_PASS);
}


GUI_status GUI_Widget::RegisterMouseUp(int x, int y, int button)
{
	if(mousedouble_delay) // accepting double-click
	{
		if(last_mouseup_time
		   && ((SDL_GetTicks() - last_mouseup_time) <= mousedouble_delay))
		{
			cancel_mouseclick(); // won't attempt MouseClick()
			return(MouseDouble(x, y, button));
		}
	}
	else if(last_mousedown_time && x == last_mousedown_x && y == last_mousedown_y)
		return(MouseClick(x, y, button));
	last_mouseup_time = SDL_GetTicks(); // MouseClick() after delay
	last_mouseup_x = x;
	last_mouseup_y = y;
	last_mouseup_button = button;
	return(GUI_PASS);
}

/* Mouse button was pressed and released over the widget.
 */
GUI_status GUI_Widget::MouseClick(int x, int y, int button)
{
    return(GUI_PASS);
}

/* Mouse button was clicked twice over the widget, within mousedouble_delay.
 */
GUI_status GUI_Widget::MouseDouble(int x, int y, int button)
{
    return(GUI_PASS);
}

/* Mouse cursor passed out of the widget area.
 */
GUI_status GUI_Widget::MouseEnter(Uint8 state)
{
    return(GUI_PASS);
}

/* Mouse cursor passed into the widget area.
 */
GUI_status GUI_Widget::MouseLeave(Uint8 state)
{
    return(GUI_PASS);
}

