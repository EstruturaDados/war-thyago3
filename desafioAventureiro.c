// war_mestre.c
// Jogo WAR - Nível Mestre (versão interativa, 2-4 jogadores, mapa fixo, missões dinâmicas)
// Compilar: gcc war_mestre.c -o war_mestre
// Executar: ./war_mestre

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOME 30
#define MAX_COR 12
#define MAX_MISSOES 8
#define MAX_TERRITORIOS 12
#define MAX_JOGADORES 4
#define MIN_JOGADORES 2

typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

typedef struct {
    char nome[MAX_NOME];
    char cor[10];
    char *missao; // alocada dinamicamente
} Jogador;

/* Protótipos */
Territorio *criarMapa(int *tamanho);
Jogador *criarJogadores(int qtd, int *qtdCriado);
void distribuirTerritorios(Territorio *mapa, int tamanho, Jogador *jogadores, int qtdJogadores);
void atribuirMissao(char **destino, char *missoes[], int totalMissoes, const char *corPlayer);
void exibirMissao(const char *missao);
void exibirMapa(Territorio *mapa, int tamanho);
void atacar(Territorio *atacante, Territorio *defensor);
int verificarMissao(char *missao, Territorio *mapa, int tamanho, const char *corJogador);
void liberarMemoria(Territorio *mapa, int tamanho, Jogador *jogadores, int qtdJogadores);
int lerInteiroInterativo(int minimo, int maximo);
void limparBufferEntrada();

/* Implementações */

/* Limpa buffer */
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) ;
}

/* Leitura segura de inteiro com validade */
int lerInteiroInterativo(int minimo, int maximo) {
    int v;
    while (1) {
        if (scanf("%d", &v) != 1) {
            printf("Entrada invalida. Tente novamente: ");
            limparBufferEntrada();
            continue;
        }
        limparBufferEntrada();
        if (v < minimo || v > maximo) {
            printf("Valor deve ser entre %d e %d. Tente novamente: ", minimo, maximo);
            continue;
        }
        return v;
    }
}

/* Cria mapa fixo dinamicamente */
Territorio *criarMapa(int *tamanho) {
    int n = MAX_TERRITORIOS;
    Territorio *mapa = (Territorio *)calloc(n, sizeof(Territorio));
    if (!mapa) { perror("calloc mapa"); exit(1); }

    const char *nomes[12] = {
        "Amazonia","Pantanal","Cerrado","MataAtl","Pampa","Caatinga",
        "Serra","Litoral","Planalto","ValeSul","IlhaNorte","Depressao"
    };
    int tropasInit[12] = {5,4,4,3,5,3,4,5,3,4,3,2};

    for (int i = 0; i < n; i++) {
        strncpy(mapa[i].nome, nomes[i], sizeof(mapa[i].nome)-1);
        mapa[i].tropas = tropasInit[i];
        strcpy(mapa[i].cor, "Neutro"); // inicialmente neutro; depois distribuído
    }
    *tamanho = n;
    return mapa;
}

/* Cria jogadores dinamicamente e pede nomes; cores atribuídas automaticamente */
Jogador *criarJogadores(int qtd, int *qtdCriado) {
    if (qtd < MIN_JOGADORES || qtd > MAX_JOGADORES) return NULL;
    Jogador *jog = (Jogador *)malloc(sizeof(Jogador) * qtd);
    if (!jog) { perror("malloc jogadores"); exit(1); }

    const char *coresDisponiveis[MAX_JOGADORES] = {"Azul","Vermelha","Verde","Amarela"};

    for (int i = 0; i < qtd; i++) {
        printf("Digite o nome do jogador %d: ", i+1);
        if (!fgets(jog[i].nome, MAX_NOME, stdin)) { strcpy(jog[i].nome,"Jogador"); }
        // remove newline
        jog[i].nome[strcspn(jog[i].nome, "\n")] = '\0';
        if (strlen(jog[i].nome) == 0) snprintf(jog[i].nome, MAX_NOME, "Jogador %d", i+1);
        strncpy(jog[i].cor, coresDisponiveis[i], sizeof(jog[i].cor)-1);
        jog[i].missao = NULL;
    }
    *qtdCriado = qtd;
    return jog;
}

/* Distribui territórios inicialmente entre jogadores de forma circular */
void distribuirTerritorios(Territorio *mapa, int tamanho, Jogador *jogadores, int qtdJogadores) {
    if (!mapa || !jogadores || qtdJogadores <= 0) return;
    for (int i = 0; i < tamanho; i++) {
        int idx = i % qtdJogadores;
        strncpy(mapa[i].cor, jogadores[idx].cor, sizeof(mapa[i].cor)-1);
        // deixa as tropas conforme inicial definidas
    }
}

