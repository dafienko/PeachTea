#ifndef PT_WINUTIL_H
#define PT_WINUTIL_H

#include "lightWindows.h"
#include <stdio.h>

// rect bullcrap
#ifndef R_WIDTH
#define R_WIDTH(r) r.right - r.left
#endif
#ifndef R_HEIGHT
#define R_HEIGHT(r) r.bottom - r.top
#endif


#ifndef GWL_WNDPROC
#define GWL_WNDPROC -4
#endif

#endif
