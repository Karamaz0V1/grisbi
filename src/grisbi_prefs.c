/* ************************************************************************** */
/*                                                                            */
/*     Copyright (C)    2001-2008 Cédric Auger (cedric@grisbi.org)            */
/*          2003-2008 Benjamin Drieu (bdrieu@april.org)                       */
/*          2009-2020 Pierre Biava (grisbi@pierre.biava.name)                 */
/*          https://www.grisbi.org/                                           */
/*                                                                            */
/*  This program is free software; you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation; either version 2 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*                                                                            */
/* ************************************************************************** */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <glib/gi18n.h>

/*START_INCLUDE*/
#include "grisbi_prefs.h"
#include "grisbi_app.h"
#include "grisbi_settings.h"
#include "utils_prefs.h"
#include "prefs_page_accueil.h"
#include "prefs_page_archives.h"
#include "prefs_page_bank.h"
#include "prefs_page_bet_account.h"
#include "prefs_page_bet_general.h"
#include "prefs_page_currency.h"
#include "prefs_page_currency_link.h"
#include "prefs_page_display_adr.h"
#include "prefs_page_display_fonts.h"
#include "prefs_page_display_form.h"
#include "prefs_page_display_gui.h"
#include "prefs_page_display_ope.h"
#include "prefs_page_divers.h"
#include "prefs_page_files.h"
#include "prefs_page_form_completion.h"
#include "prefs_page_form_options.h"
#include "prefs_page_fyear.h"
#include "prefs_page_import.h"
#include "prefs_page_metatree.h"
#include "prefs_page_msg_delete.h"
#include "prefs_page_msg_warning.h"
#include "prefs_page_options_ope.h"
#include "prefs_page_payment_method.h"
#include "prefs_page_reconcile.h"
#include "prefs_page_reconcile_sort.h"
#include "structures.h"
#include "utils_prefs.h"
#include "erreur.h"
/*END_INCLUDE*/


/*START_STATIC*/
/*END_STATIC*/

struct _GrisbiPrefs
{
    GtkDialog dialog;
};

struct _GrisbiPrefsClass
{
    GtkDialogClass parent_class;
};

/* Private structure type */
typedef struct _GrisbiPrefsPrivate GrisbiPrefsPrivate;

struct _GrisbiPrefsPrivate
{
	GtkWidget *			vbox_prefs;
	GtkWidget *			paned_prefs;

	/* panel de gauche */
	GtkWidget *			button_collapse_all;
	GtkWidget *			button_expand_all;
	GtkWidget *			button_left_collapse;
	GtkWidget *			sw_left;
	GtkWidget *			treeview_left;

    /* notebook de droite */
    GtkWidget *			notebook_prefs;
	//~ GtkWidget *			vbox_import_page;

	/* pages num */
	gint 				form_num_page;
	gint				metatree_num_page;
	gint				reconcile_sort_page_num;
 };


G_DEFINE_TYPE_WITH_PRIVATE (GrisbiPrefs, grisbi_prefs, GTK_TYPE_DIALOG)

/******************************************************************************/
/* Private functions                                                          */
/******************************************************************************/
/* LEFT PANED */
/**
 * set bouton in mode collapse when select an element
 *
 * \param
 * \param
 * \param
 *
 * \return
 **/
static void grisbi_prefs_set_collapse_mode (GtkWidget *button,
                                            GrisbiPrefs *prefs)
{
	GrisbiPrefsPrivate *priv;

	priv = grisbi_prefs_get_instance_private (prefs);
	if (gtk_widget_get_no_show_all (priv->button_expand_all))
	{
		gtk_widget_set_no_show_all (priv->button_expand_all, FALSE);
	}
	else
	{
		gtk_widget_set_no_show_all (priv->button_collapse_all, FALSE);
	}

	gtk_widget_hide (priv->button_expand_all);
	gtk_widget_show_all (priv->button_collapse_all);
}

/**
 * Cette fonction réduit ou développe toutes les lignes du tree_view.
 * Le libellé du bouton est modifié en conséquence.
 *
 * \param le button de commande
 * \param le tree_view considéré
 *
 * \return
 **/
