//======================== LIBRAIRIE H ================================//


#include "library.h"


//======================== Variables globales ================================//


Node *nodes[100]; /* Tableau pour stocker les nœuds (taille arbitraire parce
que elle doit etre changer avec le reacloc dans la function qui vas cree les
lien)*/

int node_count = 0; // Compteur de nœuds
int id_start_node; // Identifiant du nœud de départ
int id_end_node;
int node_unconnected;


//=============================FONCTIONS======================================//


//============================================================================//

// Fonction pour lire le fichier et récupérer les nœuds de départ et de fin //


int read_file(char *filename)
{
    char buffer[256];
    int start_node = -1, end_node = -1; /* on initialise a -1 parceque  on ne
sait pas encore si il y a un node qui est le start ou le end qui a pour chiffre
0. mais le code marche aussi si on initialize a 0 . mais je préfaire -1 parce
que ca vas nous aider pour la fonction de  gestion des erreurs*/

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {

        return FILE_NOT_FOUND;
    }

    // Lire le fichier ligne par ligne
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {

        buffer[strcspn(buffer, "\n")] = 0;

        // Vérifie si la ligne contient #start
        if (strcmp(buffer, "#start") == 0)
        {

            if (fgets(buffer, sizeof(buffer), file) != NULL)
            {
                
                // Récupérer la valeur du nœud de départ
                start_node = atoi(buffer);
                id_start_node = start_node;
            }
            continue;
        }

        // Vérifie si la ligne contient #end
        if (strcmp(buffer, "#end") == 0)
        {

            if (fgets(buffer, sizeof(buffer), file) != NULL)
            {

                end_node = atoi(buffer); // Récupérer la valeur du nœud de fin
                id_end_node = end_node;
            }
            continue;
        }
    }

    fclose(file); // Fermeture du fichierc

    if (start_node == -1)
    {

        return NO_START_NODE;
    }
    if (end_node == -1)
    {

        return NO_END_NODE;
    }

    // Affiche les nœuds de départ et de fin
    printf("Start: %d\nEnd: %d\n", start_node, end_node);
    return 0;
}


//============================================================================//

// Fonction pour initialiser les nœuds à partir du fichier


void initialise_node(FILE *file)
{

    char buffer[256];

    // Lire les nœuds jusqu'à atteindre la section #links
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {

        buffer[strcspn(buffer, "\n")] = 0;

        // Quitter si on atteint #links
        if (strcmp(buffer, "#links") == 0)
        {

            return; // Terminer la fonction si on atteint #links
        }

        // Ignorer les lignes de section
        if (strcmp(buffer, "#nodes") == 0 || strcmp(buffer, "#start") == 0 || 
        strcmp(buffer, "#end") == 0)

        {

            continue; // Passer à la prochaine ligne
        }

        // Vérifier si la ligne contient un chiffre
        if (isdigit(buffer[0]))
        {

            // Créer un nouveau nœud
            Node *new_node = malloc(sizeof(Node));
            if (new_node == NULL)
            {

                printf("Erreur d'allocation de mémoire pour un nouveau nœud!\n");
                return;
            }

            new_node->id = atoi(buffer); // Récupérer l'identifiant du nœud
            new_node->links = NULL; // Initialiser les liens à NULL
            new_node->link_count = 0; /*Les deux dernières lignes serviront
                            quand il faudra calculer le chemin de pathfinding*/

            // Ajouter le nœud au tableau
            nodes[node_count++] = new_node; // Stocker le nouveau nœud
        }
    }
}


//============================================================================//

// Fonction pour libérer la mémoire allouée pour les nœuds


void free_nodes()
{

    for (int i = 0; i < node_count; i++)
    {

        if (nodes[i] != NULL)
        {

            free(nodes[i]->links); // Libère les liens associés au nœud
            free(nodes[i]); // Libère le nœud lui-même
            nodes[i] = NULL; // Évite de libérer à nouveau ce nœud
        }
    }
}


//============================================================================//

// Fonction pour afficher les nœuds


