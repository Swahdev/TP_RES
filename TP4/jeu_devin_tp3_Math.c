#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>

int flag=0;
int nbG=0;

struct for_threads{
  int client_sock;
  int nrbJoueurs;
  int goal;
  int ind;
};

struct n_p{
  int *clients_sock;
  int nrbJoueurs;
};

/*struct cadeau_fils {
  int clients_sock[2];
};

struct threads_obj_n {
  int *values;
  int flag;
  int *clients_sock;
  int nrbJoueurs;
  unsigned short int goal;
};

struct threads_obj {
  int values[2];
  int flags[2];
  int client_sock[2];
  unsigned short int goal;
};

struct th_id {
  int ind; 
  struct threads_obj *tObj; 
};
*/

int server_1p();
int server_2p();
int server_2p_loop();
int server_np_loop(int nrbJoueurs);
int server_1p_fork();
int server_1p_threads();
void *serve(void *arg);

int main(int argn,char** args) {
  int nrbJoueurs = atoi(args[1]);
  server_np_loop(nrbJoueurs);
}

/***************LES FONCTIONS DE JEU ******************/

/**
 * fait tourner une partie pour un joueur dont le socket est passé en argument
 */

/* 
void game_1p(int sock) {
  srand(time(NULL) + sock);
  
  // une valeur aléatoire mystere entre 0 et 65536
  unsigned short int n = rand() % (1 << 16); 
  printf("nb mystere pour partie socket %d = %d\n", sock, n);

  unsigned short int guess;  // le nb proposé par le joueur, sur 2 octets
  int taille = 0;
  int tentatives = 20;
  int gagne = 0;
  char buff_in[100];
  while ((taille = recv(sock, buff_in, 100, 0)) > 0) {
    sscanf(buff_in, "%hu", &guess);
    printf("Joueur courant a envoyé : %d\n", guess);
    char reponse[20];
    if (n < guess || n > guess) {
      tentatives--;
    }
    if (tentatives == 0)
      sprintf(reponse, "PERDU\n");
    else if (n < guess)
      sprintf(reponse, "MOINS %d\n", tentatives);
    else if (n > guess)
      sprintf(reponse, "PLUS %d\n", tentatives);
    else {
      sprintf(reponse, "GAGNE\n");
      gagne = 1;
    }
    send(sock, reponse, strlen(reponse), 0);
    if (gagne || !tentatives) break;
  }
  printf("Fin de partie\n");

  close(sock);
}

int server_1p() {
  int serv_sock = socket(PF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(4242);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int r = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (r != 0) {
    fprintf(stderr, "Échec de bind");
    exit(-1);
  }

  r = listen(serv_sock, 0);
  if (r != 0) {
    fprintf(stderr, "Échec de listen");
    exit(-1);
  }

  int client_sock = 0;
  while (1) {
    client_sock = accept(serv_sock, NULL, NULL);
    if (client_sock < 0) {
      fprintf(stderr, "Échec de accept");
      exit(-1);
    } 
    else {
      printf("Connexion acceptee, nouvelle partie lancée.\n");
      game_1p(client_sock);
      close(client_sock);      
    }
  }
  close(serv_sock);
  return 0;
}


int server_1p_fork() {
  int serv_sock = socket(PF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(4242);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int r = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (r != 0) {
    fprintf(stderr, "Échec de bind");
    exit(-1);
  }

  r = listen(serv_sock, 0);
  if (r != 0) {
    fprintf(stderr, "Échec de listen");
    exit(-1);
  }

  int client_sock = 0;
  while (1) {
    client_sock = accept(serv_sock, NULL, NULL);
    if (client_sock < 0) {
      fprintf(stderr, "Échec de accept");
      exit(-1);
    } 
    else {
      printf("Connexion acceptee, nouvelle partie lancée.\n");
      int f = -2;
      switch(f = fork()){
        case -1 : 
            fprintf(stderr, "Problème de fork");
            exit(-1);
        case 0 : 
            close(serv_sock);
            game_1p(client_sock);
            exit(0); 
        default : break;  
      }
      close(client_sock);      
    }
    while(waitpid(-1, NULL, WNOHANG) > 0);
  }
  close(serv_sock);
  return 0;
}

int server_1p_threads() {
  int serv_sock = socket(PF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(4242);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int r = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (r != 0) {
    fprintf(stderr, "Échec de bind");
    exit(-1);
  }

  r = listen(serv_sock, 0);
  if (r != 0) {
    fprintf(stderr, "Échec de listen");
    exit(-1);
  }

  int *sockclient = malloc(sizeof(int));
  while (1) {
    *sockclient = accept(serv_sock, NULL, NULL);
    if (*sockclient < 0){
      fprintf(stderr, "Erreur d'accept");
      exit(-1);
    }
    pthread_t thread;
    if (pthread_create(&thread, NULL, serve, sockclient) == -1){
      perror("pthread_create");
      continue;
    }
    else {
      close(*sockclient);      
    }
  }
  close(serv_sock);
  return 0;
}

void *serve(void *arg) {
  int sock = *((int *) arg); //on récupère le descripteur de socket
  game_1p(sock);
  return NULL;
}

void *threads_jeu(void* th_ob) {
  struct th_id *th_ids = (struct th_id *) th_ob;
  struct threads_obj *th_obj = (struct threads_obj *) th_ids->tObj;
  int sock = th_ids->tObj->client_sock[th_ids->ind]; //on récupère le descripteur de socket

  unsigned short int guess; 
  int taille = 0;
  int tentatives = 10;
  int gagne = 0;
  char buff_in[100];
  printf("client socket : %d\n", sock);
  while ((taille = recv(sock, buff_in, 100, 0)) > 0) {
    sscanf(buff_in, "%hu", &guess);
    printf("Joueur %d a envoyé : %d\n", th_ids->ind+1, guess);
    char reponse[20];

    th_obj->values[th_ids->ind]=guess;
    th_obj->flags[th_ids->ind]=0;
    while(th_obj->flags[0]!=0 || th_obj->flags[1]!=0){}
    th_obj->flags[th_ids->ind]=1;

    if (th_obj->values[0] == th_obj->goal && th_obj->values[1] != th_obj->goal){
      if(th_ids->ind==0)
        sprintf(reponse, "GAGNE\n");
      else
        sprintf(reponse, "PERDU\n");
      gagne = 1;  
    }
    else if (th_obj->values[1] == th_obj->goal && th_obj->values[0] != th_obj->goal){
      if(th_ids->ind==1)
        sprintf(reponse, "GAGNE\n");
      else
        sprintf(reponse, "PERDU\n");
      gagne = 1;
    }
    else if (th_obj->values[1] == th_obj->goal && th_obj->values[0] == th_obj->goal){
      sprintf(reponse, "MATCH NUL\n");
      gagne = 1;
    }
    else{
      tentatives-=1;
      if (tentatives == 0)
        sprintf(reponse, "PERDU\n");
      else if (th_obj->goal < guess)
        sprintf(reponse, "MOINS %d\n", tentatives);
      else
        sprintf(reponse, "PLUS %d\n", tentatives);
    }

    send(sock, reponse, strlen(reponse), 0);
    if (gagne || !tentatives) break;
  }

  close(sock);

  return NULL;
}

int server_2p() {
  int serv_sock = socket(PF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(9191);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int r = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (r != 0) {
    fprintf(stderr, "Échec de bind");
    exit(-1);
  }

  r = listen(serv_sock, 0);
  if (r != 0) {
    fprintf(stderr, "Échec de listen");
    exit(-1);
  }

  struct threads_obj *tObj;
  tObj = malloc(sizeof(struct threads_obj));

  srand(time(NULL) + serv_sock);

  unsigned int short k = rand() % (1 << 16);

  tObj->goal = k;
  tObj->flags[0] = 1;
  tObj->flags[1] = 1;

  printf("La valeur à trouver est : %hu\n", k);

  for(int i=0; i<2; i++) {

    struct th_id *thread_id;
    thread_id = malloc(sizeof(struct th_id));

    int sockclient = accept(serv_sock, NULL, NULL);

    tObj->client_sock[i]= sockclient;

    thread_id->ind = i; 
    thread_id->tObj = tObj;

    if (sockclient < 0){
      fprintf(stderr, "Erreur d'accept");
      exit(-1);
    }
    pthread_t thread;
    if (pthread_create(&thread, NULL, threads_jeu, thread_id) == -1){
      perror("pthread_create");
      continue;
    }
  }
  while(1){}
  close(serv_sock);
  return 0;
}

void *multi_gaming(void *arg) {
    struct cadeau_fils *cdo = (struct cadeau_fils *) arg;
    struct threads_obj *tObj;
    tObj = malloc(sizeof(struct threads_obj));

    srand(time(NULL) + cdo->clients_sock[0]);

    unsigned int short k = rand() % (1 << 16);

    tObj->goal = k;
    tObj->flags[0] = 1;
    tObj->flags[1] = 1;

    printf("La valeur à trouver est : %hu\n", k);

    for(int i=0; i<2; i++) {

      struct th_id *thread_id;
      thread_id = malloc(sizeof(struct th_id));

      tObj->client_sock[i] = cdo->clients_sock[i];

      thread_id->ind = i; 
      thread_id->tObj = tObj;

      pthread_t thread;
      if (pthread_create(&thread, NULL, threads_jeu, thread_id) == -1){
        perror("pthread_create");
        continue;
      }
    }
    while(1){}
}

int server_2p_loop() {
  int serv_sock = socket(PF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(9191);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int r = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (r != 0) {
    fprintf(stderr, "Échec de bind");
    exit(-1);
  }

  r = listen(serv_sock, 0);
  if (r != 0) {
    fprintf(stderr, "Échec de listen");
    exit(-1);
  }
  int cpt=0;
  while(1){
    cpt++;
    struct cadeau_fils *cdo;
    cdo = malloc(sizeof(struct cadeau_fils));
    for(int i=0; i<2; i++) {
        int sockclient = accept(serv_sock, NULL, NULL);
        printf("Welcome Player %d to game %d ! \n", i, cpt);
        cdo->clients_sock[i]=sockclient;
    }
    pthread_t thread;
    if (pthread_create(&thread, NULL, multi_gaming, cdo) == -1){
      perror("pthread_create");
      continue;
    }
  }
  close(serv_sock);
  return 0;
}
*/

