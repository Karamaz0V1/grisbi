#ifndef _GSB_DIRS_H
#define _GSB_DIRS_H

#include <glib.h>

/* START_INCLUDE_H */
/* END_INCLUDE_H */


/* START_DECLARATION */
void gsb_dirs_init ( void );
void gsb_dirs_shutdown ( void );
const gchar *gsb_dirs_get_categories_dir ( void );
const gchar *gsb_dirs_get_home_dir ( void );
const gchar *gsb_dirs_get_locale_dir ( void );
const gchar *gsb_dirs_get_pixmaps_dir ( void );
const gchar *gsb_dirs_get_plugins_dir ( void );
const gchar *gsb_dirs_get_ui_dir ( void );
const gchar *gsb_dirs_get_user_config_dir ( void );
const gchar *gsb_dirs_get_user_data_dir ( void );
/* END_DECLARATION */
#endif
