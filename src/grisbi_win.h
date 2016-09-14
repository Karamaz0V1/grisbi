#ifndef __GRISBI_WIN_H__
#define __GRISBI_WIN_H__

#include <gtk/gtk.h>

/*START_INCLUDE*/
#include "structures.h"
/*END_INCLUDE*/

G_BEGIN_DECLS

#define GRISBI_TYPE_WIN         (grisbi_win_get_type ())
#define GRISBI_WIN(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRISBI_TYPE_WIN, GrisbiWin))
#define GRISBI_IS_WINDOW(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj), GRISBI_TYPE_WIN))

typedef struct _GrisbiWin       GrisbiWin;
typedef struct _GrisbiWinClass  GrisbiWinClass;


GType grisbi_win_get_type (void) G_GNUC_CONST;

const gchar *   grisbi_win_get_filename             (GrisbiWin *win);
void            grisbi_win_set_filename             (GrisbiWin *win,
                                                      const gchar *filename);
GtkWidget *     grisbi_win_get_headings_eb          (GrisbiWin *win);
GtkWidget *     grisbi_win_get_main_box             (GrisbiWin *win);
GtkWidget *     grisbi_win_get_vbox_general         (GrisbiWin *win);

void            grisbi_win_init_menubar             (GrisbiWin *win,
                                                      gpointer app);
void            grisbi_win_menu_move_to_acc_delete  (void);
void            grisbi_win_menu_move_to_acc_new     (void);
void            grisbi_win_menu_move_to_acc_update  (gboolean active);

gboolean        grisbi_win_set_grisbi_title         (gint account_number);
void            grisbi_win_set_size_and_position    (GtkWindow *win);

void gsb_gui_headings_update_suffix (gchar *suffix);
void gsb_gui_headings_update_title (gchar *title);
gboolean gsb_gui_update_show_headings (void);
void gsb_gui_sensitive_headings (gboolean sensitive);


G_END_DECLS

#endif  /* __GRISBI_APP_H__ */
