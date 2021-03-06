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

int atualizar_arquivo(int nreg);

/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto);

/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn);

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int* nregistros);

/* Funcoes comparativas para ordenacao dos respectivos indices */
int comp_ip(const void* a, const void* b);
int comp_is_by_pk(const void* a, const void* b);
int comp_is_by_str(const void* a, const void* b);
int comp_isf(const void* a, const void* b);
int comp_ir(const void* a, const void* b);

void criar_isecondary(Ip *indice_primario, Is* istuff, int* nregistros, int type);

void criar_isf(Ip *indice_primario, Isf* iprice, int* nregistros);

void criar_ireverse(Ip *indice_primario, Ir* icategory, int* nregistros, int* ncat);

void inserir_lista(ll **lista_ligada, char* pk);

/* Realiza os scanfs na struct Produto */
void ler_entrada(char* registro, Produto *novo);

/* Insere produto novo no arquivo de dados */
void inserir_arquivo(Produto *novo);

/* Insere novo indice primario e reordena
 	Retorna 1 com inserção bem sucedida, 0 se chave repetida*/
int adicionar_aos_indices(Ip *indice_primario, Is* iproduct, Is* ibrand, Isf *iprice, Ir* icategory, int *nregistros, int* ncat, Produto *j);

int remover(Ip *indice_primario, char* pk, int *nregistros);

int alterar(char* desconto, Ip *indice_alterar, Isf *iprice, int *nregistros);

void liberar_espaco(Ip *indice_primario, Is* iproduct, Is* ibrand, Isf *iprice, Ir* icategory, int *ncat);

/* Busca linearmente por um registro que pode ser repetido */
int buscar_iproduct(char* registro, Ip *indice_primario, Is* istuff, int *nregistros);

