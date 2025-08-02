/*
 * x11-rainbow.c - X11 example program
 *
 * Copyright(C) 2019 - MT
 *
 * Experiments in colour mapping!
 * 
 * This  program is free software: you can redistribute it and/or modify it
 * under  the terms of the GNU General Public License as published  by  the
 * Free  Software Foundation, either version 3 of the License, or (at  your
 * option) any later version.
 *
 * This  program  is distributed in the hope that it will  be  useful,  but
 * WITHOUT   ANY   WARRANTY;   without even   the   implied   warranty   of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You  should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 29 Nov 23  0.1.0001  - Initial version - MT
 *  1 Aug 25            - Updated debug and error handler - MT
 * 
 */

#define  NAME           "x11-rainbow"
#define  VERSION        "0.10"
#define  BUILD          "0001"
#define  DATE           "29 Nov 23"
#define  AUTHOR         "MT"

#define  COLOUR_DEPTH    4
#define  WIDTH           400
#define  HEIGHT          301
#define  BORDER          0

#if defined(VMS)
#define  FONT            "*helvetica-medium-r-*-12-*"
#else
#define  FONT            "7x13"
#endif

#include <string.h>     /* strlen(), etc */
#include <stdarg.h>     /* vargs(), etc */
#include <stdlib.h>     /* exit(), etc */
#include <stdio.h>
#include <errno.h>      /* errno */

#include <X11/Xlib.h>
#include <X11/Xutil.h> 
#include <X11/keysym.h> 

#include "gcc-debug.h"

