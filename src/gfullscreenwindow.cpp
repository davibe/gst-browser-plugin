#include <windows.h>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>

#include "gfullscreenwindow.h"

LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_CLOSE:
      DestroyWindow (hwnd);
    break;
    case WM_LBUTTONDOWN:
      ShowWindow(hwnd, SW_HIDE);
    break;
    case WM_DESTROY:
      PostQuitMessage (0);
    break;
    default:
      return DefWindowProc (hwnd, msg, wParam, lParam);
  }
  return 0;
}

static gpointer gst_directdraw_sink_window_thread (gpointer object) {

  GFullScreenWindow *obj;

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

  obj = (GFullScreenWindow *) object;

  obj->hWnd = CreateWindowEx (0, "wimtv",
      "wimtv",
      WS_EX_TOPMOST | WS_POPUP , 0, 0,
      GetSystemMetrics(SM_CXSCREEN),
      GetSystemMetrics(SM_CYSCREEN),
      NULL, NULL,
      WndClass.hInstance, (LPVOID) NULL);

  ReleaseSemaphore (obj->window_created_signal, 1, NULL);

  ShowWindow(obj->hWnd, SW_HIDE);

  while (GetMessage ( &msg, NULL, 0, 0) != FALSE) {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
  }
  
  return NULL;
}

GFullScreenWindow *g_fullscreen_window_new () {
  GFullScreenWindow *obj = (GFullScreenWindow *) 
      calloc (1, sizeof(GFullScreenWindow));

  obj->window_created_signal = CreateSemaphore (NULL, 0, 1, NULL);
  
  obj->thread = g_thread_create (
    (GThreadFunc) gst_directdraw_sink_window_thread, obj, TRUE, NULL);

  /* wait maximum 10 seconds for windows creating */
  if (WaitForSingleObject (obj->window_created_signal, 10000) != WAIT_OBJECT_0)
    return NULL;

  CloseHandle (obj->window_created_signal);

  return obj;
}

void g_fullscreen_window_close (GFullScreenWindow *obj) {
  PostMessage(obj->hWnd, WM_QUIT, 0, 0);
  g_thread_join (obj->thread);
}

void g_fullscreen_window_show (GFullScreenWindow *obj) {
  ShowWindow(obj->hWnd, SW_SHOW);
  SetWindowPos(obj->hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void g_fullscreen_window_hide (GFullScreenWindow *obj) {
  ShowWindow(obj->hWnd, SW_HIDE);
}

void main () {
  GMainLoop *loop;
  GstElement *src;
  GstElement *tee;
  GstElement *sink_fs;
  GstElement *sink;
  GstElement *q1;
  GstElement *q2;
  GstPipeline *pipeline;

  GFullScreenWindow * fs_window;

  gst_init (NULL, NULL);

  fs_window = g_fullscreen_window_new ();
  g_fullscreen_window_hide (fs_window);

  loop = g_main_loop_new (NULL, FALSE);  
  pipeline = (GstPipeline *) gst_pipeline_new ("test");

  src = gst_element_factory_make ("videotestsrc", "videotestsrc");
  tee = gst_element_factory_make ("tee", "tee");
  q1 = gst_element_factory_make ("queue", "q1");
  q2 = gst_element_factory_make ("queue", "q2");

  sink = gst_element_factory_make ("directdrawsink", "directdrawsink1");
  sink_fs = gst_element_factory_make ("directdrawsink", "directdrawsink2");

  gst_x_overlay_set_xwindow_id (GST_X_OVERLAY (sink), (gulong) fs_window->hWnd);

  gst_bin_add_many (GST_BIN(pipeline), src, tee, q1, sink, q2, sink_fs, NULL);
  gst_element_link_many (src, tee, NULL);
  gst_element_link_many (q1, sink, NULL);
  gst_element_link_many (q2, sink_fs, NULL);

  gst_pad_link(
      gst_element_get_request_pad (tee, "src%d"),
      gst_element_get_static_pad (q1, "sink"));

  gst_pad_link(
      gst_element_get_request_pad (tee, "src%d"),
      gst_element_get_static_pad (q2, "sink"));
 
  gst_element_set_state (GST_ELEMENT(pipeline), GST_STATE_PLAYING);

  getchar ();

  g_fullscreen_window_show (fs_window);

  getchar ();

  g_fullscreen_window_hide (fs_window);

  getchar ();

  g_fullscreen_window_show (fs_window);
  
  getchar ();

  g_fullscreen_window_close (fs_window);

  gst_x_overlay_set_xwindow_id (GST_X_OVERLAY (sink), (gulong) NULL);

  g_main_loop_run (loop);
}