/* Busca linearmente por um registro que pode ser repetido */
int buscar_ibrand(char* registro, char* categoria, Ip *indice_primario, Is* istuff, Ir* icategory, int *nregistros, int *ncat);

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

	// iproduct
	Is *iproduct = (Is *) malloc (MAX_REGISTROS * sizeof(Is));
  	if (!iproduct) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_isecondary(iprimary, iproduct, &nregistros, 0);

	// ibrand
	Is *ibrand = (Is *) malloc (MAX_REGISTROS * sizeof(Is));
  	if (!ibrand) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_isecondary(iprimary, ibrand, &nregistros, 1);

	// iprice
	Isf *iprice = (Isf *) malloc (MAX_REGISTROS * sizeof(Isf));
	if (!iprice) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_isf(iprimary, iprice, &nregistros);

	// icategory
	Ir *icategory = (Ir *) malloc (MAX_REGISTROS * sizeof(Ir));
  	if (!icategory) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_ireverse(iprimary, icategory, &nregistros, &ncat);

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

				if(nregistros) {
					Ip *found;
					found = (Ip *) bsearch(produto_aux.pk, iprimary, nregistros, sizeof(Ip), comp_ip);

					if(found == NULL) {
						inserir_arquivo(&produto_aux);
						adicionar_aos_indices(iprimary, iproduct, ibrand, iprice, icategory, &nregistros, &ncat, &produto_aux);
					} else {
						printf(ERRO_PK_REPETIDA, produto_aux.pk);
					}
				} else {
					inserir_arquivo(&produto_aux);
					adicionar_aos_indices(iprimary, iproduct, ibrand, iprice, icategory, &nregistros, &ncat, &produto_aux);
				}

			break;
			case 2:
				/*alterar desconto*/
				printf(INICIO_ALTERACAO);

				char pk_alt[TAM_PRIMARY_KEY];
				char desconto[TAM_DESCONTO];

				scanf("%[^\n]%*c", pk_alt);

				Ip *found;
				found = (Ip *) bsearch(pk_alt, iprimary, nregistros, sizeof(Ip), comp_ip);

				if(found) {

					scanf("%[^\n]%*c", desconto);

					if(alterar(desconto, found, iprice, &nregistros))
						printf(SUCESSO);
					else
						printf(FALHA);
				} else {
					printf(REGISTRO_N_ENCONTRADO);
					printf(FALHA);
				}
			break;
			case 3:
				/*excluir produto*/
				printf(INICIO_EXCLUSAO);

				char pk_rem[TAM_PRIMARY_KEY];

				scanf("%[^\n]%*c", pk_rem);

				if(remover(iprimary, pk_rem, &nregistros))
					printf(SUCESSO);
				else
					printf(FALHA);
			break;
			case 4:
				/*busca*/
				printf(INICIO_BUSCA);

				scanf("%d%*c", &opcao);

				switch(opcao) {
					/* Chave Primaria */
					case 1:
						if(nregistros) {
							scanf("%[^\n]%*c", registro);

							Ip *found;
							found = (Ip *) bsearch(registro, iprimary, nregistros, sizeof(Ip), comp_ip);

							if(found) {
								if(found->rrn != -1)
									exibir_registro(found->rrn, 0);
								else
									printf(REGISTRO_N_ENCONTRADO);
							} else {
								printf(REGISTRO_N_ENCONTRADO);
							}
						} else {
							printf(ARQUIVO_VAZIO);
						}
					break;
					/* Nome */
					case 2:
						if(nregistros) {
							scanf("%[^\n]%*c", registro);

							if(!buscar_iproduct(registro, iprimary, iproduct, &nregistros))
								printf(REGISTRO_N_ENCONTRADO);

						} else {
							printf(ARQUIVO_VAZIO);
						}
					break;
					case 3:
						if(nregistros) {
							char categoria[TAM_CATEGORIA];

							scanf("%[^\n]%*c", registro);
							scanf("%[^\n]%*c", categoria);

							if(!buscar_ibrand(registro, categoria, iprimary, ibrand, icategory, &nregistros, &ncat))
								printf(REGISTRO_N_ENCONTRADO);

						} else {
							printf(ARQUIVO_VAZIO);
						}
					break;
				}

			break;
			case 5:
				/*listagens*/
				printf(INICIO_LISTAGEM);

				scanf("%d%*c", &opcao);

				switch(opcao) {
					/* Ordem de codigo */
					case 1:
						if(nregistros) {
							int next = 0;
							for( int i = 0 ; i < nregistros ; i++) {
								if(next) {
									printf("\n");
								}

								if(iprimary[i].rrn != -1) {
									exibir_registro(iprimary[i].rrn, 0);
									next = 1;
								} else {
									next = 0;
								}
							}
						} else {
							printf(REGISTRO_N_ENCONTRADO);
						}
					break;
					/* Categoria */
					case 2:
						if(nregistros) {
							scanf("%[^\n]%*c", registro);

							Ir *found;
							found = (Ir *) bsearch(registro, icategory, nregistros, sizeof(Ir), comp_ir);

							if(found) {
								ll *aux = found->lista;

								int next = 0;
								while(aux != NULL) {
									if(next) {
										printf("\n");
									}

									Ip *found2;
									found2 = (Ip *) bsearch(aux->pk, iprimary, nregistros, sizeof(Ip), comp_ip);

									if(found2->rrn != -1) {
										exibir_registro(found2->rrn, 0);
										next = 1;
									} else {
										next = 0;
									}
									aux = aux->prox;
								}
							} else {
								printf(REGISTRO_N_ENCONTRADO);
							}

						} else {
							printf(ARQUIVO_VAZIO);
						}
					break;
					case 3:
					/* Marca */
						if(nregistros) {
							int next = 0;
							qsort(ibrand, nregistros, sizeof(Is), comp_is_by_str);
							for( int i = 0 ; i < nregistros ; i++) {
								if(next) {
									printf("\n");
								}

								Ip *found;
								found = (Ip *) bsearch(ibrand[i].pk, iprimary, nregistros, sizeof(Ip), comp_ip);

								if(found->rrn != -1) {
									exibir_registro(found->rrn, 0);
									next = 1;
								} else {
									next = 0;
								}
							}
							qsort(ibrand, nregistros, sizeof(Is), comp_is_by_pk);
						} else {
							printf(REGISTRO_N_ENCONTRADO);
						}
					break;
					case 4:
					/* preco */
						if(nregistros) {
							int next = 0;
							for( int i = 0 ; i < nregistros ; i++) {
								if(next) {
									printf("\n");
								}

								Ip *found;
								found = (Ip *) bsearch(iprice[i].pk, iprimary, nregistros, sizeof(Ip), comp_ip);

								if(found->rrn != -1) {
									exibir_registro(found->rrn, 1);
									next = 1;
								} else {
									next = 0;
								}
							}
						} else {
							printf(REGISTRO_N_ENCONTRADO);
						}
					break;
				}

			break;
			case 6:
				/*libera espaço*/
				free(iprimary);
				free(iproduct);
				free(ibrand);
				free(iprice);
				// for(int i = 0 ; i < ncat ; i++)
				// 	free(icategory[i].lista);
				free(icategory);
				ncat = 0;

				atualizar_arquivo(nregistros);

				/* Índice primário */
				Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
			  	if (!iprimary) {
					perror(MEMORIA_INSUFICIENTE);
					exit(1);
				}
				criar_iprimary(iprimary, &nregistros);

				/*Alocar e criar índices secundários*/

				// iproduct
				Is *iproduct = (Is *) malloc (MAX_REGISTROS * sizeof(Is));
			  	if (!iproduct) {
					perror(MEMORIA_INSUFICIENTE);
					exit(1);
				}
				criar_isecondary(iprimary, iproduct, &nregistros, 0);

				// ibrand
				Is *ibrand = (Is *) malloc (MAX_REGISTROS * sizeof(Is));
			  	if (!ibrand) {
					perror(MEMORIA_INSUFICIENTE);
					exit(1);
				}
				criar_isecondary(iprimary, ibrand, &nregistros, 1);

				// iprice
				Isf *iprice = (Isf *) malloc (MAX_REGISTROS * sizeof(Isf));
				if (!iprice) {
					perror(MEMORIA_INSUFICIENTE);
					exit(1);
				}
				criar_isf(iprimary, iprice, &nregistros);

				// icategory
				Ir *icategory = (Ir *) malloc (MAX_REGISTROS * sizeof(Ir));
			  	if (!icategory) {
					perror(MEMORIA_INSUFICIENTE);
					exit(1);
				}
				criar_ireverse(iprimary, icategory, &nregistros, &ncat);

			break;
			case 7:
				/*imprime o arquivo de dados*/
				printf(INICIO_ARQUIVO);
				printf("%s\n", ARQUIVO);
			break;
			case 8:
				/*imprime os índices secundários*/
				imprimirSecundario(iproduct, ibrand, icategory, iprice, nregistros, ncat);
			break;
			case 9:
	      		/*Liberar memória e finalizar o programa */

				// free((Ip *)iprimary);
				// free(iproduct);
				// free(ibrand);
				// free(iprice);
				// for(int i = 0 ; i < ncat ; i++)
				// 	free(icategory[i].lista);
				// free(icategory);
				// ncat = 0;

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