int main(int argc, char *argv[])
{
   Display* h_display;
   Visual* h_visual;
   Window x_application_window;
   XFontStruct *h_font; /* Pointer to font descriptor */
   XSetWindowAttributes x_attributes;
   XGCValues x_context;
   XEvent x_event;
   XColor x_background_colour, x_text_colour, x_colour, x_exact_colour;
   Atom wm_delete_window;

   char *s_title = NAME; /* Windows title */
   char *s_font = FONT;
   char *s_display_name = ""; /* Just use the default display */
   /* char *s_text_colour = "Tan"; /* Text colour. */
   /* char *s_colours[] = {"Red", "Orange", "Yellow", "Green", "Medium Blue", "Blue Violet", "Violet"}; /* Foreground colours. Note - No 'indigo' on DECwindows!! */
   char *s_colours[] = {"White","Light Gray", "Grey", "Dark Grey", "Dim Grey","Black"}; /* Foreground colours. */
   char *s_text_colour = "#6495ED"; /* Text colour. */

   unsigned int i_window_width = WIDTH; /* Window width in pixels */
   unsigned int i_window_height = HEIGHT; /* Window height in pixels */
   unsigned int i_window_border = BORDER; /* Window's border width - usually ignored by window managers */
   unsigned int i_colour_depth; /* Window's colour depth */
   unsigned int i_screen_width; /* Screen width */
   unsigned int i_screen_height; /* Screen height */
   int i_window_left, i_window_top; /* Window's top-left corner - usually ignored by window managers */
   int i_screen; /* Default screen number */

   int i_offset; /* Position of rectangle */
   int i_indent; /* Position of text */
   int i_height; /* Width of rectangle */
   int i_count, i_colours = sizeof(s_colours) / sizeof(s_colours[0]);

   int i_status = 0;
   int b_abort = 0;

   if ((h_display = XOpenDisplay(s_display_name))) 
   {
      i_screen = DefaultScreen (h_display);
      i_colour_depth = DefaultDepth (h_display, i_screen);
      warning (errno, "Colour depth = %d bpp", i_colour_depth);
      
      if (i_colour_depth >= COLOUR_DEPTH) 
      {
         if ((h_font = XLoadQueryFont(h_display, s_font)))
         {
            if (XAllocNamedColor (h_display, DefaultColormap (h_display, i_screen), s_colours[i_colours - 1], &x_background_colour, &x_exact_colour) &&
               (XAllocNamedColor (h_display, DefaultColormap (h_display, i_screen), s_text_colour, &x_text_colour, &x_exact_colour)))
            {
               h_visual = DefaultVisual (h_display, i_screen);
               
               i_screen_width = DisplayWidth(h_display, i_screen); /* Get screen resolution */
               i_screen_height = DisplayHeight(h_display, i_screen);
               i_window_left = (i_screen_width - i_window_width) /2; /* Calculate position of window */
               i_window_top = (i_screen_height - i_window_height) /2;
            
               x_attributes.background_pixel = x_background_colour.pixel; /* Set colour attribute */

               x_application_window = XCreateWindow (h_display,
                  XRootWindow (h_display, i_screen), i_window_left, i_window_top,
                  i_window_width, i_window_height, i_window_border, i_colour_depth, InputOutput, 
                  h_visual, CWBackPixel, &x_attributes);

               XStoreName (h_display, x_application_window, s_title); /* Set the window title */

               wm_delete_window = XInternAtom(h_display, "WM_DELETE_WINDOW", False);
               XSetWMProtocols(h_display, x_application_window, &wm_delete_window, 1);
               XSelectInput(h_display, x_application_window, ExposureMask | StructureNotifyMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask);  /* Note - ButtonPress is required for ButtonRelease */

               XMapWindow (h_display, x_application_window); /* Draw window */

               while (!(b_abort || i_status))
               {
                  while (XPending(h_display) > 0) 
                  {
                     XNextEvent(h_display, &x_event);
                     switch (x_event.type) 
                     {
                        case ConfigureNotify: /* Note - Requires StructureNotifyMask */
                           if (i_window_width != x_event.xconfigure.width || i_window_height != x_event.xconfigure.height) /* Check if the windows has changed size */
                           {
                              i_window_width = x_event.xconfigure.width;
                              i_window_height = x_event.xconfigure.height;
                           } 
                           else
                              break;
                        case Expose:
                           i_height = i_window_height / i_colours; /* Work out how wide each vertical bar needs to be */
                           for (i_count = 0; i_count < i_colours; i_count++)
                           {
                              if (XAllocNamedColor (h_display, DefaultColormap (h_display, i_screen), s_colours[i_count], &x_colour, &x_exact_colour))
                              {
                                 x_context.foreground = x_colour.pixel; /* Set the foreground colour */
                                 XSetForeground(h_display, DefaultGC(h_display, i_screen), x_context.foreground);
                                 i_offset = i_height * i_count; /* Work out where to put it */
                                 XFillRectangle(h_display, x_application_window, DefaultGC(h_display, i_screen), 0, i_offset, i_window_width, i_height); /* Draw the next vertical colour bar on the window */
                                 
                                 x_context.foreground = x_text_colour.pixel; /* Set text colour */
                                 XSetForeground(h_display, DefaultGC(h_display, i_screen), x_context.foreground);
                                 i_offset = i_offset - h_font->descent + (i_height + h_font->ascent + h_font->descent) / 2;
                                 XSetFont(h_display, DefaultGC(h_display, i_screen), h_font->fid);
                                 i_indent = (i_window_width - XTextWidth(h_font, s_colours[i_count], strlen(s_colours[i_count]))) / 2;
                                 XDrawString(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent, i_offset, s_colours[i_count], strlen(s_colours[i_count])); /* Draw the main text. */
                              }
                              else
                                 error (errno, "Cannot allocate colour '%s'.", s_colours[i_count]); /* Get next colour */
                           }
                           break;
                        case KeyPress:
                           switch (XLookupKeysym(&x_event.xkey, 0))
                           {
                           case XK_Escape: /* Not mapped to keyboard under DEC Windows ? */
                           case XK_space:
                              b_abort = True;
                           }
                           break;
                        case ClientMessage: 
                           if ((Atom) x_event.xclient.data.l[0] == wm_delete_window)
                              b_abort = True;
                           break;
                        case ButtonRelease: 
                           b_abort = True; /* Any mouse click exits */
                     }
                  }
               }
            }
            else
               error (errno, "Cannot allocate colour."); /* Get colour */
         }
         else
          error (errno, "Cannot find font '%s'.", s_font);
      }
      else
         error (errno, "Requires a %d-bit colour display.", COLOUR_DEPTH); /* Check colour depth */
      XFree(h_font);
      XDestroyWindow(h_display, x_application_window);
      XCloseDisplay (h_display);
   }
   else
      error (errno, "Cannot connect to X server '%s'.", s_display_name); /* Open the display and create a new window */
   exit (i_status);
}