void print_nodes()
{

    printf("Liste des nœuds:\n");
    for (int i = 0; i < node_count; i++)
    {

        printf("Nœud ID: %d\n", nodes[i]->id);
    }
}


//============================================================================//

// Fonction pour compter les liens


int count_links(char *filename)
{

    char buffer[256];
    int link_count = 0;

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {

        return FILE_NOT_FOUND;
    }

    // Lire le fichier ligne par ligne
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {

        // Commencer à compter uniquement après avoir trouvé #links
        if (strcmp(buffer, "#links\n") == 0)
        {

            while (fgets(buffer, sizeof(buffer), file) != NULL)
            {

                link_count++; // Incrémenter le compteur de liens
            }
            break; // Sortir de la boucle après avoir compté
        }
    }

    fclose(file);
    printf("Nombre de lien(s): %d\n", link_count);
    return 0;
}


//============================================================================//

// Fonction pour initialiser les liens


void initialize_links(char *filename)
{

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {

        printf("Erreur lors de l'ouverture du fichier pour les liens!\n");
        return;
    }

    char buffer[256];
    int links_section_found = 0;

    // Lire le fichier jusqu'à trouver la section #links
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {

        // Chercher la section #links
        if (strcmp(buffer, "#links\n") == 0)
        {

            links_section_found = 1; /* Marquer que la section des liens
                                     est trouvée*/

            continue; // Passer à la ligne suivante
        }

        // Si nous sommes dans la section des liens, traiter les liens
        if (links_section_found)
        {

            // Enlever le saut de ligne
            buffer[strcspn(buffer, "\n")] = 0; // Remove newline

            int id1, id2;
            if (sscanf(buffer, "%d-%d", &id1, &id2) == 2)
            {
                
                Node *node1 = NULL; // Variable pour le nœud 1
                Node *node2 = NULL; // Variable pour le nœud 2

                // Chercher les nœuds correspondants
                for (int i = 0; i < node_count; i++)
                {

                    if (nodes[i]->id == id1)
                    {

                        node1 = nodes[i]; // Trouver le nœud 1
                    }

                    if (nodes[i]->id == id2)
                    {

                        node2 = nodes[i]; // Trouver le nœud 2
                    }
                }

             // Si les deux nœuds sont trouvés, créer des liens bidirectionnels
                if (node1 && node2)
                {

                    // Réallocation de la mémoire pour les liens
                    Node **new_links1 = realloc(node1->links, (node1->link_count
                    + 1) * sizeof(Node *));

                    Node **new_links2 = realloc(node2->links, (node2->link_count
                    + 1) * sizeof(Node *));

                    // Vérifier l'allocation
                    if (new_links1 == NULL || new_links2 == NULL)
                    {

                        printf("Erreur d'allocation de mémoire pour les \
                        liens!\n");

                        fclose(file);
                        return; // Sortir en cas d'erreur d'allocation
                    }

                    // Affecter les nouvelles allocations
                    node1->links = new_links1;
                    node2->links = new_links2;

                    // Lien de node1 à node2
                    node1->links[node1->link_count] = node2;
                    // Lien de node2 à node1
                    node2->links[node2->link_count] = node1;

                    // Incrémenter le compteur de liens
                    node1->link_count++;
                    node2->link_count++;
                }
                else
                {

                    printf("Erreur: un ou plusieurs nœuds non trouvés pour \
                    les IDs: %d, %d\n", id1, id2);
                }
            }
        }
    }

    fclose(file);
}


//============================================================================//

// Fonction pour afficher les liens


void print_links()
{

    printf("=== Liens entre les nœuds ===\n");
    for (int i = 0; i < node_count; i++)
    {
        
        Node *node = nodes[i];
        printf("Nœud %d est lié à: ", node->id);

        if (node->link_count > 0)
        {

            for (int j = 0; j < node->link_count; j++)
            {

                printf("%d", node->links[j]->id); // Afficher l'ID du nœud lié

                if (j < node->link_count - 1)
                {
                    
                    printf(", "); // Ajouter une virgule entre les liens
                }
            }
        }

        else
        {

            printf("Aucun lien"); // Aucun lien trouvé
        }

        printf("\n");
    }
}


