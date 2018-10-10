/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01
 *
 * RA: 743506
 * Aluno: André Matheus Bariani Trava
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY	11
#define TAM_NOME 		51
#define TAM_MARCA 		51
#define TAM_DATA 		11
#define TAM_ANO 		3
#define TAM_PRECO 		8
#define TAM_DESCONTO 	4
#define TAM_CATEGORIA 	51


#define TAM_REGISTRO 	192
#define MAX_REGISTROS 	1000
#define MAX_CATEGORIAS 	30
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas para o usuario */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!"
#define INICIO_BUSCA 		 		"**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM  			"**********************LISTAR**********************\n"
#define INICIO_ALTERACAO 			"**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO  			"**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO  			"**********************ARQUIVO*********************\n"
#define INICIO_ARQUIVO_SECUNDARIO	"*****************ARQUIVO SECUNDARIO****************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"



/* Registro do Produto */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;


/*----- Registros dos Índices -----*/

/* Struct para índice Primário */
typedef struct primary_index{
  char pk[TAM_PRIMARY_KEY];
  int rrn;
} Ip;

/* Struct para índice secundário */
typedef struct secundary_index{
  char pk[TAM_PRIMARY_KEY];
  char string[TAM_NOME];
} Is;

/* Struct para índice secundário de preços */
typedef struct secundary_index_of_final_price{
  float price;
  char pk[TAM_PRIMARY_KEY];
} Isf;

/* Lista ligada para o Índice abaixo*/
typedef struct linked_list{
  char pk[TAM_PRIMARY_KEY];
  struct linked_list *prox;
} ll;

/* Struct para lista invertida */
typedef struct reverse_index{
  char cat[TAM_CATEGORIA];
  ll* lista;
} Ir;

/*----- GLOBAL -----*/
char ARQUIVO[TAM_ARQUIVO];

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto);

/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn);

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int* nregistros);

/* Funcao comparativa para ordenacao dos indices primarios */
// int comp_ip(const void* a, const void* b);

/* Realiza os scanfs na struct Produto */
void ler_entrada(char* registro, Produto *novo);

/* Insere produto novo no arquivo de dados */
void inserir_arquivo(Produto *novo);

/* Insere novo indice primario e reordena
 	Retorna 1 com inserção bem sucedida, 0 se chave repetida*/
int inserir_iprimary(Ip *indice_primario, int *nregistros, Produto *j);

