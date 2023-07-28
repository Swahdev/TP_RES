#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

int server_1p();        // Déclaration de la fonction server_1p()
int server_1p_fork();   // Déclaration de la fonction server_1p_fork()
int server_1p_thread(); // Déclaration de la fonction server_1p_thread()
void game_1p(int sock); // Déclaration de la fonction game_1p()
int serveur_2p();
void game_2p(int client_sock, int client_sock2);
void *game_2p_point(void *arg);

int main(int argn, char **args)
{
  // Si le nombre d'arguments est inférieur ou égal à 1 ou que le premier argument est "-1p"
  if (argn <= 1 || strcmp(args[1], "-1p") == 0)
  {
    serveur_2p(); // Lancer la fonction server_1p() pour démarrer le serveur
  }
}

// Fonction pour gérer une partie pour un joueur unique
void game_1p(int sock)
{
  srand(time(NULL) + sock); // Initialiser le générateur de nombres aléatoires avec le temps actuel + la socket

  // Choisir un nombre mystère entre 0 et 65536
  unsigned short int n = rand() % (1 << 16);
  printf("nb mystere pour partie socket %d = %d\n", sock, n); // Afficher le nombre mystère choisi

  // Variables pour suivre les tentatives et la réponse du serveur
  unsigned short int guess; // Le nombre proposé par le joueur
  int taille = 0;           // Taille de la réponse du client
  int tentatives = 20;      // Nombre de tentatives autorisées
  int gagne = 0;            // Variable pour indiquer si le joueur a gagné
  char buff_in[100];        // Tampon pour stocker la réponse du client

  // Boucle principale de la partie
  while ((taille = recv(sock, buff_in, 100, 0)) > 0)
  {                                                  // Attendre une réponse du client
    sscanf(buff_in, "%hu", &guess);                  // Convertir la réponse du client en un entier non signé court
    printf("Joueur courant a envoyé : %d\n", guess); // Afficher la réponse du client

    // Vérifier si la proposition est correcte et envoyer la réponse appropriée au client
    char reponse[20]; // Tampon pour stocker la réponse du serveur
    if (n < guess || n > guess)
    {               // Si la proposition est incorrecte
      tentatives--; // Réduire le nombre de tentatives restantes
    }
    if (tentatives == 0)                          // Si le joueur n'a plus de tentatives
      sprintf(reponse, "PERDU\n");                // Indiquer qu'il a perdu
    else if (n < guess)                           // Si la proposition est trop grande
      sprintf(reponse, "MOINS %d\n", tentatives); // Indiquer que le joueur doit proposer un nombre plus petit
    else if (n > guess)                           // Si la proposition est trop petite
      sprintf(reponse, "PLUS %d\n", tentatives);  // Indiquer que le joueur doit proposer un nombre plus grand
    else
    {                              // Si la proposition est correcte
      sprintf(reponse, "GAGNE\n"); // Indiquer que le joueur a gagné
      gagne = 1;                   // Définir la variable pour indiquer que le joueur a gagné
    }

    send(sock, reponse, strlen(reponse), 0); // Envoyer la réponse au client
    if (gagne || !tentatives)
      break; // Si le joueur a gagné ou n'a plus de tentatives, sortir de la boucle
  }

  printf("Fin de partie\n"); // Afficher un

  // ferme la connexion avec le joueur
  close(sock);
}

// fonction pour lancer le serveur 1 player

int server_1p()
{
  // création du socket serveur
  int serv_sock = socket(PF_INET, SOCK_STREAM, 0);

  // création de l'adresse du serveur
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(7979);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // liaison du socket à l'adresse
  int r = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (r != 0)
  {
    fprintf(stderr, "Échec de bind");
    exit(-1);
  }

  // mise en écoute du socket pour les connexions entrantes
  r = listen(serv_sock, 0);
  if (r != 0)
  {
    fprintf(stderr, "Échec de listen");
    exit(-1);
  }

  // boucle infinie pour gérer les connexions des clients
  int client_sock;
  while (1)
  {
    // attend une connexion entrante
    client_sock = accept(serv_sock, NULL, NULL);
    if (client_sock < 0)
    {
      fprintf(stderr, "Échec de accept");
      exit(-1);
    }
    else
    {
      // affiche un message pour indiquer qu'une nouvelle connexion est établie
      printf("Connexion acceptee, nouvelle partie lancee.\n");

      // lance la partie pour le client connecté
      game_1p(client_sock);

      // ferme la connexion avec le client
      close(client_sock);
    }
  }

  // ferme le socket serveur
  close(serv_sock);

  // retourne 0 pour indiquer que le serveur s'est arrêté avec succès
  return 0;
}