static void grisbi_prefs_collapse_expand_all_rows (GtkToggleButton *button,
												   GrisbiPrefs *prefs)
{
	GrisbiWinRun *w_run;
	GrisbiPrefsPrivate *priv;

	priv = grisbi_prefs_get_instance_private (prefs);
	w_run = (GrisbiWinRun *) grisbi_win_get_w_run ();

	if (gtk_widget_get_no_show_all (priv->button_expand_all))
	{
		gtk_widget_set_no_show_all (priv->button_expand_all, FALSE);
	}
	else
	{
		gtk_widget_set_no_show_all (priv->button_collapse_all, FALSE);
	}

	if (gtk_toggle_button_get_active (button))
	{
		gtk_widget_hide (priv->button_expand_all);
		gtk_widget_show_all (priv->button_collapse_all);
		gtk_tree_view_expand_all (GTK_TREE_VIEW (priv->treeview_left));
		w_run->prefs_expand_tree = TRUE;
	}
	else
	{
		gtk_widget_show_all (priv->button_expand_all);
		gtk_widget_hide (priv->button_collapse_all);
		gtk_tree_view_collapse_all (GTK_TREE_VIEW (priv->treeview_left));
		w_run->prefs_expand_tree = FALSE;
	}
}

/**
 * remplit le model pour la configuration des états
 *
 * \param GtkTreeStore		model
 * \param GrisbiPrefs		prefs
 *
 * \return
 **/