/* Rotina para impressao de indice secundario */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat);


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main(){
  /* Arquivo */
	int carregarArquivo = 0, nregistros = 0, ncat = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	/* Índice primário */
	Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
  	if (!iprimary) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_iprimary(iprimary, &nregistros);

	/*Alocar e criar índices secundários*/

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		Produto produto_aux;
		char registro[TAM_NOME];

		scanf("%d%*c", &opcao);
		switch(opcao)
		{
			case 1:
				ler_entrada(registro, &produto_aux);
				if(inserir_iprimary(iprimary, &nregistros, &produto_aux)) inserir_arquivo(&produto_aux);
				else printf(ERRO_PK_REPETIDA, produto_aux.pk);
			break;
			case 2:
				/*alterar desconto*/
				printf(INICIO_ALTERACAO);
				/*
				if(alterar([args]))
					printf(SUCESSO);
				else
					printf(FALHA);
				*/
			break;
			case 3:
				/*excluir produto*/
				printf(INICIO_EXCLUSAO);
				/*
				if(remover([args]))
					printf(SUCESSO);
				else
					printf(FALHA);
				*/
			break;
			case 4:
				/*busca*/
				printf(INICIO_BUSCA );
			break;
			case 5:
				/*listagens*/
				printf(INICIO_LISTAGEM);
			break;
			case 6:
				/*libera espaço*/
			break;
			case 7:
				/*imprime o arquivo de dados*/
				printf(INICIO_ARQUIVO);
				printf("%s\n", ARQUIVO);
			break;
			case 8:
				/*imprime os índices secundários*/
				// imprimirSecundario(iproduct, ibrand, icategory, iprice, nregistros, ncat);
			break;
			case 9:
	      		/*Liberar memória e finalizar o programa */
				return 0;
			break;
			default:
				printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}

/* Gera campo Código do produto de entrada */
void gerarChave(Produto *p) {
	p->pk[0] = toupper(p->nome[0]);
	p->pk[1] = toupper(p->nome[1]);
	p->pk[2] = toupper(p->marca[0]);
	p->pk[3] = toupper(p->marca[1]);
	p->pk[4] = p->data[0];
	p->pk[5] = p->data[1];
	p->pk[6] = p->data[3];
	p->pk[7] = p->data[4];
	p->pk[8] = p->ano[0];
	p->pk[9] = p->ano[1];
	p->pk[10] = '\0';
}

// int comp_ip(const void* a, const void* b) { return strcmp(*(char*) a, *(char*) b); }

/* Cria índice primário */
void criar_iprimary(Ip *indice_primario, int* nregistros) {
	if(*nregistros == 0)
		return;

	for (int i = 0; i < *nregistros; i++) {
		Produto j = recuperar_registro(i);

		strcpy(indice_primario[i].pk, j.pk);
		indice_primario[i].rrn = i;
	}

	// qsort(indice_primario, *nregistros, sizeof(Ip), comp_ip);

	return;
}

int inserir_iprimary(Ip *indice_primario, int* nregistros, Produto *j) {
	if(*nregistros == MAX_REGISTROS)
		return 0;

	/* Busca linear por chave repetida */
	for(int i = 0 ; i < *nregistros ; i++) {
		if(strcmp(indice_primario[i].pk, j->pk))
			return 0;
	}

	strcpy(indice_primario[*nregistros].pk, j->pk);
	indice_primario[*nregistros].rrn = *nregistros;

	*nregistros++;
	// qsort(indice_primario, *nregistros, sizeof(Ip), comp_ip);

	return 1;
}

void ler_entrada(char* registro, Produto *novo) {
	scanf("%[^\n]%*c", registro);
	strcpy(novo->nome, registro);
	// getchar();

	scanf("%[^\n]%*c", registro);
	strcpy(novo->marca, registro);
	// getchar();

	scanf("%[^\n]%*c", registro);
	strcpy(novo->data, registro);
	// getchar();

	scanf("%[^\n]%*c", registro);
	strcpy(novo->ano, registro);
	// getchar();

	scanf("%[^\n]%*c", registro);
	strcpy(novo->preco, registro);
	// getchar();

	scanf("%[^\n]%*c", registro);
	strcpy(novo->desconto, registro);
	// getchar();

	scanf("%[^\n]%*c", registro);
	strcpy(novo->categoria, registro);
	// getchar();

	gerarChave(novo);
}

void inserir_arquivo(Produto *p) {

	int ori = strlen(ARQUIVO) + 192;

	strcat(ARQUIVO, p->nome);
	strcat(ARQUIVO, "@");

	strcat(ARQUIVO, p->marca);
	strcat(ARQUIVO, "@");

	strcat(ARQUIVO, p->data);
	strcat(ARQUIVO, "@");

	strcat(ARQUIVO, p->ano);
	strcat(ARQUIVO, "@");

	strcat(ARQUIVO, p->preco);
	strcat(ARQUIVO, "@");

	strcat(ARQUIVO, p->desconto);
	strcat(ARQUIVO, "@");

	strcat(ARQUIVO, p->categoria);
	strcat(ARQUIVO, "@");

	int novo =  ori - strlen(ARQUIVO);
	for( int i = 0 ; i < novo ; i++ ) {
		strcat(ARQUIVO, "#");
	}

}


/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto)
{
	if(rrn<0)
		return 0;
	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
  	char *cat, categorias[TAM_CATEGORIA];
	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	if(!com_desconto)
	{
		printf("%s\n", j.preco);
		printf("%s\n", j.desconto);
	}
	else
	{
		sscanf(j.desconto,"%d",&desconto);
		sscanf(j.preco,"%f",&preco);
		preco = preco *  (100-desconto);
		preco = ((int) preco)/ (float) 100 ;
		printf("%07.2f\n",  preco);

	}
	strcpy(categorias, j.categoria);

	cat = strtok (categorias, "|");

	while(cat != NULL){
		printf("%s", cat);
		cat = strtok (NULL, "|");
		if(cat != NULL){
			printf(", ");
		}
	}

	printf("\n");

	return 1;
}


int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
	char temp[193], *p;
	strncpy(temp, ARQUIVO + ((rrn)*192), 192);
	temp[192] = '\0';
	Produto j;
	p = strtok(temp,"@");
	strcpy(j.nome,p);
	p = strtok(NULL,"@");
	strcpy(j.marca,p);
	p = strtok(NULL,"@");
	strcpy(j.data,p);
	p = strtok(NULL,"@");
	strcpy(j.ano,p);
	p = strtok(NULL,"@");
	strcpy(j.preco,p);
	p = strtok(NULL,"@");
	strcpy(j.desconto,p);
	p = strtok(NULL,"@");
	strcpy(j.categoria,p);
	gerarChave(&j);
	return j;
}


/* Imprimir indices secundarios */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat){
	int opPrint = 0;
	ll *aux;
	printf(INICIO_ARQUIVO_SECUNDARIO);
	scanf("%d", &opPrint);
	if(!nregistros)
		printf(ARQUIVO_VAZIO);
	switch (opPrint) {
		case 1:
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",iproduct[i].pk, iproduct[i].string);
			}
		break;
		case 2:
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",ibrand[i].pk, ibrand[i].string);
			}
		break;
		case 3:
			for(int i = 0; i < ncat; i++){
				printf("%s", icategory[i].cat);
				aux =  icategory[i].lista;
				while(aux != NULL){
					printf(" %s", aux->pk);
					aux = aux->prox;
				}
				printf("\n");
			}
		break;

		case 4:
		for(int i = 0; i < nregistros; i++){
			printf("%s %.2f\n",iprice[i].pk, iprice[i].price);
		}
		break;
	}
}