//============================================================================//

// Fonction pour obtenir un nœud par son identifiant


Node *get_node_by_id(Node **nodes, int size, int id)
{

    for (int i = 0; i < size; i++)
    {

        if (nodes[i]->id == id)
        {

            return nodes[i]; // Retourne le nœud trouvé
        }
    }
    return NULL; // Retourne NULL si non trouvé
}


//============================================================================//

// Fonction pour afficher les nœuds à partir d'un nœud de départ


void display_nodes(Node *start)
{

    if (start == NULL)
    {

        printf("Nœud de départ nul.\n");
        return;
    }
    printf("Nœud en ordre de lien: ");

    int visited[100] = {0}; // Tableau pour marquer les nœuds visités

    // Fonction récursive DFS pour afficher les nœuds
    void dfs(Node * current)
    {

        if (current == NULL || visited[current->id])
        {

            return; // Si le nœud est NULL ou déjà visité, sortir
        }
        visited[current->id] = 1; // Marquer comme visité
        printf("%d ", current->id); // Afficher l'ID du nœud
        for (int i = 0; i < current->link_count; i++)
        {

            dfs(current->links[i]); // Appel récursif pour chaque lien
        }
    }

    dfs(start); // Démarrer la recherche depuis le nœud de départ
    printf("\n");
}


//============================================================================//

// Fonction pour détecter les nœuds isolés


void detect_isolated_nodes()
{

    for (int i = 0; i < node_count; i++)
    {

        if (nodes[i]->link_count == 0)
        {

            // Afficher l'ID du nœud isolé
            printf("Le nœud %d n'a pas de lien\n", nodes[i]->id); 
        }
    }
}


//============================================================================//

// fonction qui gère les messages d'erreur


void printErrorMessage(int result)
{
    switch (result)
    {

    case FILE_NOT_FOUND:
        printf("Erreur : Lors de l'ouverture du fichier !\n");
        break;

    case NO_START_NODE:
        printf("Erreur : Aucun nœud de départ spécifié !\n");
        break;

    case NO_END_NODE:
        printf("Erreur : Aucun nœud de fin spécifié !\n");
        break;

    case NO_VALID_PATH:
        printf("Erreur : Aucun chemin valide trouvé !\n");
        break;

    case BAD_FILE_FORMAT:
        printf("Erreur : Le fichier texte est dans un mauvais format !\n");
        break;
    }
}


//============================================================================//

// fonction qui vérifie le format du fichier

int check_file_format(const char *file_path)
{

    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {

        perror("Error opening file");
        return -1; // Erreur lors de l'ouverture du fichier
    }

    char line[256];
    int has_links = 0;
    int has_nodes = 0;

    while (fgets(line, sizeof(line), file))
    {

        if (strstr(line, "#links"))
        {

            has_links = 1;
        }
        if (strstr(line, "#nodes"))
        {

            has_nodes = 1;
        }
    }

    fclose(file);

    if (!has_links || !has_nodes)
    {

        return BAD_FILE_FORMAT; // Format de fichier incorrect
    }

    return 0; // Format de fichier valide
}


//============================================================================//

/*Fonction pour marquer les nœuds visités en utilisant la recherche en
profondeur (DFS)*/


void mark_visited(Node *current, int *visited)
{

    if (current == NULL || visited[current->id])
    {

        return; // Si le nœud est NULL ou déjà visité, sortir
    }

    visited[current->id] = 1; // Marquer le nœud comme visité

    // Appel récursif pour chaque nœud lié
    for (int i = 0; i < current->link_count; i++)
    {

        mark_visited(current->links[i], visited);
    }
}


//============================================================================//

// Fonction pour obtenir les nœuds non connectés


