#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_ITENS 10
#define TAM_STR   50

/* ============================
   ESTRUTURAS E ENUMERAÇÕES
   ============================ */

/* Item da mochila */
typedef struct {
    char nome[TAM_STR];
    char tipo[TAM_STR];
    int quantidade;
    int prioridade; /* 1 a 5 (Nível Mestre) */
} Item;

/* Critérios de ordenação (Nível Mestre) */
typedef enum {
    ORDENAR_POR_NOME = 1,
    ORDENAR_POR_TIPO = 2,
    ORDENAR_POR_PRIORIDADE = 3
} CriterioOrdenacao;

/* ============================
   VARIÁVEIS GLOBAIS
   ============================ */

Item mochila[MAX_ITENS];
int qtdItens = 0;

/* indica se o vetor está ordenado por nome (para busca binária) */
bool ordenadoPorNome = false;

/* ============================
   FUNÇÕES AUXILIARES
   ============================ */

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* lê string com espaços (até \n) */
void lerString(const char *msg, char *dest, int tamanho) {
    printf("%s", msg);
    /* lê até tamanho-1 caracteres ou até \n */
    if (fgets(dest, tamanho, stdin) != NULL) {
        size_t len = strlen(dest);
        if (len > 0 && dest[len - 1] == '\n') {
            dest[len - 1] = '\0';
        }
    }
}

/* lê inteiro com mensagem */
int lerInt(const char *msg) {
    int valor;
    printf("%s", msg);
    scanf("%d", &valor);
    limparBuffer();
    return valor;
}

/* ============================
   NÍVEL NOVATO
   ============================ */

/* adicionar item */
void adicionarItem() {
    if (qtdItens >= MAX_ITENS) {
        printf("\nMochila cheia! Nao e possivel adicionar mais itens.\n");
        return;
    }

    Item novo;
    printf("\n=== Adicionar Item ===\n");
    lerString("Nome do item: ", novo.nome, TAM_STR);
    lerString("Tipo do item: ", novo.tipo, TAM_STR);
    novo.quantidade = lerInt("Quantidade: ");

    /* para Nível Mestre já pedimos prioridade */
    int prio = lerInt("Prioridade (1 a 5): ");
    if (prio < 1) prio = 1;
    if (prio > 5) prio = 5;
    novo.prioridade = prio;

    mochila[qtdItens] = novo;
    qtdItens++;

    /* depois de inserir, consideramos que a lista pode ter "quebrado"
       a ordenação por nome. */
    ordenadoPorNome = false;

    printf("Item adicionado com sucesso!\n");
}

/* remover item pelo nome */
void removerItem() {
    if (qtdItens == 0) {
        printf("\nMochila vazia. Nao ha itens para remover.\n");
        return;
    }

    char nomeBusca[TAM_STR];
    printf("\n=== Remover Item ===\n");
    lerString("Digite o nome do item que deseja remover: ", nomeBusca, TAM_STR);

    int i, pos = -1;
    for (i = 0; i < qtdItens; i++) {
        if (strcmp(mochila[i].nome, nomeBusca) == 0) {
            pos = i;
            break;
        }
    }

    if (pos == -1) {
        printf("Item nao encontrado.\n");
        return;
    }

    /* "puxa" todos os elementos seguintes uma posição para trás */
    for (i = pos; i < qtdItens - 1; i++) {
        mochila[i] = mochila[i + 1];
    }
    qtdItens--;

    printf("Item removido com sucesso.\n");
}

/* listar todos os itens */
void listarItens() {
    int i;

    printf("\n=== Itens na mochila (%d/%d) ===\n", qtdItens, MAX_ITENS);
    if (qtdItens == 0) {
        printf("Mochila vazia.\n");
        return;
    }

    printf("%-20s | %-15s | %-10s | %-10s\n", "Nome", "Tipo", "Quantidade", "Prioridade");
    printf("-----------------------------------------------------------------\n");
    for (i = 0; i < qtdItens; i++) {
        printf("%-20s | %-15s | %-10d | %-10d\n",
               mochila[i].nome,
               mochila[i].tipo,
               mochila[i].quantidade,
               mochila[i].prioridade);
    }
}

/* ============================
   NÍVEL AVENTUREIRO
   ============================ */