static void grisbi_prefs_left_panel_populate_tree_model (GrisbiPrefs *prefs)
{
	GtkWidget *widget = NULL;
	GtkTreeStore *tree_model;
	gint page = 0;
	gboolean is_loading;
	GrisbiPrefsPrivate *priv;

	devel_debug (NULL);

	priv = grisbi_prefs_get_instance_private (prefs);
	is_loading = grisbi_win_file_is_loading ();

	tree_model = GTK_TREE_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (priv->treeview_left)));

	/* append group page "Generalities" */
	utils_prefs_left_panel_add_line (tree_model, NULL, NULL, _("Generalities"), -1);

    /* append page Fichiers */
	widget = GTK_WIDGET (prefs_page_files_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Files"), page);
	page++;

	/* append page Archives */
	widget = GTK_WIDGET (prefs_page_archives_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Archives"), page);
	page++;

	/* append page Import */
    widget = GTK_WIDGET (prefs_page_import_new (prefs));
	if (is_loading == FALSE)
		gtk_widget_set_sensitive (widget, FALSE);
    utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Import"), page);
    page++;

	/* append page Divers */
    widget = GTK_WIDGET (prefs_page_divers_new (prefs));
    utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Various settings"), page);
    page++;

	/* append page Accueil */
	widget = GTK_WIDGET (prefs_page_accueil_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Main page"), page);
	page++;

	/* append group page "Display" */
    utils_prefs_left_panel_add_line (tree_model, NULL, NULL, _("Display"), -1);

	/* append page Fonts & logo */
	widget = GTK_WIDGET (prefs_page_display_fonts_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Fonts & Logo"), page);
	page++;

	/* append page Messages & warnings */
	widget = GTK_WIDGET (prefs_page_msg_warning_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Messages & warnings"), page);
	page++;

	/* append page Addresses & titles */
	widget = GTK_WIDGET (prefs_page_display_adr_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Addresses & titles"), page);
	page++;

	/* append page Payees, categories and budgetaries */
	widget = GTK_WIDGET (prefs_page_metatree_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Payees, categories and budgetaries"), page);
	priv->metatree_num_page = page;
	page++;

	/* append page Elements of interface */
	widget = GTK_WIDGET (prefs_page_display_gui_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Elements of interface"), page);
	page++;

	/* append group page "Transactions" */
    utils_prefs_left_panel_add_line (tree_model, NULL, NULL, _("Transactions"), -1);

	/* append page List behavior */
	widget = GTK_WIDGET (prefs_page_options_ope_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("List behavior"), page);
	page++;

	/* append page Transactions list cells */
	widget = GTK_WIDGET (prefs_page_display_ope_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Transactions list cells"), page);
	page++;

	/* append page Messages before deleting */
	widget = GTK_WIDGET (prefs_page_msg_delete_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Messages before deleting"), page);
	page++;

	/* append page Reconciliation */
	widget = GTK_WIDGET (prefs_page_reconcile_new (prefs));
	if (is_loading == FALSE)
		gtk_widget_set_sensitive (widget, FALSE);
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Reconciliation"), page);
	page++;

	/* append page Sort for reconciliation */
	widget = GTK_WIDGET (prefs_page_reconcile_sort_new (prefs));
	if (is_loading == FALSE)
		gtk_widget_set_sensitive (widget, FALSE);
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Sort for reconciliation"), page);
	priv->reconcile_sort_page_num = page;
	page++;

    /* append group page "Transaction form" */
    utils_prefs_left_panel_add_line (tree_model, NULL, NULL, _("Transaction form"), -1);

	/* append page Form behavior */
	widget = GTK_WIDGET (prefs_page_form_options_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Form behavior"), page);
	page++;

	/* append page Content of form */
	widget = GTK_WIDGET (prefs_page_display_form_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Content"), page);
	priv->form_num_page = page;
	page++;

	/* append page Completion */
	widget = GTK_WIDGET (prefs_page_form_completion_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Completion"), page);
	page++;

    /* append group page "Resources subtree" */
    utils_prefs_left_panel_add_line (tree_model, NULL, NULL, _("Resources"), -1);

	/* append page Currencies */
	widget = GTK_WIDGET (prefs_page_currency_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Currencies"), page);
	page++;

	/* append page Currencies links */
	widget = GTK_WIDGET (prefs_page_currency_link_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Currencies links"), page);
	page++;

	/* append page Banks */
	widget = GTK_WIDGET (prefs_page_bank_new (prefs));
	if (is_loading == FALSE)
		gtk_widget_set_sensitive (widget, FALSE);
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Banks"), page);
	page++;

	/* append page Financial years */
	widget = GTK_WIDGET (prefs_page_fyear_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Financial years"), page);
	page++;

	/* append page Payment methods */
	widget = GTK_WIDGET (prefs_page_payment_method_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Payment methods"), page);
	page++;

    /* append group page "Balance estimate" */
    utils_prefs_left_panel_add_line (tree_model, NULL, NULL, _("Balance estimate"), -1);

	/* append page General Options */
	widget = GTK_WIDGET (prefs_page_bet_general_new (prefs));
	if (is_loading == FALSE)
		gtk_widget_set_sensitive (widget, FALSE);
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("General Options"), page);
	page++;

	/* append page Accounts data */
	widget = GTK_WIDGET (prefs_page_bet_account_new (prefs));
	utils_prefs_left_panel_add_line (tree_model, priv->notebook_prefs, widget, _("Accounts data"), page);
	page++;
}

/**
 *
 *
 * \param
 * \param
 *
 * \return
 */
static gboolean grisbi_prefs_left_panel_tree_view_selection_changed (GtkTreeSelection *selection,
																	 GtkWidget *notebook)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
	GtkTreePath *path;
    gint selected;
	GrisbiWinRun *w_run;

    if (! gtk_tree_selection_get_selected (selection, &model, &iter))
	{
        return (FALSE);
	}

	w_run = (GrisbiWinRun *) grisbi_win_get_w_run ();
	path = gtk_tree_model_get_path (model, &iter);
	if (w_run->prefs_selected_row)
		g_free (w_run->prefs_selected_row);

	w_run->prefs_selected_row = gtk_tree_path_to_string (path);
    gtk_tree_model_get (model, &iter, 1, &selected, -1);
    gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), selected);
	gtk_tree_path_free (path);

    /* return */
    return FALSE;
}

/**
 * called when we press a button on the list
 *
 * \param tree_view
 * \param ev
 *
 * \return FALSE
 **/
static gboolean grisbi_prefs_left_treeview_button_press (GtkWidget *tree_view,
														 GdkEventButton *ev,
														 GrisbiPrefs *prefs)
{
    if (ev->type == GDK_2BUTTON_PRESS)
    {
        GtkTreePath *path = NULL;

        if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (tree_view), ev->x, ev->y, &path, NULL, NULL, NULL))
        {
            if (gtk_tree_view_row_expanded (GTK_TREE_VIEW (tree_view), path))
                gtk_tree_view_collapse_row (GTK_TREE_VIEW (tree_view), path);
            else
			{
                gtk_tree_view_expand_row (GTK_TREE_VIEW (tree_view), path, FALSE);
				grisbi_prefs_set_collapse_mode (NULL, prefs);
			}

            gtk_tree_path_free (path);

            return FALSE;
        }
    }

    return FALSE;
}

/**
 * création du tree_view qui liste les onglets de la fenêtre de dialogue
 *
 *
 *\return tree_view or NULL;
 **/
static void grisbi_prefs_left_tree_view_setup (GrisbiPrefs *prefs,
                                               GtkWidget *tree_view,
                                               GrisbiWinRun *w_run)
{
    GtkTreeStore *model = NULL;
    GtkCellRenderer *cell;
	GtkTreePath *path;
    GtkTreeSelection *selection;
    GtkTreeViewColumn *column;
	GrisbiPrefsPrivate *priv;

	devel_debug (NULL);
	priv = grisbi_prefs_get_instance_private (prefs);

    /* Création du model */
    model = gtk_tree_store_new (LEFT_PANEL_TREE_NUM_COLUMNS,
								G_TYPE_STRING,  				/* LEFT_PANEL_TREE_TEXT_COLUMN */
								G_TYPE_INT,     				/* LEFT_PANEL_TREE_PAGE_COLUMN */
								G_TYPE_INT,     				/* LEFT_PANEL_TREE_BOLD_COLUMN */
								G_TYPE_INT);    				/* LEFT_PANEL_TREE_ITALIC_COLUMN */

	/* set tree_iew model */
    gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (model));
    g_object_unref (G_OBJECT (model));

    /* set the color of selected row */
	gtk_widget_set_name (tree_view, "tree_view");

    /* make column */
    cell = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes ("Categories",
													   cell,
													   "text", LEFT_PANEL_TREE_TEXT_COLUMN,
													   "weight", LEFT_PANEL_TREE_BOLD_COLUMN,
													   "style", LEFT_PANEL_TREE_ITALIC_COLUMN,
													   NULL);

    gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column), GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), GTK_TREE_VIEW_COLUMN (column));

    /* Choose which entries will be selectable */
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_view));
    gtk_tree_selection_set_select_function (selection,
											utils_prefs_left_panel_tree_view_selectable_func,
											NULL,
											NULL);

	/* set headers hide */
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (tree_view), FALSE);

	/* fill model */
	grisbi_prefs_left_panel_populate_tree_model (prefs);

	/* select the first item */
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_view));
	path = gtk_tree_path_new_from_indices (0, -1);
	gtk_tree_selection_select_path (selection, path);
	gtk_tree_path_free (path);

	/* expand all */
	if (w_run->prefs_expand_tree)
		gtk_tree_view_expand_all (GTK_TREE_VIEW (tree_view));

	g_signal_connect (G_OBJECT (tree_view),
					  "button-press-event",
					  G_CALLBACK (grisbi_prefs_left_treeview_button_press),
					  prefs);

    /* Handle select */
    g_signal_connect (selection,
					  "changed",
					  G_CALLBACK (grisbi_prefs_left_panel_tree_view_selection_changed),
					  priv->notebook_prefs);
}

/**
 * récupère la largeur des préférences
 *
 * \param GtkWidget			prefs
 * \param GtkAllocation		allocation
 * \param gpointer			null
 *
 * \return FALSE
 **/
static gboolean grisbi_prefs_size_allocate (GtkWidget *prefs,
											GtkAllocation *allocation,
											gpointer null)
{

	GSettings *settings;

	settings = grisbi_settings_get_settings (SETTINGS_PREFS);
    conf.prefs_height = allocation->height;
	conf.prefs_width = allocation->width;

    /* save settings_prefs */
	g_settings_set_int (G_SETTINGS (settings), "prefs-height", conf.prefs_height);
	g_settings_set_int (G_SETTINGS (settings), "prefs-width", conf.prefs_width);
	g_settings_set_int (G_SETTINGS (settings), "prefs-panel-width", conf.prefs_panel_width);

	return FALSE;
}

/**
 * save prefs hpahed width
 *
 * \param GtkWidget			hpaned
 * \param GtkAllocation 	allocation
 * \param gpointer			NULL
 *
 * \return FALSE
 **/
static gboolean grisbi_prefs_paned_size_allocate (GtkWidget *prefs_hpaned,
												  GtkAllocation *allocation,
												  gpointer null)
{
    conf.prefs_panel_width = gtk_paned_get_position (GTK_PANED (prefs_hpaned));

	return FALSE;
}

/**
 * Création de la page des preferences
 *
 * \param page
 *
 * \return
 **/
static void grisbi_prefs_setup_page (GrisbiPrefs *prefs,
									 GrisbiWin *win)
{
	GrisbiWinRun *w_run;
	GrisbiPrefsPrivate *priv;

	devel_debug (NULL);
	priv = grisbi_prefs_get_instance_private (prefs);
	w_run = (GrisbiWinRun *) grisbi_win_get_w_run ();

	/* initialise left_tree_view */
	grisbi_prefs_left_tree_view_setup (prefs, priv->treeview_left, w_run);

	/* set geometry */
	if (conf.prefs_height && conf.prefs_width)
	{
        gtk_widget_set_size_request (GTK_WIDGET (prefs), conf.prefs_width, conf.prefs_height);
	}
    else
	{
        gtk_widget_set_size_request(GTK_WIDGET (prefs), PREFS_MIN_WIN_WIDTH, PREFS_MIN_WIN_HEIGHT);
	}

	if (conf.prefs_panel_width)
	{
        gtk_paned_set_position (GTK_PANED (priv->paned_prefs), conf.prefs_panel_width);
	}
    else
	{
		gtk_paned_set_position (GTK_PANED (priv->paned_prefs), PREFS_MIN_PANED_WIDTH);
	}

	/* positionne le button en fonction de run->prefs_expand_tree */
	if (w_run->prefs_expand_tree)
	{
		gtk_widget_set_no_show_all (priv->button_expand_all, TRUE);
		gtk_widget_hide (priv->button_expand_all);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->button_left_collapse), TRUE);
	}
	else
	{
		gtk_widget_set_no_show_all (priv->button_collapse_all, TRUE);
		gtk_widget_hide (priv->button_collapse_all);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->button_left_collapse), FALSE);
	}

	/* set signal button collapse expand all */
	g_signal_connect (G_OBJECT (priv->button_left_collapse),
					  "clicked",
					  G_CALLBACK (grisbi_prefs_collapse_expand_all_rows),
					  prefs);

	/* set signal prefs */
	g_signal_connect (G_OBJECT (prefs),
	                  "size-allocate",
	                  (GCallback) grisbi_prefs_size_allocate,
	                  NULL);

	/* set signal paned_prefs */
	g_signal_connect (G_OBJECT (priv->paned_prefs),
	                  "size-allocate",
	                  (GCallback) grisbi_prefs_paned_size_allocate,
	                  NULL);

}