Node **get_unconnected_nodes(Node **nodes, int size, Node *head)
{

    // Allouer de la mémoire pour suivre les nœuds visités
    int *visited = calloc(size, sizeof(int));
    if (visited == NULL)
    {

        printf("Erreur d'allocation de mémoire pour le tableau des nœuds \
        visités!\n");

        return NULL; // Gérer l'erreur d'allocation
    }

    // Marquer les nœuds visités à partir du nœud de départ
    mark_visited(head, visited);

    // Compter les nœuds non connectés
    int unconnected_count = 0;
    for (int i = 0; i < size; i++)
    {

        if (visited[nodes[i]->id] == 0)
        {

            unconnected_count++;
        }
    }

    // Allouer de la mémoire pour les nœuds non connectés
    Node **unconnected_nodes = malloc(unconnected_count * sizeof(Node *));

    if (unconnected_nodes == NULL)
    {

        printf("Erreur d'allocation de mémoire pour le tableau des nœuds non \
        connectés!\n");

        free(visited);
        return NULL; // Gérer l'erreur d'allocation
    }

    // Remplir le tableau avec les nœuds non connectés
    int index = 0;

    for (int i = 0; i < size; i++)
    {

        if (visited[nodes[i]->id] == 0)
        {

            unconnected_nodes[index++] = nodes[i];
        }
    }

    node_unconnected = unconnected_count;

    free(visited); // Libérer la mémoire du tableau des nœuds visités
    return unconnected_nodes; // Retourner le tableau des nœuds non connectés
}


//============================================================================//

// fonction qui montre les noeuds qui n'ont pas de lien

void display_unconnected_nodes(Node **unconnected_nodes, int count)
{

    printf("contenue de la liste de node sans lien\n");
    for (int i = 0; i < count; i++)
    {

        if (unconnected_nodes[i] != NULL)
        {

            printf("Node ID: %d\n", unconnected_nodes[i] -> id);
        }
    }
}


//============================================================================//

/* Fonction pour trouver le chemin le plus court entre deux nœuds */


Path *findPath(int start_id, int end_id, Node *nodes[], int node_count)
{

    int *visited = (int *)calloc(node_count, sizeof(int));
    Node **parent = (Node **)malloc(node_count * sizeof(Node *));

    Node *start = NULL;
    Node *end = NULL;

    // Recherche des nœuds de départ et d'arrivée dans le tableau
    for (int i = 0; i < node_count; i++)
    {

        if (nodes[i]->id == start_id)
        {
            
            start = nodes[i];
        }

        if (nodes[i]->id == end_id)
        {

            end = nodes[i];
        }
    }

    if (!start || !end)
    {

        free(visited);
        free(parent);
        return NULL; // Retourne NULL si l'un des nœuds n'est pas trouvé
    }

    Node **queue = (Node **)malloc(node_count * sizeof(Node *));
    int front = 0, rear = 0;

    queue[rear++] = start;
    visited[start_id] = 1;
    parent[start_id] = NULL;

    while (front < rear)
    {

        Node *current = queue[front++];

        if (current == end)
        {

            break; // On a trouvé le nœud d'arrivée
        }

        for (int i = 0; i < current->link_count; i++)
        {

            Node *next = current->links[i];
            if (!visited[next->id])
            {

                visited[next->id] = 1;
                parent[next->id] = current;
                queue[rear++] = next;
            }
        }
    }

    // Construction du chemin
    Path *path = (Path *)malloc(sizeof(Path));
    path->nodes = (Node **)malloc(node_count * sizeof(Node *));
    path->length = 0;

    for (Node *current = end; current != NULL; current = parent[current->id])
    {

        path->nodes[path->length++] = current;
    }

    // Inverser le chemin pour l'avoir dans l'ordre du départ à l'arrivée
    for (int i = 0; i < path->length / 2; i++)
    {

        Node *temp = path->nodes[i];
        path->nodes[i] = path->nodes[path->length - 1 - i];
        path->nodes[path->length - 1 - i] = temp;
    }

    free(visited);
    free(parent);
    free(queue);

    return path; // Retourne le chemin trouvé
}