/* Cria índice primário */
void criar_iprimary(Ip *indice_primario, int* nregistros) {
	if(*nregistros == 0)
		return;

	for (int i = 0; i < *nregistros; i++) {
		Produto j = recuperar_registro(i);

		strcpy(indice_primario[i].pk, j.pk);
		indice_primario[i].rrn = i;
	}

	qsort(indice_primario, *nregistros, sizeof(Ip), comp_ip);

	return;
}

/* Comparacao de strings */
int comp_ip(const void* a, const void* b) {
	Ip *pa = (Ip *)a;
	Ip *pb = (Ip *)b;
	return strcmp(pa->pk, pb->pk);
}

int comp_is_by_pk(const void* a, const void* b) {
	Is *pa = (Is *)a;
	Is *pb = (Is *)b;
	return strcmp(pa->pk, pb->pk);
}

int comp_is_by_str(const void* a, const void* b) {
	Is *pa = (Is *)a;
	Is *pb = (Is *)b;
	return strcmp(pa->string, pb->string);
}

int comp_isf(const void* a, const void* b) {
	Isf *pa = (Isf *)a;
	Isf *pb = (Isf *)b;
	return (int)(pa->price*100 - pb->price*100);
}

int comp_ir(const void* a, const void* b) {
	Ir *pa = (Ir *)a;
	Ir *pb = (Ir *)b;
	return strcmp(pa->cat, pb->cat);
}