// fonction où vous créez un nouveau processus par client

int server_1p_fork()
{

  int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(7979);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  int r = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (r != 0)
  {
    fprintf(stderr, "Échec de bind");
    exit(-1);
  }
  r = listen(serv_sock, 0);
  if (r != 0)
  {
    fprintf(stderr, "Échec de listen");
    exit(-1);
  }

  int client_sock = 0;

  while (1)
  {
    client_sock = accept(serv_sock, NULL, NULL);
    if (client_sock < 0)
    {
      fprintf(stderr, "Échec de accept");
      exit(-1);
    }
    else
    {
      printf("Connexion acceptee, nouvelle partie lancee.\n");
      int pid = fork();
      if (pid == 0)
      {
        game_1p(client_sock);
        close(client_sock);
        exit(0);
      }
      else if (pid > 0)
      {
        close(client_sock);
      }
      else
      {
        fprintf(stderr, "Échec de fork");
        exit(-1);
      }
    }
  }
  close(serv_sock);
  return 0;
}

// fonction où vous utilisez des threads pour gérer les clients

void *game_1p_point(void *arg)
{
  int *joueurs = (int *)arg;
  game_1p(*joueurs);
  return NULL;
}

int server_1p_thread()
{
  int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(7878);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  int r = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (r != 0)
  {
    fprintf(stderr, "Échec de bind");
    exit(-1);
  }
  r = listen(serv_sock, 0);
  if (r != 0)
  {
    fprintf(stderr, "Échec de listen");
    exit(-1);
  }

  int client_sock = 0;

  while (1)
  {
    client_sock = accept(serv_sock, NULL, NULL);
    if (client_sock < 0)
    {
      fprintf(stderr, "Échec de accept");
      exit(-1);
    }
    else
    {
      printf("Connexion acceptee, nouvelle partie lancee.\n");
      pthread_t thread;
      int *sock = malloc(sizeof(int));
      *sock = client_sock;
      int a = pthread_create(&thread, NULL, game_1p_point, sock);
      if (a != 0)
      {
        fprintf(stderr, "Échec de pthread_create");
        exit(-1);
      }
    }
  }
  close(serv_sock);
  return 0;
}

// Fonction pour gérer une partie pour deux joueurs

