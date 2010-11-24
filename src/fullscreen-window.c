
#include "fullscreen-window.h"
#ifdef WIN32
#include <windows.h>
#endif

G_DEFINE_TYPE (FullscreenWindow, fullscreen_window, G_TYPE_OBJECT)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), FULLSCREEN_TYPE_WINDOW, FullscreenWindowPrivate))

struct _FullscreenWindowPrivate {
  int dummy;
#ifdef WIN32
  HANDLE window_created_signal;
  HWND hWnd;
  GThread *thread;
#endif
#ifdef __apple__

#endif
};

enum {
  SIGNAL_CLICKED,  
  LAST_SIGNAL
};

static guint window_signals[LAST_SIGNAL];

#ifdef WIN32
LRESULT CALLBACK 
WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static gpointer 
fullscreen_window_thread (gpointer object);
#endif WIN32

static void
fullscreen_window_finalize (GObject *object)
{
  G_OBJECT_CLASS (fullscreen_window_parent_class)->finalize (object);
}

static void
fullscreen_window_class_init (FullscreenWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (FullscreenWindowPrivate));

  object_class->finalize = fullscreen_window_finalize;

  window_signals[SIGNAL_CLICKED] = g_signal_new ("clicked",
      G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (FullscreenWindowClass, clicked), NULL, NULL,
      g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
  
}

static void
fullscreen_window_init (FullscreenWindow *self)
{
  /* create the window */
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
      FULLSCREEN_TYPE_WINDOW, FullscreenWindowPrivate);

#ifdef WIN32
  self->priv->window_created_signal = CreateSemaphore (NULL, 0, 1, NULL);
  
  self->priv->thread = g_thread_create (
    (GThreadFunc) fullscreen_window_thread, self, TRUE, NULL);

  /* wait maximum 10 seconds for windows creating */
  if (WaitForSingleObject (self->priv->window_created_signal, 10000) != WAIT_OBJECT_0)
    return;

  CloseHandle (self->priv->window_created_signal);
#endif
}

FullscreenWindow*
fullscreen_window_new (void)
{
  return FULLSCREEN_WINDOW (g_object_new (FULLSCREEN_TYPE_WINDOW, NULL));
}

void *
fullscreen_window_get_handle (FullscreenWindow *self)
{
#ifdef WIN32
  return (void *) self->priv->hWnd;
#endif
  return  NULL;
}

#ifdef WIN32
LRESULT CALLBACK 
WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_CLOSE:
      DestroyWindow (hwnd);
    break;
    case WM_LBUTTONDOWN:
      DestroyWindow(hwnd);
    break;
    case WM_DESTROY:
      PostQuitMessage (0);
    break;
    default:
      return DefWindowProc (hwnd, msg, wParam, lParam);
  }
  return 0;
}

static gpointer 
fullscreen_window_thread (gpointer object) {

  FullscreenWindow *obj;

  WNDCLASS WndClass;
  MSG msg;
  memset (&WndClass, 0, sizeof (WNDCLASS));
  WndClass.hInstance = GetModuleHandle (NULL);
  WndClass.lpszClassName = "wimtv";
  WndClass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
  WndClass.cbClsExtra = 0;
  WndClass.cbWndExtra = 0;
  WndClass.lpfnWndProc = WndProc;
  WndClass.hCursor = LoadCursor (NULL, IDC_ARROW);
  RegisterClass (&WndClass);

  obj = (FullscreenWindow *) object;

  obj->priv->hWnd = CreateWindowEx (0, "wimtv",
      "wimtv",
      WS_EX_TOPMOST | WS_POPUP , 0, 0,
      GetSystemMetrics(SM_CXSCREEN),
      GetSystemMetrics(SM_CYSCREEN),
      NULL, NULL,
      WndClass.hInstance, (LPVOID) NULL);

  ReleaseSemaphore (obj->priv->window_created_signal, 1, NULL);

  ShowWindow(obj->priv->hWnd, SW_SHOW);
  SetWindowPos(obj->priv->hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

  while (GetMessage ( &msg, NULL, 0, 0) != FALSE) {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
  }

  g_signal_emit (obj, window_signals[SIGNAL_CLICKED], 0);
  
  return NULL;
}
#endif