/******************************************************************************/
/* Fonctions propres à l'initialisation des fenêtres                          */
/******************************************************************************/
static void grisbi_prefs_init (GrisbiPrefs *prefs)
{
	gtk_widget_init_template (GTK_WIDGET (prefs));
}

static void grisbi_prefs_finalize (GObject *object)
{
    /* libération de l'objet prefs */
    G_OBJECT_CLASS (grisbi_prefs_parent_class)->finalize (object);
	grisbi_win_set_prefs_dialog (NULL, NULL);
}

static void grisbi_prefs_class_init (GrisbiPrefsClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = grisbi_prefs_finalize;

	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass),
												 "/org/gtk/grisbi/ui/grisbi_prefs.ui");

	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GrisbiPrefs, vbox_prefs);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GrisbiPrefs, paned_prefs);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GrisbiPrefs, button_collapse_all);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GrisbiPrefs, button_expand_all);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GrisbiPrefs, button_left_collapse);

	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GrisbiPrefs, sw_left);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GrisbiPrefs, treeview_left);

	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass), GrisbiPrefs, notebook_prefs);
}

/******************************************************************************/
/* Public functions                                                           */
/******************************************************************************/
/**
 *
 *
 * \param
 *
 * \return
 **/
GrisbiPrefs *grisbi_prefs_new (GrisbiWin *win)
{
	GrisbiPrefs *prefs;

	devel_debug (NULL);
	prefs = g_object_new (GRISBI_PREFS_TYPE, "transient-for", win, NULL);
	grisbi_prefs_setup_page (prefs, win);

	return prefs;
}

