#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX 50

struct Ocorrencia; /* Forward Declaration. */

struct Pilha /* Precisa existir já ocorrência, por isso declarei o Forward Declaration. */
{
    struct Ocorrencia *topo;
};

enum UnidadesServicoTipo{
    ambulancia = 1, /* Tempo 3 de registro. Tempo 4 de processamento. */
    bombeiro, /* 2 */ /* Tempo 7. Tempo 2 de processamento. */
    policia /* 3 */ /* Tempo 9. Tempo 5 de processamento. */
};

struct Bairro{
    int idBairro;
    char nomeBairro[50];
    struct Bairro *prox;
    struct AssociacaoBairroServico *primeiroServico;
}; /* Será usado na tabela Hash. */

struct Cidadao{
    char CPF[20];
    char nomePessoa[30];
    char email[30];
    char endereco[30];
    char bairro[30];
    struct Cidadao *prox;
};

struct UnidadeServico{
    int id;
    enum UnidadesServicoTipo tipoUnidade;
    char nomeServico[50];
    char endereco[50];
    char bairro[30];
    struct UnidadeServico *prox;
    struct AssociacaoBairroServico *primeiroBairro;
}; /* Será usada na fila. */

struct Ocorrencia{
    int id;
    char cpfCidadao[20];
    int gravidade; /* 0 à 5 */
    enum UnidadesServicoTipo tipo;
    int IdDoBairro;
    struct Ocorrencia *prox;
}; /* Será usado na fila com prioridade. A prioridade é de 0 à 5 na gravidade. */

// Estrutura para representar a associação entre bairros e serviços
struct AssociacaoBairroServico {
    int idBairro;
    int idServico;
    struct AssociacaoBairroServico *proxBairro;  // Próximo serviço no mesmo bairro
    struct AssociacaoBairroServico *proxServico; // Próximo bairro para o mesmo serviço
};

// Declarações das variáveis globais
extern struct Ocorrencia *filaOcorrencias; /* Fila com prioridade das ocorrências. */
extern struct Cidadao *listaCidadoes;
extern struct UnidadeServico *listaDeServicos; /* Lista de serviços cadastrados. */
extern struct Bairro *tabelaHashing[MAX]; /* Tabela Hashing dos bairros. */
extern struct Cidadao* tabelaHash[MAX];
extern struct Bairro *cabecalhosBairros;
extern struct UnidadeServico *cabecalhosServicos;
extern int tempoAtual; /* Será usado no tempo. */
extern int simulacoes;

// Funções principais
void cadastrarBairro(int id, char *nome);
void cadastrarCidadao(char *cpf, char *nome, char *email, char *endereco, char *bairro);
void cadastrarServico(int idDesejado, int tipo, char *nome, char *endereco, char *bairro);
struct Ocorrencia* registrarOcorrencia(int idDaOcorrencia, char *cpfCidadaoSolicitante, int prioridade, int tipoDaOcorrencia, int idDoBairroAlvo);

void processarOcorrencia(); /* Apenas para aparecer os dados da ocorrência mais importante antes de ser removida. */
void exibirOcorrencias();
int funcaoHashing(int id); /* Para cadastro dos bairros */
void inicializaTabela(); /* Inicialização da tabela hashing. */

/* ESTRUTURAS DE EXIBICAO */
void exibirServicosRegistrados();
void exibirBairrosRegistrados();
void exibirCidadoesRegistrados();
void exibirTudo(struct Pilha *p);

void simulacao(struct Pilha *p);
void tempo(); /*  */
void limparTudo(struct Pilha *p); /* Tirar todas as memórias do MALLOC. */

/* Aleatoriedade de dados: */
#define MAX_DADOS 100 /* Máximo de linhas para cada tipo aleatorio. */

extern char nomes[MAX_DADOS][50];
extern char cpfs[MAX_DADOS][50];
extern char emails[MAX_DADOS][50];
extern char bairros[MAX_DADOS][50];
extern char nomeServicos[MAX_DADOS][50];
extern char enderecos[MAX_DADOS][50];

extern int totalNome, totalCPF, totalEmail, totalBairro, nomeServicoss, enderecoss; /* Armazenar o total de linhas de cada tipo. */
int carregarArquivo(char *nomeArquivo, char dados[][50], int maxLinhas); /* Armazena os tipos nesse vetor. */
void carregarDados();

//Estruturas da pilha e funções necessárias para implementar o histórico de atendimentos
void push(struct Pilha *p, struct Ocorrencia *ptr); //Alteração dos tipos
void inicializa(struct Pilha *p);
int vazia(struct Pilha *p);
void esvazia(struct Pilha *p);
void imprimePilha(struct Pilha *p);
void exibeOcorrencia(struct Ocorrencia *p); /* Para exibir a ocorrencia selecionada no imprimePilha. */

// Funções de busca de inserção na tabela hash
int funcaoHash(char* cpf);
void inserirNaHash(struct Cidadao* novo);
struct Cidadao* buscarPorCPF(char* cpf);
void buscarCidadaoCPF();
struct Bairro* buscarBairroPorID(int id);
void buscarPorId();
int validarCPF(char* cpf);
int buscarIdBairroPorNome(char* nome);

// Funções para listas cruzadas
void inicializarListasCruzadas();
void associarServicoAoBairro(int idBairro, int idServico);
void exibirServicosNoBairro(int idBairro);
void exibirBairrosDoServico(int idServico);
void limparListasCruzadas();
struct Bairro* obterCabecalhoBairro(int idBairro);
struct UnidadeServico* obterCabecalhoServico(int idServico);
struct Ocorrencia* criarCopiaOcorrencia(struct Ocorrencia* original);

struct NoArvoreBST { // Esta estrutura define um "nó" para a nossa Árvore de Busca Binária (BST).
    struct Ocorrencia *ocorrencia; // Ele guarda um ponteiro para a ocorrência.
    struct NoArvoreBST *esquerda;
    struct NoArvoreBST *direita;
};

// Protótipos das funções que vamos usar para manipular a BST.
struct NoArvoreBST* inserirBST(struct NoArvoreBST* raiz, struct Ocorrencia* novaOcorrencia, int ordenarPor); // Para colocar uma nova ocorrência na BST.
void percursoEmOrdemBST(struct NoArvoreBST* raiz); // Para visitar todos os nós da BST em ordem crescente.
struct NoArvoreBST* buscarBST(struct NoArvoreBST* raiz, int id); // Para encontrar uma ocorrência específica pelo ID na BST.
void liberarBST(struct NoArvoreBST* raiz); // Para limpar toda a memória que a BST usou.

struct NoArvoreAVL { // Esta estrutura define um "nó" para a nossa Árvore AVL, que é uma BST "auto-balanceada".
    struct Ocorrencia *ocorrencia; // Assim como na BST, guarda um ponteiro para a ocorrência.
    struct NoArvoreAVL *esquerda;
    struct NoArvoreAVL *direita;
    int altura; // A altura deste nó na árvore, fundamental para o balanceamento da AVL.
};

// Protótipos das funções para a AVL.
struct NoArvoreAVL* inserirAVL(struct NoArvoreAVL* no, struct Ocorrencia* novaOcorrencia); // Para adicionar uma ocorrência na AVL, mantendo o balanceamento.
void percursoEmOrdemAVL(struct NoArvoreAVL* raiz); // Para percorrer a AVL em ordem.
struct NoArvoreAVL* buscarAVL(struct NoArvoreAVL* raiz, int gravidade); // Para buscar ocorrências por gravidade na AVL.
void liberarAVL(struct NoArvoreAVL* raiz); // Para liberar toda a memória da AVL.
