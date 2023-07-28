#ifndef TP7_CODE_TUTEURS_H_
#define TP7_CODE_TUTEURS_H_

#define _CMD_LIST  "LIST"
#define _CMD_ACQ_S "ACQUIRE"
#define _CMD_QUEST "QUESTION"
#define _CMD_REL   "RELEASE"
#define _CMD_QUIT  "QUIT"
#define _MSG_ALREADY_ASSIGNED   "TUTOR_ALREADY_ASSIGNED"
#define _MSG_NOT_ASSIGNED "TUTOR_NOT_ASSIGNED"
#define _MSG_NOT_AVAILABLE "TUTOR_NOT_AVAILABLE"
#define _MSG_ERR   "ERROR"
#define _MSG_MAXLENGTH 100
#define _RPLY_MAXMSGS 100


// Structures de données
struct tutor {
	char id[100];
	char subj[100];
	struct tutor *next;
};

struct assignment{
  struct tutor* moniteur;
  int id_student;
  struct assignment *next;
};

/*
 * Initialise une liste de tuteurs à partir d'un fichier.
 * @param[in]	filepath	La reference du fichier
 * @param[out]	tutors  	La tête de la liste des tuteurs
 */
int init_tutors(char *, struct tutor **);

/*
 * Copie une liste de tuteurs
 * @param[in]	tutor  	        La tête de la liste des tuteurs à copier
 * @return			La tête de la copie
 */
struct tutor *copy_tutors(struct tutor *);

/*
 * Copie une liste d'associations
 * @param[in]	assign          La tête de la liste d'associations à copier
 * @return			La tête de la copie
 */
struct assignment *copy_assign(struct assignment * assign);

/*
 * Assigne un tuteur a un etudiant.
 * @param[in]	tutors  	La tête de la liste des tuteurs
 * @param[in]   assignments     La tête de la liste des associations
 * @param[in]   subject         Le nom de la discipline a assigner
 * @param[in]   id              L'identifiant de l'etudiant (par exemple la socket de connexion) 
 * @return                      Le pointeur sur l'association creee. Cette association est 
 *                              incluse dans la liste des associations. Si l'association n'a pas
 *                              ete possible, NULL est retourne.
 */
struct assignment* assign(struct tutor**, struct assignment**, char *, int);

/*
 * Verifie qu'un tuteur peut etre assigne a l'etudiant.
 * @param[in]   assignments     La tête de la liste des associations
 * @param[in]   id              L'identifiant de l'etudiant (par exemple la socket de connexion)
 * @return                      Si l'etudiant a deja un tuteur assigne
 */
int can_assign(struct assignment*, int);

/*
 * Retourne l'identifiant du tuteur de l'etudiant.
 * @param[in]   list_assignments La tête de la liste des associations
 * @param[in]   id_student       L'identifiant de l'etudiant (par exemple la socket de connexion)
 * @return       NULL            Si l'etudiant n'a pas de tuteur assigne
 */
char *id_tutor(struct assignment* list_assignment, int id_student);

/*
 * Ote le tuteur a l'etudiant
 * @param[in]   tutors  	La tête de la liste des tuteurs
 * @param[in]   assignments     La tête de la liste des associations
 * @param[in]   id              L'identifiant de l'etudiant (par exemple la socket de connexion)
 * @return                      Si la remise du tuteur a ete possible.
 */
int release(struct tutor**, struct assignment**, int);

/*
 * Affiche la liste des tuteurs
 * @param[in]	tutors  	La tête de la liste des tuteurs
 */
void print_tutors(struct tutor*);

/*
 * Affiche la liste des associations
 * @param[in] assignments       La tête de la liste des associations
 */
void print_assignments(struct assignment*);


#endif /* TP7_CODE_TUTEURS_H_ */
