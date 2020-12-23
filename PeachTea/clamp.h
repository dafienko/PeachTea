#ifndef PT_CLAMP_H
#define PT_CLAMP_H

#ifndef MIN
#define MIN(a, b) (a > b ? b : a)
#endif

#ifndef MAX
#define MAX(a, b) (a < b ? b : a)
#endif

int clamp(int x, int a, int b);

#endif