/* busca sequencial por nome */
void buscarItemSequencial() {
    if (qtdItens == 0) {
        printf("\nMochila vazia. Nao ha itens para buscar.\n");
        return;
    }

    char nomeBusca[TAM_STR];
    printf("\n=== Buscar Item (Busca Sequencial) ===\n");
    lerString("Digite o nome do item: ", nomeBusca, TAM_STR);

    bool encontrado = false;
    for (int i = 0; i < qtdItens; i++) {
        if (strcmp(mochila[i].nome, nomeBusca) == 0) {
            printf("\nItem encontrado:\n");
            printf("Nome: %s\n", mochila[i].nome);
            printf("Tipo: %s\n", mochila[i].tipo);
            printf("Quantidade: %d\n", mochila[i].quantidade);
            printf("Prioridade: %d\n", mochila[i].prioridade);
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        printf("Item nao encontrado na mochila.\n");
    }
}

/* ============================
   NÍVEL MESTRE - ORDENAÇÃO
   ============================ */

/* compara dois itens conforme o criterio */
int compararItens(const Item *a, const Item *b, CriterioOrdenacao crit) {
    switch (crit) {
        case ORDENAR_POR_NOME:
            return strcmp(a->nome, b->nome);
        case ORDENAR_POR_TIPO:
            return strcmp(a->tipo, b->tipo);
        case ORDENAR_POR_PRIORIDADE:
            /* menor prioridade primeiro, por exemplo */
            if (a->prioridade < b->prioridade) return -1;
            if (a->prioridade > b->prioridade) return 1;
            return 0;
        default:
            return 0;
    }
}

/* insertion sort com contador de comparações */
void ordenarMochila(CriterioOrdenacao crit) {
    if (qtdItens <= 1) {
        printf("\nPoucos itens para ordenar.\n");
        return;
    }

    long long comparacoes = 0;

    for (int i = 1; i < qtdItens; i++) {
        Item chave = mochila[i];
        int j = i - 1;

        while (j >= 0) {
            comparacoes++;
            if (compararItens(&mochila[j], &chave, crit) > 0) {
                mochila[j + 1] = mochila[j];
                j--;
            } else {
                break;
            }
        }
        mochila[j + 1] = chave;
    }

    /* atualiza flag se foi ordenado por nome */
    if (crit == ORDENAR_POR_NOME) {
        ordenadoPorNome = true;
    } else {
        ordenadoPorNome = false;
    }

    printf("\nMochila ordenada com sucesso.\n");
    printf("Total de comparacoes na ordenacao: %lld\n", comparacoes);
}

/* ============================
   NÍVEL MESTRE - BUSCA BINÁRIA
   ============================ */

void buscarItemBinaria() {
    if (qtdItens == 0) {
        printf("\nMochila vazia. Nao ha itens para buscar.\n");
        return;
    }

    if (!ordenadoPorNome) {
        printf("\nA lista PRECISA estar ordenada por NOME para usar busca binaria.\n");
        printf("Use a opcao de ordenacao por nome antes.\n");
        return;
    }

    char nomeBusca[TAM_STR];
    printf("\n=== Buscar Item (Busca Binaria) ===\n");
    lerString("Digite o nome do item: ", nomeBusca, TAM_STR);

    int inicio = 0;
    int fim = qtdItens - 1;
    bool encontrado = false;

    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        int cmp = strcmp(nomeBusca, mochila[meio].nome);

        if (cmp == 0) {
            printf("\nItem encontrado:\n");
            printf("Nome: %s\n", mochila[meio].nome);
            printf("Tipo: %s\n", mochila[meio].tipo);
            printf("Quantidade: %d\n", mochila[meio].quantidade);
            printf("Prioridade: %d\n", mochila[meio].prioridade);
            encontrado = true;
            break;
        } else if (cmp < 0) {
            fim = meio - 1;
        } else {
            inicio = meio + 1;
        }
    }

    if (!encontrado) {
        printf("Item nao encontrado na mochila.\n");
    }
}

/* ============================
   MENU PRINCIPAL
   ============================ */

void exibirMenu() {
    printf("\n===== CODIGO DA ILHA - MOCHILA =====\n");
    printf("1 - Adicionar item (Novato)\n");
    printf("2 - Remover item (Novato)\n");
    printf("3 - Listar itens (Novato)\n");
    printf("4 - Buscar item por nome (Busca sequencial - Aventureiro)\n");
    printf("5 - Ordenar mochila (Mestre)\n");
    printf("6 - Buscar item por nome (Busca binaria - Mestre)\n");
    printf("0 - Sair\n");
}

/* submenu para escolha de criterio de ordenacao */
void menuOrdenacao() {
    printf("\n=== Ordenar Mochila ===\n");
    printf("1 - Por nome\n");
    printf("2 - Por tipo\n");
    printf("3 - Por prioridade\n");
}

/* ============================
   FUNÇÃO PRINCIPAL
   ============================ */

int main(void) {
    int opcao;

    do {
        exibirMenu();
        opcao = lerInt("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                adicionarItem();
                break;
            case 2:
                removerItem();
                break;
            case 3:
                listarItens();
                break;
            case 4:
                buscarItemSequencial();
                break;
            case 5: {
                menuOrdenacao();
                int c = lerInt("Escolha o criterio: ");
                if (c >= 1 && c <= 3) {
                    ordenarMochila((CriterioOrdenacao)c);
                } else {
                    printf("Criterio invalido.\n");
                }
                break;
            }
            case 6:
                buscarItemBinaria();
                break;
            case 0:
                printf("\nEncerrando o programa...\n");
                break;
            default:
                printf("Opcao invalida.\n");
                break;
        }

    } while (opcao != 0);

    return 0;
}