void criar_isecondary(Ip *indice_primario, Is* istuff, int* nregistros, int type) {
	if(*nregistros == 0)
		return;

	for (int i = 0; i < *nregistros; i++) {
		Produto j = recuperar_registro(indice_primario[i].rrn);

		strcpy(istuff[i].pk, j.pk);
		type ? strcpy(istuff[i].string, j.marca) : strcpy(istuff[i].string, j.nome);
	}

	qsort(istuff, *nregistros, sizeof(Is), comp_is_by_pk);

}

void criar_isf(Ip *indice_primario, Isf* iprice, int* nregistros) {
	if(*nregistros == 0)
		return;

	for (int i = 0; i < *nregistros; i++) {
		Produto j = recuperar_registro(indice_primario[i].rrn);

		strcpy(iprice[i].pk, j.pk);

		float preco;
		preco = atof(j.preco);
		float desconto = atoi(j.desconto);

		preco = preco * (100 - desconto);

		iprice[i].price = ((int) preco)/ (float) 100;
	}

	qsort(iprice, *nregistros, sizeof(Isf), comp_isf);
}

void inserir_lista(ll **lista_ligada, char* pk) {
	if(*lista_ligada == NULL) {
		ll *novo = (ll *) malloc (sizeof(ll));
		*lista_ligada = novo;
		strcpy(novo->pk, pk);
		novo->prox = NULL;
	} else {
		ll *aux = *lista_ligada;

		while(aux != NULL) {
			// Se primeiro elemento
			if(strcmp(pk, aux->pk) < 0) {
				if(strcmp(aux->pk, (*lista_ligada)->pk) == 0) {
					ll *novo = (ll *) malloc (sizeof(ll));
					(*lista_ligada) = novo;
					strcpy(novo->pk, pk);
					novo->prox = aux;
					return;
				}
			// Se ultimo elemento
			} else if(aux->prox == NULL) {
				ll *novo = (ll *) malloc (sizeof(ll));
				strcpy(novo->pk, pk);
				aux->prox = novo;
				novo->prox = NULL;
				return;
			// Se estiver no meio
			} else if(strcmp(pk, aux->prox->pk) < 0) {
				ll *novo = (ll *) malloc (sizeof(ll));
				strcpy(novo->pk, pk);
				novo->prox = aux->prox;
				aux->prox = novo;
				return;
			}

			aux = aux->prox;
		}
	}
}

void criar_ireverse(Ip *indice_primario, Ir* icategory, int* nregistros, int* ncat) {
	if(*nregistros == 0)
		return;

	char *cat, categorias[TAM_CATEGORIA];

	for (int i = 0; i < *nregistros; i++) {
		Produto j = recuperar_registro(indice_primario[i].rrn);

		strcpy(categorias, j.categoria);

		cat = strtok (categorias, "|");

		while(cat != NULL){
			if(*ncat != 0) {

				int found = 0;
				for(int i = 0 ; i < *ncat && !found ; i++) {
					/* Se houver categoria repetida [Versao Linear] */
					if(strcmp(cat, icategory[i].cat) == 0) {
						// printf("Rep: In %s: ", icategory[i].cat);
						inserir_lista(&icategory[i].lista, j.pk);
						found = 1;
						*ncat = *ncat - 1;
					}
				}

				/* Se for categoria nova */
				if(!found) {
					int pos = *ncat;

					strcpy(icategory[pos].cat, cat);

					// printf("New Cat: In %s: ", icategory[pos].cat);
					inserir_lista(&icategory[pos].lista, j.pk);
				}
			} else {
				/* Se nao houver categorias no vetor */
				strcpy(icategory[0].cat, cat);
				// printf("Empty Cat: In %s: ", icategory[0].cat);
				icategory[0].lista = NULL;
				inserir_lista(&icategory[0].lista, j.pk);
			}

			*ncat = *ncat + 1;
			cat = strtok(NULL, "|");
		}
	}

	qsort(icategory, *ncat, sizeof(Ir), comp_ir);
}