/**
 * callback pour la fermeture des preferences
 *
 * \param prefs_dialog
 * \param result_id
 *
 * \return
 **/
void grisbi_prefs_dialog_response  (GtkDialog *prefs,
                                    gint result_id)
{
	GSettings *settings;

    devel_debug (NULL);
	if (result_id == GTK_RESPONSE_CLOSE)
	{
		GrisbiWinRun *w_run;

		grisbi_win_status_bar_message (_("Preferences stop"));
		if (!prefs)
		{
			grisbi_win_status_bar_message (_("Done"));

			return;
		}
		/* on récupère éventuellement la dimension de la fenêtre */
		w_run = (GrisbiWinRun *) grisbi_win_get_w_run ();
		settings = grisbi_settings_get_settings (SETTINGS_PREFS);
		if (w_run->resolution_screen_toggled == FALSE)
		{
			gtk_window_get_size (GTK_WINDOW (prefs), &conf.prefs_width, &conf.prefs_height);
			g_settings_set_int (G_SETTINGS (settings),
								"prefs-height",
								conf.prefs_height);

			g_settings_set_int (G_SETTINGS (settings),
								"prefs-panel-width",
								conf.prefs_panel_width);

			g_settings_set_int (G_SETTINGS (settings),
								"prefs-width",
								conf.prefs_width);
		}
		else
		{
			w_run->resolution_screen_toggled = FALSE;
			g_settings_reset (G_SETTINGS (settings), "prefs-height");
			g_settings_reset (G_SETTINGS (settings), "prefs-panel-width");
			g_settings_reset (G_SETTINGS (settings), "prefs-width");

			conf.prefs_height = g_settings_get_int (settings, "prefs-height");
			conf.prefs_panel_width = g_settings_get_int (settings, "prefs-panel-width");
			conf.prefs_width = g_settings_get_int (settings, "prefs-width");
		}
	}
	gtk_widget_destroy (GTK_WIDGET (prefs));
	grisbi_win_set_prefs_dialog (NULL, NULL);
	grisbi_win_status_bar_message (_("Done"));
}

