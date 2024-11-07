#include "library.h"

int main(int argc, char *argv[])
{

    Node *found_node;

    char *filename = argv[1];

    int errorformat = check_file_format(filename);
    printErrorMessage(errorformat);

    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {

        printf("Erreur lors de l'ouverture du fichier\n");
        return 1;
    }

    initialise_node(file);
    fclose(file);

    printf("Nombre de nœud(s): %d\n", node_count);

    count_links(filename);
    initialize_links(filename);

    int error1 = read_file(filename);
    printErrorMessage(error1);

    found_node = get_node_by_id(nodes, node_count, id_start_node);

    if (found_node != NULL)
    {

        printf("Nœud trouvé: ID = %d\n", found_node->id);
    }

    else
    {

        printf("Nœud avec ID %d non trouvé.\n", id_start_node);
    }

    Path *path = findPath(id_start_node, id_end_node, nodes, node_count);

    if (path)
    {

        printf("Chemin trouvé : ");

        for (int i = 0; i < path->length; i++)
        {

            printf("%d ", path->nodes[i]->id);
        }

        printf("\n");
        free(path->nodes); // Libération du tableau de nœuds dans le chemin
        free(path); // Libération de la structure path elle-même
    }

    else
    {

        printf("Aucun chemin trouvé.\n");
    }

    // Libérer la mémoire de chaque lien de chaque nœud
    for (int i = 0; i < node_count; i++)
    {

        if (nodes[i] != NULL)
        {

            free(nodes[i]->links); // Libérer les liens du nœud
            free(nodes[i]); // Libérer le nœud lui-même
            nodes[i] = NULL; /* Éviter un double free en mettant le pointeur
                             à NULL*/
        }
    }

    free_nodes(); // Nettoyage final des nœuds (si nécessaire)
    return 0;
}