int adicionar_aos_indices(Ip *indice_primario, Is* iproduct, Is* ibrand, Isf *iprice, Ir* icategory, int *nregistros, int* ncat, Produto *j) {

	if(*nregistros == MAX_REGISTROS)
		return 0;

	/* Busca linear por chave repetida */
	for(int i = 0 ; i < *nregistros ; i++) {
		if(strcmp(indice_primario[i].pk, j->pk) == 0)
			return 0;
	}

	strcpy(indice_primario[*nregistros].pk, j->pk);
	indice_primario[*nregistros].rrn = *nregistros;
	qsort(indice_primario, *nregistros + 1, sizeof(Ip), comp_ip);

	strcpy(iproduct[*nregistros].pk, j->pk);
	strcpy(iproduct[*nregistros].string, j->nome);
	qsort(iproduct, *nregistros  + 1, sizeof(Is), comp_is_by_pk);

	strcpy(ibrand[*nregistros].pk, j->pk);
	strcpy(ibrand[*nregistros].string, j->marca);
	qsort(ibrand, *nregistros  + 1, sizeof(Is), comp_is_by_str);

	strcpy(iprice[*nregistros].pk, j->pk);
	float preco;
	preco = atof(j->preco);
	float desconto = atoi(j->desconto);
	preco = preco * (100 - desconto);
	iprice[*nregistros].price = ((int) preco)/ (float) 100;
	qsort(iprice, *nregistros + 1, sizeof(Isf), comp_isf);

	char *cat, categorias[TAM_CATEGORIA];

	strcpy(categorias, j->categoria);
	qsort(icategory, *ncat, sizeof(Ir), comp_ir);

	cat = strtok(categorias, "|");

	while(cat != NULL){
		if(*ncat != 0) {

			int found = 0;
			for(int i = 0 ; i < *ncat && !found ; i++) {
				/* Se houver categoria repetida [Versao Linear] */
				if(strcmp(cat, icategory[i].cat) == 0) {
					// printf("Rep: In %s: ", icategory[i].cat);
					inserir_lista(&icategory[i].lista, j->pk);
					found = 1;
					*ncat = *ncat - 1;
				}
			}

			/* Se for categoria nova */
			if(!found) {
				int pos = *ncat;

				strcpy(icategory[pos].cat, cat);

				// printf("New Cat: In %s: ", icategory[pos].cat);
				inserir_lista(&icategory[pos].lista, j->pk);
			}
		} else {
			/* Se nao houver categorias no vetor */
			strcpy(icategory[0].cat, cat);
			// printf("Empty Cat: In %s: ", icategory[0].cat);
			icategory[0].lista = NULL;
			inserir_lista(&icategory[0].lista, j->pk);
		}

		*ncat = *ncat + 1;
		cat = strtok(NULL, "|");
	}
	qsort(icategory, *ncat, sizeof(Ir), comp_ir);

	*nregistros = *nregistros + 1;

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

	if(ori == 192)
		ARQUIVO[0] = '\0';

	char temp[TAM_REGISTRO];

	sprintf(temp, "%s@%s@%s@%s@%s@%s@%s@",	p->nome, p->marca,
											p->data, p->ano,
											p->preco, p->desconto,
											p->categoria);

	int size = strlen(temp);
	if(size > 192)
		return; // NOT ENOUGH SPACE

	size =  192 - size;
	for( int i = 0 ; i < size ; i++ )
		strcat(temp, "#");

	strcat(ARQUIVO, temp);
}

int buscar_iproduct(char* registro, Ip* iprimary, Is* istuff, int *nregistros) {
	int found = 0;
	/* [Versao Linear] e necessaria nesse caso devido a possibilidade de repeticao de registro (string) */
	for(int i = 0 ; i < *nregistros ; i++) {
		if(strcmp(istuff[i].string, registro) == 0) {
			if(found)
				printf("\n");


			Ip *pk; // hehe
			pk = (Ip *) bsearch(istuff[i].pk, iprimary, *nregistros, sizeof(Ip), comp_ip);

			exibir_registro(pk->rrn, 0);
			found = 1;
		}
	}

	return found;
}