/**
 * retourne le widget enfant de la page passée en paramètre
 *
 * \param page_name
 *
 * \return the child of notebook page
 **/
GtkWidget *grisbi_prefs_get_child_by_page_name (const gchar *page_name)
{
	GtkWidget *widget = NULL;
	GrisbiPrefs *prefs;
	GrisbiWin *win;
	GrisbiPrefsPrivate *priv;

	win = grisbi_app_get_active_window (NULL);
	prefs = GRISBI_PREFS (grisbi_win_get_prefs_dialog (win));
	if (!prefs)
		return NULL;

	priv = grisbi_prefs_get_instance_private (prefs);

	if (strcmp (page_name, "metatree_num_page") == 0)
	{
		widget = gtk_notebook_get_nth_page (GTK_NOTEBOOK (priv->notebook_prefs), priv->metatree_num_page);
	}
	else if (strcmp (page_name, "reconcile_sort_page_num") == 0)
	{
		widget = gtk_notebook_get_nth_page (GTK_NOTEBOOK (priv->notebook_prefs), priv->reconcile_sort_page_num);
	}

	return widget;
}

/**
 *
 *
 * \param
 *
 * \return
 **/
void grisbi_prefs_set_page_by_name (gchar *page_name)
{
	GrisbiPrefs *prefs;
	GrisbiWin *win;
	GrisbiPrefsPrivate *priv;
	gint result;

	devel_debug (page_name);
	win = grisbi_app_get_active_window (NULL);
	prefs = grisbi_prefs_new (win);
	priv = grisbi_prefs_get_instance_private (prefs);
	grisbi_win_set_prefs_dialog (win, GTK_WIDGET (prefs));

	gtk_window_present (GTK_WINDOW (prefs));
	gtk_widget_show_all (GTK_WIDGET (prefs));

	if (strcmp (page_name, "form_num_page") == 0)
	{
		utils_prefs_left_panel_tree_view_select_page (priv->treeview_left,
		                                              priv->notebook_prefs,
		                                              priv->form_num_page);
	}
	result = gtk_dialog_run (GTK_DIALOG (prefs));
	grisbi_prefs_dialog_response (GTK_DIALOG (prefs), result);
}

/**
 *
 *
 * \param
 *
 * \return
 **/
/* Local Variables: */
/* c-basic-offset: 4 */
/* End: */