void *threads_jeu(void* arg) {
  struct for_threads *argu = (struct for_threads *) arg;
  int sock = argu->client_sock; //on récupère le descripteur de socket

  unsigned short int guess; 
  int taille = 1;
  int tentatives = 10;
  int gagne = 0;
  char buff_in[100];
  printf("client socket : %d\n", sock);
  while (taille > 0) {
    nbG=0;
    flag=0;
    taille = recv(sock, buff_in, 100, 0);
    sscanf(buff_in, "%hu", &guess);
    printf("Joueur %d a envoyé : %d\n", argu->ind+1, guess);
    char reponse[20];

    if(guess == argu->goal)
      nbG+=1;
    flag+=1;
    while(flag<argu->nrbJoueurs){}

    if(guess == argu->goal && nbG==1){
      sprintf(reponse, "GAGNE\n");
    }
    else if(guess == argu->goal && nbG>1){
      sprintf(reponse, "MATCH NUL\n");
    }
    else if(nbG>=1){
      sprintf(reponse, "PERDU\n");
    }
    else{
      tentatives-=1;
      if (tentatives == 0)
        sprintf(reponse, "PERDU\n");
      else if (argu->goal < guess)
        sprintf(reponse, "MOINS %d\n", tentatives);
      else
        sprintf(reponse, "PLUS %d\n", tentatives);
    }

    send(sock, reponse, strlen(reponse), 0);
    if (gagne || !tentatives) break;
  }

  close(sock);

  return NULL;
}

