#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
	char pk[51];
	char nome[51];
	char marca[51];
	char data[51];	/* DD/MM/AAAA */
	char ano[51];
	char preco[51];
	char desconto[51];
	char categoria[51];
} Produto;

void ler_entrada(char* registro, Produto *novo) {
	scanf("%51[^\n]", registro);
	strcpy(novo->nome, registro);
    getchar();

    printf("%s\n", novo->nome);

	scanf("%51[^\n]", registro);
	strcpy(novo->marca, registro);
    getchar();

    printf("%s\n", novo->marca);

	scanf("%51[^\n]", registro);
	strcpy(novo->data, registro);
    getchar();

    printf("%s\n", novo->data);
	scanf("%51[^\n]", registro);
	strcpy(novo->ano, registro);
    getchar();

    printf("%s\n", novo->ano);
	scanf("%51[^\n]", registro);
	strcpy(novo->preco, registro);
    getchar();

    printf("%s\n", novo->preco);
	scanf("%51[^\n]", registro);
	strcpy(novo->desconto, registro);
    getchar();

    printf("%s\n", novo->desconto);
	scanf("%51[^\n]", registro);
	strcpy(novo->categoria, registro);
    getchar();

    printf("%s\n", novo->categoria);
}

int main() {
    char registro[51];
    Produto j;

    ler_entrada(registro,&j);

    return 0;
}
