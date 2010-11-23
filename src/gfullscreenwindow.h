#include <windows.h>

struct _GFullScreenWindow {
  HANDLE window_created_signal;
  HWND hWnd;  
  GThread *thread;
};

typedef struct _GFullScreenWindow GFullScreenWindow;

GFullScreenWindow *g_fullscreen_window_new ();
void g_fullscreen_window_show (GFullScreenWindow *obj);
void g_fullscreen_window_hide (GFullScreenWindow *obj);
void g_fullscreen_window_close (GFullScreenWindow *obj);