void *n_personnes_games(void *arg) {
    struct n_p *thread_n = (struct n_p *) arg; 

    srand(time(NULL) + thread_n->clients_sock[0]);
    unsigned int short k = rand() % (1 << 16);

    printf("La valeur à trouver est : %hu\n", k);

    for(int i=0; i<thread_n->nrbJoueurs; i++) {
      struct for_threads *argu;
      argu = malloc(sizeof(struct for_threads));

      argu->client_sock=thread_n->clients_sock[i];
      argu->nrbJoueurs=thread_n->nrbJoueurs;
      argu->goal=k;
      argu->ind=i;

      pthread_t thread;
      if (pthread_create(&thread, NULL, threads_jeu, argu) == -1){
        perror("pthread_create");
        continue;
      }
    }
    while(1){}
    return 0;
}

int server_np_loop(int nrbJoueurs) {
  int serv_sock = socket(PF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(9191);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int r = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (r != 0) {
    fprintf(stderr, "Échec de bind");
    exit(-1);
  }

  r = listen(serv_sock, 0);
  if (r != 0) {
    fprintf(stderr, "Échec de listen");
    exit(-1);
  }

  int cpt=0;
  
  while(1){
    cpt++;

    struct n_p *thread_n;
    thread_n=malloc(sizeof(struct n_p));
    thread_n->nrbJoueurs=nrbJoueurs;
    thread_n->clients_sock = (int *)malloc(sizeof(int)*nrbJoueurs);

    for(int i=0; i<nrbJoueurs; i++) {
        int sockclient = accept(serv_sock, NULL, NULL);
        printf("Welcome Player %d to game %d ! \n", i+1, cpt);
        thread_n->clients_sock[i]=sockclient;
    }
    pthread_t thread;
    if (pthread_create(&thread, NULL, n_personnes_games, thread_n) == -1){
      perror("pthread_create");
      continue;
    }
  }
  close(serv_sock);
  return 0;
}