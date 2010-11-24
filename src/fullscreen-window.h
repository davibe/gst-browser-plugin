#ifndef _FULLSCREEN_WINDOW
#define _FULLSCREEN_WINDOW

#include <glib-object.h>

G_BEGIN_DECLS

#define FULLSCREEN_TYPE_WINDOW fullscreen_window_get_type()

#define FULLSCREEN_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), FULLSCREEN_TYPE_WINDOW, FullscreenWindow))

#define FULLSCREEN_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), FULLSCREEN_TYPE_WINDOW, FullscreenWindowClass))

#define FULLSCREEN_IS_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FULLSCREEN_TYPE_WINDOW))

#define FULLSCREEN_IS_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), FULLSCREEN_TYPE_WINDOW))

#define FULLSCREEN_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), FULLSCREEN_TYPE_WINDOW, FullscreenWindowClass))

typedef struct _FullscreenWindowPrivate FullscreenWindowPrivate;

typedef struct {
  GObject parent;

  FullscreenWindowPrivate *priv;
} FullscreenWindow;

typedef struct {
  GObjectClass parent_class;
  /*signals*/
  void (*clicked)(FullscreenWindow *);
} FullscreenWindowClass;

GType fullscreen_window_get_type (void);

FullscreenWindow* fullscreen_window_new (void);
void* fullscreen_window_get_handle (FullscreenWindow *);

G_END_DECLS

#endif 