void game_2p(int client_sock, int client_sock2)
{
  srand(time(NULL) + client_sock + client_sock2);
  unsigned short int n1 = rand() % (1 << 16);
  unsigned short int n2 = rand() % (1 << 16);
  printf("nb mystere pour partie du sock %d = %d\n", client_sock, n1);
  printf("nb mystere pour partie du sock %d = %d\n", client_sock2, n2);
  unsigned short int guess1 = 0;
  unsigned short int guess2 = 0;
  int taille1 = 0;
  int taille2 = 0;
  int tentatives1 = 10;
  int tentatives2 = 10;
  int gagne1 = 0;
  int gagne2 = 0;
  char buff_in1[100];
  char buff_in2[100];
  while ((taille1 = recv(client_sock, buff_in1, 100, 0)) > 0 && (taille2 = recv(client_sock2, buff_in2, 100, 0)) > 0 && tentatives1 > 0 && (gagne1 == 0 || gagne2 == 0))
  {
    char reponse1[20];
    char reponse2[20];
    // if (memcmp(buff_in1, "\x03", 1) == 0)
    // {
    //   sprintf(reponse2, "Gagné\n");
    //   break;
    // }
    // if (memcmp(buff_in2, "\x03", 1) == 0)
    // {
    //   sprintf(reponse1, "Gagné\n");
    //   break;
    // }
    guess1 = atoi(buff_in1);
    guess2 = atoi(buff_in2);
    printf("Joueur courant a envoyé : %d\n", guess1);
    printf("Joueur courant a envoyé : %d\n", guess2);
    if (n1 < guess1 || n1 > guess1)
    {
      tentatives1--;
    }
    if (n2 < guess2 || n2 > guess2)
    {
      tentatives2--;
    }
    if (n1 < guess1 && n2 > guess2)
    {
      sprintf(reponse1, "Trop grand %d\n", tentatives1);
      sprintf(reponse2, "Trop petit %d\n", tentatives2);
    }
    else if (n1 < guess1 && n2 < guess2)
    {
      sprintf(reponse1, "Trop grand %d\n", tentatives1);
      sprintf(reponse2, "Trop grand %d\n", tentatives2);
    }
    else if (n1 > guess1 && n2 > guess2)
    {
      sprintf(reponse1, "Trop petit %d\n", tentatives1);
      sprintf(reponse2, "Trop grand %d\n", tentatives2);
    }
    else if (n1 > guess1 && n2 < guess2)
    {
      sprintf(reponse1, "Trop grand %d\n", tentatives1);
      sprintf(reponse2, "Trop petit %d\n", tentatives2);
    }
    else if (n1 == guess1 && n2 != guess2)
    {
      gagne1 = 1;
      sprintf(reponse1, "Gagné\n");
      sprintf(reponse2, "Perdu\n");
    }
    else if (n1 != guess1 && n2 == guess2)
    {
      gagne2 = 1;
      sprintf(reponse1, "Perdu\n");
      sprintf(reponse2, "Gagné\n");
    }
    else if (n1 == guess1 && n2 == guess2)
    {
      gagne1 = 1;
      gagne2 = 1;
      sprintf(reponse1, "Match nul\n");
      sprintf(reponse2, "Match nul\n");
    }
    send(client_sock, reponse1, strlen(reponse1), 0);
    send(client_sock2, reponse2, strlen(reponse2), 0);
    if (gagne1 == 1 || gagne2 == 1)
    {
      break;
    }
  }
  printf("Fin de partie\n");
  close(client_sock);
  close(client_sock2);
}

// Fonction qui permet à deux joueurs de jouer une partie en même temps

void *game_2p_point(void *arg)
{
  int *joueurs = (int *)arg;
  int *joueurs2 = (int *)(arg + sizeof(int));
  game_2p(*joueurs, *joueurs2);
  return NULL;
}

int serveur_2p()
{
  // Serveur IPv6
  
  /*
  int serv_sock = socket(PF_INET6, SOCK_STREAM, 0);
struct sockaddr_in6 serv_addr;
memset(&serv_addr, 0, sizeof(serv_addr));
serv_addr.sin6_family = AF_INET6;
serv_addr.sin6_port = htons(7878);
serv_addr.sin6_addr = in6addr_any;
int r = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
if (r != 0)
{
  fprintf(stderr, "Échec de bind");
  exit(-1);
}
r = listen(serv_sock, 0);
if (r != 0)
{
  fprintf(stderr, "Échec de listen");
  exit(-1);
}
  */
  int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(7878);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  int r = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (r != 0)
  {
    fprintf(stderr, "Échec de bind");
    exit(-1);
  }
  r = listen(serv_sock, 0);
  if (r != 0)
  {
    fprintf(stderr, "Échec de listen");
    exit(-1);
  }

  int client_sock = 0;
  int client_sock2 = 0;

  while (1)
  {
    client_sock = accept(serv_sock, NULL, NULL);
    client_sock2 = accept(serv_sock, NULL, NULL);
    if (client_sock < 0 || client_sock2 < 0)
    {
      fprintf(stderr, "Échec de accept");
      exit(-1);
    }
    else
    {
      printf("Connexion acceptee, nouvelle partie lancee.\n");
      int clients[2];
      clients[0] = client_sock;
      clients[1] = client_sock2;
      pthread_t thread;
      int a = pthread_create(&thread, NULL, game_2p_point, (void *)clients);
      if (a != 0)
      {
        fprintf(stderr, "Échec de pthread_create");
        exit(-1);
      }
    }
  }
  close(serv_sock);
  return 0;
}