int buscar_ibrand(char* registro, char* categoria, Ip *indice_primario, Is* istuff, Ir* icategory, int *nregistros, int *ncat) {
	int found = 0;
	/* [Versao Linear] e necessaria nesse caso devido a possibilidade de repeticao de registro (string) */
	for(int i = 0 ; i < *nregistros ; i++) {
		if(strcmp(istuff[i].string, registro) == 0) {
			if(found)
				printf("\n");

			Ir *cat;
			cat = (Ir *) bsearch(categoria, icategory, *ncat, sizeof(Ir), comp_ir);

			ll* aux = cat->lista;

			while(aux != NULL) {
				// ERRO AQUI
				if(strcmp(istuff[i].pk, aux->pk) == 0) {
					Ip *pk;
					pk = (Ip *) bsearch(aux->pk, indice_primario, *nregistros, sizeof(Ip), comp_ip);

					exibir_registro(pk->rrn, 0);
					found = 1;
					break;
				}
				aux = aux->prox;
			}
			return found;
		}
	}

	return found;
}


int remover(Ip *indice_primario, char* pk, int *nregistros) {
	if(nregistros) {
		Ip *found;
		found = (Ip *) bsearch(pk, indice_primario, *nregistros, sizeof(Ip), comp_ip);

		if(found) {
			ARQUIVO[found->rrn*192] = '*';
			ARQUIVO[found->rrn*192 + 1] = '|';

			found->rrn = -1;

			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

int alterar(char* desconto, Ip *indice_alterar, Isf *iprice, int* nregistros) {
	if(indice_alterar->rrn != -1) {
		/* Pre-processando a entrada */
		if(strlen(desconto) == 3) {
			int d = atoi(desconto);
			if(d <= 100 && d >= 0) {
				char temp[193];
					strncpy(temp, ARQUIVO + ((indice_alterar->rrn)*192), 192);
					temp[192] = '\0';
				int size = strlen(temp);
				int pos = 0;
					// Procura posicao do campo desconto
				int arroba = 0;
				for(int i = 0 ; i < size ; i++) {
					if(temp[i] == '@') {
						arroba++;
					}
					if(arroba == 5) {
						pos = i + 1;
						break;
					}
				}
					/* Altera Arquivo de Dados */
				ARQUIVO[((indice_alterar->rrn)*192) + pos] = desconto[0];
				ARQUIVO[((indice_alterar->rrn)*192) + pos+1] = desconto[1];
				ARQUIVO[((indice_alterar->rrn)*192) + pos+2] = desconto[2];
					/* Atualiza indice_alterar secundario de preco [Versao Linear] */
				for(int i = 0 ; i < *nregistros ; i++) {
					if(strcmp(iprice[i].pk, indice_alterar->pk) == 0) {
						float preco;
						Produto j = recuperar_registro(indice_alterar->rrn);
						preco = atof(j.preco);
						preco = preco * (100 - d);
						iprice[i].price = ((int) preco)/ (float) 100;
							qsort(iprice, *nregistros, sizeof(Isf), comp_isf);
						break;
					}
				}
				return 1;
			}
		} else {
			return 0;
		}
	}

	return 0;
}

void liberar_espaco(Ip *indice_primario, Is* iproduct, Is* ibrand, Isf *iprice, Ir* icategory, int *ncat) {
	return;
}

int atualizar_arquivo(int nreg) {

	char temp[193];
	int i = 0; // read position
	int j = 0; // write position
	for(i = 0 ; i < nreg ; i++) {
		strncpy(temp, ARQUIVO + ((i)*TAM_REGISTRO), TAM_REGISTRO);

		if(temp[0] != '*' && temp[1] != '|') {
			strncpy(ARQUIVO + ((j)*TAM_REGISTRO), temp, TAM_REGISTRO);
			j++;
		}
	}

	return j;
}

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