/* Atribui uma missão sorteada: aloca dinamicamente destino e copia (substitui token {COR}) */
void atribuirMissao(char **destino, char *missoes[], int totalMissoes, const char *corPlayer) {
    if (!destino || !missoes || totalMissoes <= 0) return;
    int idx = rand() % totalMissoes;
    char *modelo = missoes[idx];
    const char token[] = "{COR}";
    char *pos = strstr(modelo, token);
    if (pos) {
        size_t before = pos - modelo;
        size_t after = strlen(modelo) - before - strlen(token);
        size_t total = before + strlen(corPlayer) + after + 1;
        *destino = (char *)malloc(total);
        if (!*destino) { perror("malloc missao"); exit(1); }
        strncpy(*destino, modelo, before);
        (*destino)[before] = '\0';
        strcat(*destino, corPlayer);
        strcat(*destino, pos + strlen(token));
    } else {
        *destino = (char *)malloc(strlen(modelo) + 1);
        if (!*destino) { perror("malloc missao2"); exit(1); }
        strcpy(*destino, modelo);
    }
}

/* Exibe missão (passagem por valor) */
void exibirMissao(const char *missao) {
    if (!missao) { printf("Sem missao.\n"); return; }
    printf("Missao: %s\n", missao);
}

/* Exibe mapa */
void exibirMapa(Territorio *mapa, int tamanho) {
    printf("\n--- Mapa Atual ---\n");
    for (int i = 0; i < tamanho; i++) {
        printf("%2d) %-12s | Dono: %-8s | Tropas: %2d\n", i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("------------------\n");
}

/* Simula ataque: rolagem de 1-6 para atacante e defensor */
void atacar(Territorio *atacante, Territorio *defensor) {
    if (!atacante || !defensor) return;
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("Ataque invalido: mesmo dono.\n");
        return;
    }
    if (atacante->tropas <= 0) {
        printf("Ataque impossivel: atacante sem tropas.\n");
        return;
    }

    int dadoA = (rand() % 6) + 1;
    int dadoD = (rand() % 6) + 1;
    printf("Rolagem: Atacante(%s)=%d x Defensor(%s)=%d\n", atacante->cor, dadoA, defensor->cor, dadoD);

    if (dadoA > dadoD) {
        int transfer = atacante->tropas / 2;
        if (transfer < 1) transfer = 1;
        printf("Atacante vence! Transfere %d tropas e cor %s para %s.\n", transfer, atacante->cor, defensor->nome);
        defensor->tropas = transfer;
        strncpy(defensor->cor, atacante->cor, sizeof(defensor->cor)-1);
        atacante->tropas -= transfer;
        if (atacante->tropas < 0) atacante->tropas = 0;
    } else {
        atacante->tropas -= 1;
        if (atacante->tropas < 0) atacante->tropas = 0;
        printf("Defensor resiste! Atacante perde 1 tropa.\n");
    }
}

/* Verifica missão simples; retorna 1 se cumprida, 0 caso contrario.
   Implementa alguns padrões:
   - "Conquistar N territorios no total"
   - "Conquistar N territorios seguidos da cor {COR}" (ou sem cor)
   - "Eliminar todas as tropas da cor X"
   - "Controlar todos os territorios da cor X"
*/
int verificarMissao(char *missao, Territorio *mapa, int tamanho, const char *corJogador) {
    if (!missao || !mapa || tamanho <= 0) return 0;

    // 1) Conquistar N territorios no total
    if (strstr(missao, "Conquistar") && strstr(missao, "territorios no total")) {
        int alvo = 0;
        if (sscanf(missao, "Conquistar %d territorios no total", &alvo) == 1) {
            // contar territorios do jogador
            int cont = 0;
            for (int i = 0; i < tamanho; i++) {
                if (strcmp(mapa[i].cor, corJogador) == 0) cont++;
            }
            if (cont >= alvo) return 1;
        }
    }

    // 2) Conquistar N territorios seguidos da cor (pode vir com {COR} substituido)
    if (strstr(missao, "territorios seguidos")) {
        int alvo = 0;
        char cor[20] = "";
        if (sscanf(missao, "Conquistar %d territorios seguidos da cor %19s", &alvo, cor) >= 1) {
            if (alvo <= 0) alvo = 3;
            // se cor é o nome do jogador (por token) ou uma cor especifica:
            for (int i = 0; i <= tamanho - alvo; i++) {
                int ok = 1;
                for (int j = 0; j < alvo; j++) {
                    if (strcmp(mapa[i+j].cor, cor) != 0) { ok = 0; break; }
                }
                if (ok) return 1;
            }
        }
    }

    // 3) Eliminar todas as tropas da cor X
    if (strstr(missao, "Eliminar todas as tropas da cor")) {
        char alvoCor[20];
        if (sscanf(missao, "Eliminar todas as tropas da cor %19s", alvoCor) == 1) {
            for (int i = 0; i < tamanho; i++) {
                if (strcmp(mapa[i].cor, alvoCor) == 0 && mapa[i].tropas > 0) return 0;
            }
            return 1;
        }
    }

    // 4) Controlar todos os territorios da cor X
    if (strstr(missao, "Controlar todos os territorios da cor")) {
        char alvoCor[20];
        if (sscanf(missao, "Controlar todos os territorios da cor %19s", alvoCor) == 1) {
            for (int i = 0; i < tamanho; i++) {
                if (strcmp(mapa[i].cor, alvoCor) != 0) return 0;
            }
            return 1;
        }
    }

    return 0;
}

/* Libera memoria de mapa e missões dos jogadores */
void liberarMemoria(Territorio *mapa, int tamanho, Jogador *jogadores, int qtdJogadores) {
    if (mapa) free(mapa);
    if (jogadores) {
        for (int i = 0; i < qtdJogadores; i++) {
            if (jogadores[i].missao) free(jogadores[i].missao);
        }
        free(jogadores);
    }
}

/* ---------------------- MAIN (fluxo interativo) ---------------------- */
int main() {
    srand((unsigned)time(NULL));

    // Missões modelo (algumas com token {COR})
    char *missoesModelo[MAX_MISSOES] = {
        "Conquistar 3 territorios seguidos da cor {COR}",
        "Eliminar todas as tropas da cor Vermelha",
        "Conquistar 5 territorios no total",
        "Conquistar 4 territorios seguidos da cor {COR}",
        "Controlar todos os territorios da cor Amarela",
        "Conquistar 6 territorios no total",
        "Eliminar todas as tropas da cor Verde",
        "Conquistar 2 territorios seguidos da cor {COR}"
    };

    printf("=== WAR - Nível Mestre (interativo) ===\n");
    printf("Quantidade de jogadores (2 a 4): ");
    int qtd = lerInteiroInterativo(MIN_JOGADORES, MAX_JOGADORES);

    int qtdJogadores = 0;
    Jogador *jogadores = criarJogadores(qtd, &qtdJogadores);

    // criar mapa e distribuir
    int tamanhoMapa = 0;
    Territorio *mapa = criarMapa(&tamanhoMapa);
    distribuirTerritorios(mapa, tamanhoMapa, jogadores, qtdJogadores);

    // atribuir missões
    for (int i = 0; i < qtdJogadores; i++) {
        atribuirMissao(&jogadores[i].missao, missoesModelo, MAX_MISSOES, jogadores[i].cor);
    }

    // exibir missões uma vez
    printf("\n--- Missoes iniciais (exibidas uma vez) ---\n");
    for (int i = 0; i < qtdJogadores; i++) {
        printf("%s (%s): ", jogadores[i].nome, jogadores[i].cor);
        exibirMissao(jogadores[i].missao);
    }

    int turno = 0;
    int vencedor = -1;
    int maxTurnos = 500;

    while (turno < maxTurnos) {
        int atual = turno % qtdJogadores;
        printf("\n=== Turno %d - %s (%s) ===\n", turno+1, jogadores[atual].nome, jogadores[atual].cor);
        int opc = 0;
        while (1) {
            printf("\nMenu:\n1) Exibir mapa\n2) Atacar\n3) Mostrar minha missao\n4) Finalizar turno\nEscolha: ");
            opc = lerInteiroInterativo(1,4);
            if (opc == 1) {
                exibirMapa(mapa, tamanhoMapa);
            } else if (opc == 2) {
                exibirMapa(mapa, tamanhoMapa);
                // escolher atacante (territorio do jogador com tropas)
                printf("Escolha o indice do seu territorio atacante: ");
                int idxAt = lerInteiroInterativo(0, tamanhoMapa-1);
                if (strcmp(mapa[idxAt].cor, jogadores[atual].cor) != 0) {
                    printf("Territorio nao eh seu. Tente novamente.\n");
                    continue;
                }
                if (mapa[idxAt].tropas <= 0) {
                    printf("Territorio sem tropas. Tente outro.\n");
                    continue;
                }
                printf("Escolha o indice do territorio defensor: ");
                int idxDef = lerInteiroInterativo(0, tamanhoMapa-1);
                if (strcmp(mapa[idxDef].cor, jogadores[atual].cor) == 0) {
                    printf("Nao pode atacar seu proprio territorio.\n");
                    continue;
                }
                // executar ataque
                atacar(&mapa[idxAt], &mapa[idxDef]);
            } else if (opc == 3) {
                printf("Sua missao: ");
                exibirMissao(jogadores[atual].missao);
            } else if (opc == 4) {
                break;
            }
        }

        // verificar missao silenciosamente para todos os jogadores (pos-turno)
        for (int j = 0; j < qtdJogadores; j++) {
            if (verificarMissao(jogadores[j].missao, mapa, tamanhoMapa, jogadores[j].cor)) {
                vencedor = j;
                break;
            }
        }
        if (vencedor != -1) break;

        turno++;
    }

    if (vencedor != -1) {
        printf("\n*** VENCEDOR: %s (missao cumprida) ***\n", jogadores[vencedor].nome);
        printf("Missao: %s\n", jogadores[vencedor].missao);
    } else {
        printf("\nFim de jogo: nenhum jogador cumpriu a missao em %d turnos.\n", maxTurnos);
    }

    liberarMemoria(mapa, tamanhoMapa, jogadores, qtdJogadores);
    return 0;
}
