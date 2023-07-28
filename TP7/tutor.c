#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "tutor.h"


/*
 * Initialise une liste de tuteurs à partir d'un fichier.
 * @param[in]	filepath	La reference du fichier
 * @param[out]	tutors  	La tête de la liste des tuteurs
 */
int init_tutors(char *filepath, struct tutor ** tutors) {
  int length = 0;
  FILE *fic = fopen(filepath, "r");
  char buf[100];
  struct tutor * head = *tutors;
  while (fgets(buf, sizeof(buf), fic)) {
    length++;
    if (buf[strlen(buf) - 1] == '\n')
      buf[strlen(buf) - 1] = 0;
    char *space = strchr(buf, ' ');
    *space = 0;
    struct tutor *new_tutor = malloc(sizeof(struct tutor));
    strcpy(new_tutor->id, buf);
    strcpy(new_tutor->subj, space + 1);
    new_tutor->next = head;
    head = new_tutor;
  }
  fclose(fic);
  *tutors = head;
  return length;
}

/*
 * Copie une liste de tuteurs
 * @param[in]	tutor  	        La tête de la liste des tuteurs à copier
 * @return			La tête de la copie
 */
struct tutor *copy_tutors(struct tutor * tutors) {
  printf("debug: copy\n");

  if (tutors == NULL) {return NULL;}
  
  struct tutor *list = tutors, *new_list, *elem, *last;
  new_list = malloc(sizeof(struct tutor));
  strcpy(new_list->id, list->id);
  strcpy(new_list->subj, list->subj);
  last = new_list;
  list = list->next;
  while (list != NULL) {
    elem = malloc(sizeof(struct tutor));
    strcpy(elem->id, list->id);
    strcpy(elem->subj, list->subj);
    last->next = elem;
    last = elem;
    list = list->next;
  }
  return new_list;
}

/*
 * Copie une liste d'associations
 * @param[in]	assign          La tête de la liste d'associations à copier
 * @return			La tête de la copie
 */
struct assignment *copy_assign(struct assignment * assign) {
  printf("debug: copy\n");

  if (assign == NULL) {return NULL;}
  
  struct assignment *list = assign, *new_list, *elem, *last;
  new_list = malloc(sizeof(struct assignment));
  new_list->id_student = list->id_student;
  //new_list->moniteur = copy_tutors(list->moniteur);
  new_list->moniteur = malloc(sizeof(struct tutor));
  strcpy(new_list->moniteur->id, list->moniteur->id);
  strcpy(new_list->moniteur->subj, list->moniteur->subj);
  last = new_list;
  list = list->next;
  while (list != NULL) {
    elem = malloc(sizeof(struct assignment));
    elem->id_student = list->id_student;
    elem->moniteur = malloc(sizeof(struct tutor));
    strcpy(elem->moniteur->id, list->moniteur->id);
    strcpy(elem->moniteur->subj, list->moniteur->subj);
    last->next = elem;
    last = elem;
    list = list->next;
  }
  return new_list;
}

/*
 * Verifie qu'un tuteur peut etre assigne a l'etudiant.
 * @param[in]   list_assignments La tête de la liste des associations
 * @param[in]   id_student       L'identifiant de l'etudiant (par exemple la socket de connexion)
 * @return       0               Si l'etudiant a deja un tuteur assigne
 */
int can_assign(struct assignment* list_assignment, int id_student){
  while (list_assignment!= NULL){
    if (list_assignment->id_student == id_student){
      return 0;
    }
    list_assignment = list_assignment->next;
  }

   return 1;
}

/*
 * Retourne l'identifiant du tuteur de l'etudiant.
 * @param[in]   list_assignments La tête de la liste des associations
 * @param[in]   id_student       L'identifiant de l'etudiant (par exemple la socket de connexion)
 * @return       NULL            Si l'etudiant n'a pas de tuteur assigne
 */
char *id_tutor(struct assignment* list_assignment, int id_student){
  while (list_assignment!= NULL){
    if (list_assignment->id_student == id_student){
      return list_assignment->moniteur->id;
    }
    list_assignment = list_assignment->next;
  }

   return NULL;
}


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
struct assignment* assign(struct tutor** list_tutor, struct assignment ** list_assignment, char *subj, int id_student){
  while (*list_tutor != NULL){
    if (strcmp((*list_tutor)->subj, subj) == 0){
         struct assignment *m = malloc(sizeof(struct assignment));
         m->moniteur = *list_tutor;
         m->id_student = id_student;
         m->next = (*list_assignment);
         *list_assignment = m;
         *list_tutor = (*list_tutor)->next; 
         return m;
    }
    list_tutor = (&(*list_tutor)->next);
  }
  return NULL;
}


/*
 * Ote le tuteur a l'etudiant
 * @param[in]   tutors  	La tête de la liste des tuteurs
 * @param[in]   assignments     La tête de la liste des associations
 * @param[in]   id              L'identifiant de l'etudiant (par exemple la socket de connexion)
 * @return                      Si la remise du tuteur a ete possible.
 */
int release(struct tutor** list_tutor, struct assignment** list_assignment, int id_student){
  while (*list_assignment!= NULL){
    if ((*list_assignment)->id_student == id_student){
      (*list_assignment)->moniteur->next = (*list_tutor);
      (*list_tutor) = (*list_assignment)->moniteur;
      struct assignment* m = *list_assignment;
      (*list_assignment) = m->next;
      free(m);
      return 1;
    }
    list_assignment = (&(*list_assignment)->next);
  }
  return 0;
}

/*
 * Affiche la liste des tuteurs
 * @param[in]	tutors  	La tête de la liste des tuteurs
 */
void print_tutors(struct tutor* tutors){
  printf("BEGIN DEBUG\n");
  while(tutors != NULL){
    printf("%s %s\n", tutors->subj, tutors->id);
    tutors = tutors->next;
  }

  printf("END DEBUG\n");
}

/*
 * Affiche la liste des associations
 * @param[in] assignments       La tête de la liste des associations
 */
void print_assignments(struct assignment* assignments){
  printf("BEGIN DEBUG\n");
  while(assignments!= NULL){
    struct tutor* tutor = assignments->moniteur;
    printf("%d %s %s\n", assignments->id_student, tutor->subj, tutor->id);
    assignments = assignments->next;
  }

  printf("END DEBUG\n");
}
