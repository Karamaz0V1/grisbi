GtkWidget *onglet_categories ( void );
void remplit_arbre_categ ( void );
gboolean ouverture_node_categ ( GtkWidget *arbre, GtkCTreeNode *node,
				gpointer null );
gboolean selection_ligne_categ ( GtkCTree *arbre_categ, GtkCTreeNode *noeud,
				 gint colonne, gpointer null );
gboolean verifie_double_click_categ ( GtkWidget *liste, GdkEventButton *ev, 
				      gpointer null );
gboolean enleve_selection_ligne_categ ( void );
void modification_du_texte_categ ( void );
void clique_sur_modifier_categ ( void );
void clique_sur_annuler_categ ( void );
void supprimer_categ ( void );
void supprimer_sous_categ ( void );
void creation_liste_categories ( void );
void merge_liste_categories ( void );
void creation_liste_categ_combofix ( void );
void mise_a_jour_combofix_categ ( void );
void calcule_total_montant_categ ( void );
gchar *calcule_total_montant_categ_par_compte ( gint categ,
						gint sous_categ,
						gint no_compte );
void appui_sur_ajout_categorie ( void );
void appui_sur_ajout_sous_categorie ( void );
void exporter_categ ( void );
void importer_categ ( void );
gboolean keypress_category ( GtkWidget *widget, GdkEventKey *ev, gint *no_origine );
void expand_selected_category () ;


struct struct_categ *categ_par_nom ( gchar *nom_categ,
				     gboolean creer,
				     gint type_categ,
				     gint no_derniere_sous_categ );
struct struct_sous_categ *sous_categ_par_nom ( struct struct_categ *categ,
					       gchar *nom_sous_categ,
					       gboolean creer );
struct struct_categ *categ_par_no ( gint no_categorie );
struct struct_sous_categ *sous_categ_par_no ( gint no_categorie,
					     gint no_sous_categorie );
gchar *nom_categ_par_no ( gint no_categorie,
			  gint no_sous_categorie );
gchar *nom_sous_categ_par_no ( gint no_categorie,
			       gint no_sous_categorie );

