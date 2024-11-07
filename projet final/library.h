#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h> // pour pouvoir utiliser la fonction isdigit


//========================= STRUCTURES =========================//

// Structure représentant un nœud


typedef struct n
{

    int id; // Identifiant du nœud
    struct n **links; // Liens vers d'autres nœuds
    int link_count; // Nombre de liens pour faciliter l'affichage
} Node;


//============================================================================//


typedef enum
{

    FILE_NOT_FOUND = 1,
    NO_START_NODE = 2,
    NO_END_NODE = 3,
    NO_VALID_PATH = 4,
    BAD_FILE_FORMAT = 5,
} Error;


//============================================================================//


typedef struct
{

    Node **nodes; // Tableau des nœuds du chemin
    int length; // Longueur du chemin
} Path;


//========================= PROTOTYPES DES FONCTIONS =========================//


int check_file_format(const char *file_path);

int read_file(char *filename); /* Ouvre le fichier et récupère les nœuds de
                               départ et de fin */

void initialise_node(FILE *file); // Initialise les nœuds à partir du fichier

int count_links(char *filename); // Compte le nombre de liens

void initialize_links(char *filename); // Initialise les liens

void print_nodes(); // Affiche les nœuds

Node *get_node_by_id(Node **nodes, int size, int id); /* Obtient un nœud par
                                                      son identifiant */

void print_links(); // Affiche les liens

void detect_isolated_nodes(); // Détecte les nœuds isolés

void display_nodes(Node *start); /* Affiche les nœuds à partir d'un nœud de
                                 départ */

void free_nodes(); // Libère la mémoire allouée pour les nœuds

void printErrorMessage(int result);

/* Algorithme de pathfinding */
Path *findPath(int start_id, int end_id, Node *nodes[], int node_count); 

Node **get_unconnected_nodes(Node **nodes, int size, Node *head);

void display_unconnected_nodes(Node **unconnected_nodes, int count);


//============================================================================//


extern Node *nodes[100]; // Tableau pour stocker les nœuds
extern int node_count; // Compteur de nœuds
extern int id_start_node; // Identifiant du nœud de départ
extern int id_end_node; // Identifiant du nœud d'arrivée
extern int node_unconnected;