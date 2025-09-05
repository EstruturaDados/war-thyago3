#include <stdio.h>
#include <string.h>

#define NUM_TERRITORIOS 5
#define TAM_STRING 50

// Definição da struct
struct Territorio {
    char nome[TAM_STRING];
    char corExercito[TAM_STRING];
    int tropas;
};

int main() {
    struct Territorio territorios[NUM_TERRITORIOS];

    printf("=== Cadastro de Territorios ===\n");

    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        printf("\n--- Territorio %d ---\n", i + 1);

        printf("Nome: ");
        fgets(territorios[i].nome, TAM_STRING, stdin);
        territorios[i].nome[strcspn(territorios[i].nome, "\n")] = 0;

        printf("Cor do exercito: ");
        fgets(territorios[i].corExercito, TAM_STRING, stdin);
        territorios[i].corExercito[strcspn(territorios[i].corExercito, "\n")] = 0;

        printf("Numero de tropas: ");
        scanf("%d", &territorios[i].tropas);
        getchar(); // limpa o \n
    }

    printf("\n=== Estado Atual do Mapa ===\n");
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        printf("\nTerritorio %d\n", i + 1);
        printf("Nome: %s\n", territorios[i].nome);
        printf("Cor: %s\n", territorios[i].corExercito);
        printf("Tropas: %d\n", territorios[i].tropas);
    }

    return 0;
}
