#ifndef _IMPORT_CSV_H
#define _IMPORT_CSV_H (1)

#include <gtk/gtk.h>

/* START_INCLUDE_H */
#include "import.h"
/* END_INCLUDE_H */

struct csv_field {
    gchar * 	name;
    gfloat 		alignment;
    gboolean 	(*validate) (gchar *);
    gboolean 	(*parse) (struct ImportTransaction *, gchar *);
    gchar *		alias;
};

/* START_DECLARATION */
gboolean 		csv_import_csv_account 			(GtkWidget *assistant,
												 struct ImportFile *imported);
GtkWidget * 	import_create_csv_preview_page 	(GtkWidget *assistant);
gboolean 		import_enter_csv_preview_page 	(GtkWidget *assistant);
/* END_DECLARATION */

#endif
