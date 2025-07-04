#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "Estruturas.h"

// Definição das variáveis globais
struct Ocorrencia *filaOcorrencias = NULL;
struct Bairro *tabelaHashing[MAX];
struct UnidadeServico *listaDeServicos = NULL;
struct Cidadao *listaCidadoes = NULL;
struct Cidadao* tabelaHash[MAX] = {NULL};
struct Bairro *cabecalhosBairros = NULL;
struct UnidadeServico *cabecalhosServicos = NULL;
int tempoAtual = 0;
int simulacoes = 1;

struct NoArvoreBST* ocorrenciasBST = NULL;
//Raiz para a Árvoe BST de ocorrências

// Raiz global para a Árvore AVL de ocorrências.
struct NoArvoreAVL* ocorrenciasAVL = NULL; //


// Variáveis para dados aleatórios
char nomes[MAX_DADOS][50];
char cpfs[MAX_DADOS][50];
char emails[MAX_DADOS][50];
char bairros[MAX_DADOS][50];
char nomeServicos[MAX_DADOS][50];
char enderecos[MAX_DADOS][50];
int totalNome = 0, totalCPF = 0, totalEmail = 0, totalBairro = 0, nomeServicoss = 0, enderecoss = 0;

//Parte 2 - Listas Cruzadas

void inicializarListasCruzadas() {
    cabecalhosBairros = NULL;
    cabecalhosServicos = NULL;
}

// Funcao para encontrar ou criar cabecalho de bairro
struct Bairro* obterCabecalhoBairro(int idBairro) {
    struct Bairro* atual = cabecalhosBairros;

    // Procurar se ja existe
    while (atual != NULL) {
        if (atual->idBairro == idBairro) {
            return atual;
        }
        atual = atual->prox;
    }

    // Se nao existe, criar novo
    struct Bairro* novo = malloc(sizeof(struct Bairro));
    if (novo == NULL) {
        printf("Erro ao alocar memoria para cabecalho de bairro.\n");
        return NULL;
    }
    novo->idBairro = idBairro;
    // O nome do bairro nao e necessario aqui, pois este e apenas um cabecalho para a lista cruzada
    // strcpy(novo->nomeBairro, ""); // Initialize if needed
    novo->primeiroServico = NULL;
    novo->prox = cabecalhosBairros;
    cabecalhosBairros = novo;

    return novo;
}

// Funcao para encontrar ou criar cabecalho de servico
struct UnidadeServico* obterCabecalhoServico(int idServico) {
    struct UnidadeServico* atual = cabecalhosServicos;

    while (atual != NULL) {
        if (atual->id == idServico) {
            return atual;
        }
        atual = atual->prox;
    }

    struct UnidadeServico* novo = malloc(sizeof(struct UnidadeServico));
    if (novo == NULL) {
        printf("Erro ao alocar memoria para cabecalho de servico.\n");
        return NULL;
    }
    novo->id = idServico;
    // O nome e outros campos do servico nao sao necessarios aqui
    // strcpy(novo->nomeServico, ""); // Initialize if needed
    novo->primeiroBairro = NULL;
    novo->prox = cabecalhosServicos;
    cabecalhosServicos = novo;

    return novo;
}

void associarServicoAoBairro(int idBairro, int idServico) {
    // Criar nova associacao
    struct AssociacaoBairroServico* nova = malloc(sizeof(struct AssociacaoBairroServico));
    if (nova == NULL) {
        printf("Erro ao alocar memoria para associacao.\n");
        return;
    }
    nova->idBairro = idBairro;
    nova->idServico = idServico;
    nova->proxBairro = NULL;
    nova->proxServico = NULL;

    // Obter cabecalhos
    struct Bairro* cabBairro = obterCabecalhoBairro(idBairro);
    struct UnidadeServico* cabServico = obterCabecalhoServico(idServico);

    if (cabBairro == NULL || cabServico == NULL) {
        free(nova);
        return;
    }

    // Inserir na lista do bairro (servicos disponiveis no bairro)
    nova->proxBairro = cabBairro->primeiroServico;
    cabBairro->primeiroServico = nova;

    // Inserir na lista do servico (bairros atendidos pelo servico)
    nova->proxServico = cabServico->primeiroBairro;
    cabServico->primeiroBairro = nova;
}

void exibirServicosNoBairro(int idBairro) {
    struct Bairro* cabBairro = cabecalhosBairros;

    // Encontrar o bairro
    while (cabBairro != NULL && cabBairro->idBairro != idBairro) {
        cabBairro = cabBairro->prox;
    }

    if (cabBairro == NULL) {
        printf("Bairro ID %d nao encontrado nas associacoes.\n", idBairro);
        return;
    }

    printf("Servicos disponiveis no bairro ID %d:\n", idBairro);
    struct AssociacaoBairroServico* atual = cabBairro->primeiroServico;

    if (atual == NULL) {
        printf("Nenhum servico cadastrado para este bairro nas associacoes.\n");
        return;
    }

    while (atual != NULL) {
        printf("- Servico ID: %d\n", atual->idServico);
        atual = atual->proxBairro;
    }
}

void exibirBairrosDoServico(int idServico) {
    struct UnidadeServico* cabServico = cabecalhosServicos;

    // Encontrar o servico
    while (cabServico != NULL && cabServico->id != idServico) {
        cabServico = cabServico->prox;
    }

    if (cabServico == NULL) {
        printf("Servico ID %d nao encontrado nas associacoes.\n", idServico);
        return;
    }

    printf("Bairros atendidos pelo servico ID %d:\n", idServico);
    struct AssociacaoBairroServico* atual = cabServico->primeiroBairro;

    if (atual == NULL) {
        printf("Este servico nao atende nenhum bairro nas associacoes.\n");
        return;
    }

    while (atual != NULL) {
        printf("- Bairro ID: %d\n", atual->idBairro);
        atual = atual->proxServico;
    }
}


// Vai inserir na tabela hash usando a funcaoHash
void inserirNaHash(struct Cidadao* novo) {
    if (novo == NULL) return;
    int indice = funcaoHash(novo->CPF);
    novo->prox = tabelaHash[indice];
    tabelaHash[indice] = novo;
}

// Busca cpf atraves da funcao hash
struct Cidadao* buscarPorCPF(char* cpf) {
    int indice = funcaoHash(cpf);
    struct Cidadao* atual = tabelaHash[indice];
    while (atual != NULL) {
        if (strcmp(atual->CPF, cpf) == 0) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

// Realiza a busca do cidadao utilizando a tabela hashing
void buscarCidadaoCPF() {
    char cpf[20];
    printf("Digite o CPF a buscar: ");
    scanf(" %s", cpf);

    struct Cidadao* cid = buscarPorCPF(cpf);
    if (cid != NULL) {
        printf("Encontrado!\nNome: %s\nEmail: %s\nEndereco: %s\nBairro: %s\n",
            cid->nomePessoa, cid->email, cid->endereco, cid->bairro);
    } else {
        printf("Cidadao nao encontrado.\n");
    }
}

// Busca o bairro atraves do id utilizando a tabela hashing
struct Bairro* buscarBairroPorID(int id) {
    int indice = funcaoHashing(id);
    struct Bairro* atual = tabelaHashing[indice];
    while (atual != NULL) {
        if (atual->idBairro == id)
            return atual;
        atual = atual->prox;
    }
    return NULL;
}

void buscarPorId() {
    int id;
    printf("Digite o id para a busca: ");
    scanf(" %d", &id);

    struct Bairro* bar = buscarBairroPorID(id);
    if (bar != NULL) {
        printf("Encontrado!\nNome: %s\nID: %d\n",
            bar->nomeBairro, bar->idBairro);
    } else {
        printf("Bairro nao encontrado.\n");
    }
}

void limparListasCruzadas() {
    // Limpar todas as associacoes primeiro para evitar vazamentos
    // Iterar sobre todos os cabecalhos de bairro e liberar suas listas de servicos
    struct Bairro* bairroAtual = cabecalhosBairros;
    while (bairroAtual != NULL) {
        struct AssociacaoBairroServico* assoc = bairroAtual->primeiroServico;
        while (assoc != NULL) {
            struct AssociacaoBairroServico* tempAssoc = assoc;
            assoc = assoc->proxBairro;
            free(tempAssoc);
        }
        bairroAtual->primeiroServico = NULL; // Garantir que a lista esteja limpa
        bairroAtual = bairroAtual->prox;
    }

    // Agora, liberar os cabecalhos de bairro
    bairroAtual = cabecalhosBairros;
    while (bairroAtual != NULL) {
        struct Bairro* temp = bairroAtual;
        bairroAtual = bairroAtual->prox;
        free(temp);
    }
    cabecalhosBairros = NULL;

    // Fazer o mesmo para os cabecalhos de servico
    struct UnidadeServico* servicoAtual = cabecalhosServicos;
    while (servicoAtual != NULL) {
        // As associacoes ja foram liberadas ao iterar pelos bairros
        // Nao e necessario liberar novamente assoc->proxServico aqui
        servicoAtual = servicoAtual->prox;
    }
    servicoAtual = cabecalhosServicos; // Reset para liberar os proprios cabecalhos
    while (servicoAtual != NULL) {
        struct UnidadeServico* temp = servicoAtual;
        servicoAtual = servicoAtual->prox;
        free(temp);
    }
    cabecalhosServicos = NULL;
}


/* Parte da pilha: */

void push(struct Pilha *p, struct Ocorrencia* ptr){
    if (p == NULL || ptr == NULL) return;
    ptr->prox = p->topo;
    p->topo = ptr;
}

void inicializa(struct Pilha *p){
    if (p != NULL) {
        p->topo = NULL;
    }
}

int vazia(struct Pilha *p){
    if(p == NULL || p->topo == NULL){
        return 1;
    }else{
        return 0;
    }
}

void esvazia(struct Pilha *p){
    if (p == NULL) return;

    struct Ocorrencia *ptr = p->topo;
    struct Ocorrencia *aux;

    while(ptr != NULL){
        aux = ptr;
        ptr = ptr->prox;
        free(aux);
    }

    p->topo = NULL;
}

void exibeOcorrencia(struct Ocorrencia *p){
    if (p != NULL) {
        printf("Ocorrencia ID %d do cidadao %s (gravidade %d, tipo %d, bairro ID %d)\n",
            p->id,
            p->cpfCidadao,
            p->gravidade,
            p->tipo,
            p->IdDoBairro);
    }
}

void imprimePilha(struct Pilha *p){
    if (p == NULL) {
        printf("Pilha nao inicializada.\n");
        return;
    }

    struct Ocorrencia *pp = p->topo;

    printf("\nPilha de Historico de Atendimentos:\n");
    if (pp == NULL) {
        printf("Pilha vazia.\n");
        return;
    }

    while(pp != NULL){
        exibeOcorrencia(pp);
        pp = pp->prox;
    }
}

//Parte 1:

int funcaoHashing(int id){
    return id % MAX;
}

void inicializaTabela(){
    for(int i = 0; i < MAX; i++){
        tabelaHashing[i] = NULL;
        tabelaHash[i] = NULL;
    }
}

void cadastrarBairro(int id, char *nome){
    // Verificar se ja existe
    if (buscarBairroPorID(id) != NULL) {
        printf("Bairro com ID %d ja existe.\n", id);
        return;
    }

    int indice = funcaoHashing(id);
    struct Bairro *novo = malloc(sizeof(struct Bairro));
    if (novo == NULL) {
        printf("Erro ao alocar memoria para bairro.\n");
        return;
    }

    novo->idBairro = id;
    strcpy(novo->nomeBairro, nome);
    novo->prox = NULL;
    novo->primeiroServico = NULL;

    if(tabelaHashing[indice] != NULL){
        struct Bairro *atual = tabelaHashing[indice];
        while(atual->prox != NULL){
            atual = atual->prox;
        }
        atual->prox = novo;
    }else{
        tabelaHashing[indice] = novo;
    }

    printf("Bairro %s (ID: %d) cadastrado com sucesso.\n", nome, id);
}

void cadastrarCidadao(char *cpf, char *nome, char *email, char *endereco, char *bairro){

    if (buscarPorCPF(cpf) != NULL) {
        printf("CPF %s ja cadastrado. Ignorando cadastro duplicado.\n", cpf);
        return;
    }

    struct Cidadao *novo = malloc(sizeof(struct Cidadao));
    if (novo == NULL) {
        printf("Erro ao alocar memoria para cidadao.\n");
        return;
    }

    strcpy(novo->CPF, cpf);
    strcpy(novo->nomePessoa, nome);
    strcpy(novo->email, email);
    strcpy(novo->endereco, endereco);
    strcpy(novo->bairro, bairro);
    novo->prox = NULL;

    // Inserir na lista encadeada (listaCidadoes)
    // Isso parece ser uma lista auxiliar, a principal busca e pela hash
    novo->prox = listaCidadoes;
    listaCidadoes = novo;

    // Inserir na tabela hash
    inserirNaHash(novo);
    printf("Cidadao %s cadastrado com sucesso.\n", nome);
}

void cadastrarServico(int idDesejado, int tipo, char *nome, char *endereco, char *bairro){
    struct UnidadeServico *unidadeNova = malloc(sizeof(struct UnidadeServico));
    if (unidadeNova == NULL) {
        printf("Erro ao alocar memoria para servico.\n");
        return;
    }

    unidadeNova->id = idDesejado;
    unidadeNova->tipoUnidade = tipo;
    strcpy(unidadeNova->nomeServico, nome);
    strcpy(unidadeNova->endereco, endereco);
    strcpy(unidadeNova->bairro, bairro);
    unidadeNova->prox = listaDeServicos;
    unidadeNova->primeiroBairro = NULL; // Inicializar para listas cruzadas
    listaDeServicos = unidadeNova;

    // Encontrar ID do bairro e fazer associacao nas listas cruzadas
    int idBairro = buscarIdBairroPorNome(bairro);
    if (idBairro != -1) {
        associarServicoAoBairro(idBairro, idDesejado);
    }

    printf("Servico %s (ID: %d) cadastrado com sucesso.\n", nome, idDesejado);
}

int buscarIdBairroPorNome(char* nome) {
    for (int i = 0; i < MAX; i++) {
        struct Bairro* atual = tabelaHashing[i];
        while (atual != NULL) {
            if (strcmp(atual->nomeBairro, nome) == 0) {
                return atual->idBairro; // Corrigido para idBairro
            }
            atual = atual->prox;
        }
    }
    return -1; // Nao encontrado
}

void exibirBairrosRegistrados(){
    printf("\nBairros cadastrados:\n");
    int encontrou = 0;

    for(int indice = 0; indice < MAX; indice++){
        struct Bairro *p = tabelaHashing[indice];

        while(p != NULL){
            printf("Nome: %s\n", p->nomeBairro);
            printf("Id: %d\n", p->idBairro); // Corrigido para idBairro
            printf("-------------------\n");
            encontrou = 1;
            p = p->prox;
        }
    }

    if (!encontrou) {
        printf("Nenhum bairro cadastrado.\n");
    }
}

void exibirServicosRegistrados(){
    struct UnidadeServico *p = listaDeServicos;
    int i = 1;

    if(p == NULL){
        printf("\nNenhum servico foi registrado ate agora.\n");
    }else{
        printf("\nServicos cadastrados:\n");
        while(p != NULL){
            printf("\n        Servico %d:\n", i);
            printf("Nome: %s\n", p->nomeServico);
            printf("Endereco: %s\n", p->endereco);
            printf("Bairro: %s\n", p->bairro);
            printf("ID: %d\n", p->id);
            printf("Tipo de Servico: %d\n", p->tipoUnidade);
            printf("-------------------\n");

            p = p->prox;
            i++;
        }
    }
}

void exibirCidadoesRegistrados(){
    printf("\nLista de todos os cidadaos cadastrados:\n");
    int encontrou = 0;

    for (int i = 0; i < MAX; i++) {
        struct Cidadao* atual = tabelaHash[i];

        while (atual != NULL) {
            printf("CPF: %s\n", atual->CPF);
            printf("Nome: %s\n", atual->nomePessoa);
            printf("Email: %s\n", atual->email);
            printf("Endereco: %s\n", atual->endereco);
            printf("Bairro: %s\n", atual->bairro);
            printf("-------------------\n");
            encontrou = 1;
            atual = atual->prox;
        }
    }

    if (!encontrou) {
        printf("Nenhum cidadao cadastrado.\n");
    }
}

struct Ocorrencia* registrarOcorrencia(int idDaOcorrencia, char *cpfCidadaoSolicitante, int prioridade, int tipoDaOcorrencia, int idDoBairroAlvo){
    struct Ocorrencia *novo = malloc(sizeof(struct Ocorrencia));
    if (novo == NULL) {
        printf("Erro ao alocar memoria para ocorrencia.\n");
        return NULL;
    }

    strcpy(novo->cpfCidadao, cpfCidadaoSolicitante);
    novo->id = idDaOcorrencia;
    novo->gravidade = prioridade;
    novo->tipo = tipoDaOcorrencia;
    novo->IdDoBairro = idDoBairroAlvo;
    novo->prox = NULL;

    // Simular tempo de registro baseado no tipo
    switch (tipoDaOcorrencia) {
        case 1: // ambulancia
            for (int j = 0; j < 3; j++) tempo();
            break;
        case 2: // bombeiro
            for (int j = 0; j < 7; j++) tempo();
            break;
        case 3: // policia
            for (int j = 0; j < 9; j++) tempo();
            break;
        default:
            break;
    }

    // Inserir na fila com prioridade (gravidade maior = maior prioridade)
    if(filaOcorrencias == NULL || novo->gravidade > filaOcorrencias->gravidade){
        novo->prox = filaOcorrencias;
        filaOcorrencias = novo;
    }else{
        struct Ocorrencia *p = filaOcorrencias;
        struct Ocorrencia *ant = NULL;

        while(p != NULL && p->gravidade >= novo->gravidade){
            ant = p;
            p = p->prox;
        }

        if(p == NULL){
            ant->prox = novo;
        }else{
            ant->prox = novo;
            novo->prox = p;
        }
    }

    ocorrenciasBST = inserirBST(ocorrenciasBST, novo, 1); // Ordenação por ID
    ocorrenciasAVL = inserirAVL(ocorrenciasAVL, novo);    // Ordenação por gravidade

    printf("Ocorrencia ID %d registrada com sucesso.\n", idDaOcorrencia);
    return novo;
}

void processarOcorrencia(){
    if(filaOcorrencias == NULL){
        printf("\n\nFila de ocorrencias esta vazia. Nenhuma ocorrencia para processar.\n");
        return;
    }

    struct Ocorrencia *ocorrenciaAtual = filaOcorrencias;
    filaOcorrencias = filaOcorrencias->prox;

    // Simular tempo de processamento baseado no tipo
    switch (ocorrenciaAtual->tipo) {
        case 1: // ambulancia
            for (int j = 0; j < 4; j++) tempo();
            break;
        case 2: // bombeiro
            for (int j = 0; j < 2; j++) tempo();
            break;
        case 3: // policia
            for (int j = 0; j < 5; j++) tempo();
            break;
        default:
            break;
    }

     printf("Processando ocorrencia ID %d do cidadao %s (gravidade %d, tipo %d, bairro ID %d)\n",
        ocorrenciaAtual->id,
        ocorrenciaAtual->cpfCidadao,
        ocorrenciaAtual->gravidade,
        ocorrenciaAtual->tipo,
        ocorrenciaAtual->IdDoBairro);

    free(ocorrenciaAtual);
}

void exibirOcorrencias(){
    struct Ocorrencia *ocorrenciaAtual = filaOcorrencias;
    if (ocorrenciaAtual == NULL) {
        printf("\nFila de ocorrencias vazia.\n");
        return;
    }
    printf("\nFila de Ocorrencias (por prioridade):\n");
    while(ocorrenciaAtual != NULL){
        printf("Ocorrencia ID %d do cidadao %s (gravidade %d, tipo %d, bairro ID %d)\n",
        ocorrenciaAtual->id,
        ocorrenciaAtual->cpfCidadao,
        ocorrenciaAtual->gravidade,
        ocorrenciaAtual->tipo,
        ocorrenciaAtual->IdDoBairro);
        ocorrenciaAtual = ocorrenciaAtual->prox;
    }
}

void tempo() {
    tempoAtual++;
}

void limparTudo(struct Pilha *p){
    // Limpar fila de ocorrencias
    struct Ocorrencia *oco = filaOcorrencias;
    while(oco != NULL){
        struct Ocorrencia *tempOco = oco;
        oco = oco->prox;
        free(tempOco);
    }
    filaOcorrencias = NULL;

    // Limpar lista de servicos
    struct UnidadeServico *uni = listaDeServicos;
    while(uni != NULL){
        struct UnidadeServico *tempUni = uni;
        uni = uni->prox;
        free(tempUni);
    }
    listaDeServicos = NULL;

    // Limpar lista de cidadaos (se usada como lista separada da hash)
    struct Cidadao *cid = listaCidadoes;
    while(cid != NULL){
        struct Cidadao *tempC = cid;
        cid = cid->prox;
        free(tempC);
    }
    listaCidadoes = NULL;

    // Limpar tabela hashing de bairros
    for(int indice = 0; indice < MAX; indice++){
        struct Bairro *bai = tabelaHashing[indice];
        while(bai != NULL){
            struct Bairro *tempBai = bai;
            bai = bai->prox;
            free(tempBai);
        }
        tabelaHashing[indice] = NULL;
    }

    // Limpar tabela hash de cidadaos
    for(int indice = 0; indice < MAX; indice++){
        struct Cidadao *hashCid = tabelaHash[indice];
        while(hashCid != NULL){
            struct Cidadao *tempHashCid = hashCid;
            hashCid = hashCid->prox;
            free(tempHashCid);
        }
        tabelaHash[indice] = NULL;
    }

      // Libera a memória alocada para a BST.
    liberarBST(ocorrenciasBST); //
    ocorrenciasBST = NULL; // Zera o ponteiro da BST.

    // Libera a memória alocada para a AVL.
    liberarAVL(ocorrenciasAVL); //
    ocorrenciasAVL = NULL; // Zera o ponteiro da AVL.

    // Esvaziar a pilha de historico de atendimentos
    esvazia(p);

    // Limpar as listas cruzadas
    limparListasCruzadas();

    printf("\nTodos os dados e estruturas foram limpos.\n");
}

int carregarArquivo(char *nomeArquivo, char dados[][50], int maxLinhas) {
    FILE *f = fopen(nomeArquivo, "r");

    if (f == NULL) {
        printf("Erro: Arquivo '%s' nao abriu corretamente ou nao existe.\n", nomeArquivo);
        return 0; // Retorna 0 para indicar que nada foi carregado
    }

    int i = 0;
    while (i < maxLinhas && fgets(dados[i], 50, f) != NULL) {
        int len = strlen(dados[i]);
        if (len > 0 && dados[i][len - 1] == '\n') {
            dados[i][len - 1] = '\0';
        }
        i++;
    }

    fclose(f);
    return i; /* Retorna a quantidade de linhas lido. */
}

void carregarDados(){ /* Faz a leitura dos .txt de cada tipo, armazena e diz o total. */
    totalNome = carregarArquivo("nomes.txt", nomes, MAX_DADOS);
    totalCPF = carregarArquivo("cpfs.txt", cpfs, MAX_DADOS);
    totalEmail = carregarArquivo("emails.txt", emails, MAX_DADOS);
    totalBairro = carregarArquivo("bairros.txt", bairros, MAX_DADOS);
    nomeServicoss = carregarArquivo("servicosNome.txt", nomeServicos, MAX_DADOS);
    enderecoss = carregarArquivo("enderecos.txt", enderecos, MAX_DADOS);

    printf("\nDados carregados: Nomes: %d, CPFs: %d, Emails: %d, Bairros: %d, Nomes de Servicos: %d, Enderecos: %d\n",
           totalNome, totalCPF, totalEmail, totalBairro, nomeServicoss, enderecoss);
}

void exibirTudo(struct Pilha *p){
    printf("\nExibindo todos bairros registrados: \n");
    exibirBairrosRegistrados();
    printf("\nExibindo todos cidadaos registrados: \n");
    exibirCidadoesRegistrados();
    printf("\nExibindo todas ocorrencias registradas na fila: \n");
    exibirOcorrencias();
    printf("\nExibindo todos servicos registrados: \n");
    exibirServicosRegistrados();
    printf("\nExibindo Pilha de Historico de Atendimentos: \n");
    imprimePilha(p);
    printf("\n\n");
}

struct Ocorrencia* criarCopiaOcorrencia(struct Ocorrencia* original) {
    if (original == NULL) return NULL;
    struct Ocorrencia* copia = malloc(sizeof(struct Ocorrencia));
    if (copia != NULL) {
        copia->id = original->id;
        strcpy(copia->cpfCidadao, original->cpfCidadao);
        copia->gravidade = original->gravidade;
        copia->tipo = original->tipo;
        copia->IdDoBairro = original->IdDoBairro;
        copia->prox = NULL; // Importante: nao copiar o ponteiro prox
    }
    return copia;
}

// Cria e inicializa um novo nó para a BST.
struct NoArvoreBST* criarNoBST(struct Ocorrencia* ocorrencia) {
    struct NoArvoreBST* novoNo = (struct NoArvoreBST*)malloc(sizeof(struct NoArvoreBST)); //
    if (novoNo == NULL) { //
        perror("Falha ao alocar memoria para no da BST"); //
        exit(EXIT_FAILURE); //
    }
    novoNo->ocorrencia = criarCopiaOcorrencia(ocorrencia); //
    novoNo->esquerda = NULL; //
    novoNo->direita = NULL; //
    return novoNo; //
}

// Insere uma nova ocorrência na BST, mantendo a ordenação.
struct NoArvoreBST* inserirBST(struct NoArvoreBST* raiz, struct Ocorrencia* novaOcorrencia, int ordenarPor) {
    if (raiz == NULL) { //
        return criarNoBST(novaOcorrencia); //
    }

    if (novaOcorrencia->id < raiz->ocorrencia->id) { //
        raiz->esquerda = inserirBST(raiz->esquerda, novaOcorrencia, ordenarPor); //
    } else if (novaOcorrencia->id > raiz->ocorrencia->id) { //
        raiz->direita = inserirBST(raiz->direita, novaOcorrencia, ordenarPor); //
    }
    return raiz; //
}

// Percorre a BST em ordem (crescente de ID) e exibe as ocorrências.
void percursoEmOrdemBST(struct NoArvoreBST* raiz) {
    if (raiz != NULL) { //
        percursoEmOrdemBST(raiz->esquerda); //
        exibeOcorrencia(raiz->ocorrencia); //
        percursoEmOrdemBST(raiz->direita); //
    }
}

// Busca uma ocorrência na BST pelo ID.
struct NoArvoreBST* buscarBST(struct NoArvoreBST* raiz, int id) {
    if (raiz == NULL || raiz->ocorrencia->id == id) { //
        return raiz; //
    }
    if (id < raiz->ocorrencia->id) { //
        return buscarBST(raiz->esquerda, id); //
    } else { //
        return buscarBST(raiz->direita, id); //
    }
}

// Libera toda a memória da BST.
void liberarBST(struct NoArvoreBST* raiz) {
    if (raiz != NULL) { //
        liberarBST(raiz->esquerda); //
        liberarBST(raiz->direita); //
        free(raiz->ocorrencia); //
        free(raiz); //
    }
}


// Implementação das funções da Árvore AVL.

// Retorna a altura de um nó AVL.
int altura(struct NoArvoreAVL *N) {
    if (N == NULL) //
        return 0; //
    return N->altura; //
}

// Retorna o maior entre dois inteiros.
int max(int a, int b) { //
    return (a > b) ? a : b; //
}

// Cria e inicializa um novo nó para a AVL.
struct NoArvoreAVL* criarNoAVL(struct Ocorrencia* ocorrencia) {
    struct NoArvoreAVL* no = (struct NoArvoreAVL*)malloc(sizeof(struct NoArvoreAVL)); //
    if (no == NULL) { //
        perror("Falha ao alocar memoria para no da AVL"); //
        exit(EXIT_FAILURE); //
    }
    no->ocorrencia = criarCopiaOcorrencia(ocorrencia); //
    no->esquerda = NULL; //
    no->direita = NULL; //
    no->altura = 1; //
    return no; //
}

// Realiza uma rotação simples à direita para balancear a AVL.
struct NoArvoreAVL *rotacionarDireita(struct NoArvoreAVL *y) {
    struct NoArvoreAVL *x = y->esquerda;
    struct NoArvoreAVL *T2 = x->direita;

    x->direita = y; //
    y->esquerda = T2; //

    y->altura = max(altura(y->esquerda), altura(y->direita)) + 1;
    x->altura = max(altura(x->esquerda), altura(x->direita)) + 1;

    return x; //
}

// Realiza uma rotação simples à esquerda para balancear a AVL.
struct NoArvoreAVL *rotacionarEsquerda(struct NoArvoreAVL *x) {
    struct NoArvoreAVL *y = x->direita;
    struct NoArvoreAVL *T2 = y->esquerda;

    y->esquerda = x;
    x->direita = T2;

    x->altura = max(altura(x->esquerda), altura(x->direita)) + 1;
    y->altura = max(altura(y->esquerda), altura(y->direita)) + 1;

    return y;
}

// Calcula o fator de balanceamento de um nó AVL.
int obterBalanceamento(struct NoArvoreAVL *N) {
    if (N == NULL)
        return 0;
    return altura(N->esquerda) - altura(N->direita);
}

// Insere uma nova ocorrência na AVL e rebalanceia se necessário.
struct NoArvoreAVL* inserirAVL(struct NoArvoreAVL* no, struct Ocorrencia* novaOcorrencia) {
    if (no == NULL)
        return criarNoAVL(novaOcorrencia);

    if (novaOcorrencia->gravidade < no->ocorrencia->gravidade) {
        no->esquerda = inserirAVL(no->esquerda, novaOcorrencia);
    } else if (novaOcorrencia->gravidade > no->ocorrencia->gravidade) {
        no->direita = inserirAVL(no->direita, novaOcorrencia); //
    } else { // Trata ocorrências com a mesma gravidade, usando ID como desempate.
        if (novaOcorrencia->id < no->ocorrencia->id) {
            no->esquerda = inserirAVL(no->esquerda, novaOcorrencia);
        } else if (novaOcorrencia->id > no->ocorrencia->id) {
            no->direita = inserirAVL(no->direita, novaOcorrencia);
        } else {
            return no;
        }
    }

    no->altura = 1 + max(altura(no->esquerda), altura(no->direita)); // Atualiza a altura do nó.

    int balanceamento = obterBalanceamento(no); // Calcula o balanceamento.

    // Casos de rotação para rebalanceamento.
    if (balanceamento > 1 && novaOcorrencia->gravidade < no->esquerda->ocorrencia->gravidade) // Esquerda-Esquerda.
        return rotacionarDireita(no);

    if (balanceamento < -1 && novaOcorrencia->gravidade > no->direita->ocorrencia->gravidade) // Direita-Direita.
        return rotacionarEsquerda(no);

    if (balanceamento > 1 && novaOcorrencia->gravidade > no->esquerda->ocorrencia->gravidade) { // Esquerda-Direita.
        no->esquerda = rotacionarEsquerda(no->esquerda);
        return rotacionarDireita(no);
    }

    if (balanceamento < -1 && novaOcorrencia->gravidade < no->direita->ocorrencia->gravidade) { // Direita-Esquerda.
        no->direita = rotacionarDireita(no->direita);
        return rotacionarEsquerda(no);
    }

    return no;
}

// Percorre a AVL em ordem (crescente de gravidade) e exibe as ocorrências.
void percursoEmOrdemAVL(struct NoArvoreAVL* raiz) {
    if (raiz != NULL) {
        percursoEmOrdemAVL(raiz->esquerda);
        exibeOcorrencia(raiz->ocorrencia);
        percursoEmOrdemAVL(raiz->direita);
    }
}

// Busca uma ocorrência na AVL pela gravidade.
struct NoArvoreAVL* buscarAVL(struct NoArvoreAVL* raiz, int gravidade) {
    if (raiz == NULL || raiz->ocorrencia->gravidade == gravidade) {
        return raiz;
    }
    if (gravidade < raiz->ocorrencia->gravidade) {
        return buscarAVL(raiz->esquerda, gravidade);
    } else { //
        return buscarAVL(raiz->direita, gravidade);
    }
}

// Libera toda a memória da AVL.
void liberarAVL(struct NoArvoreAVL* raiz) {
    if (raiz != NULL) {
        liberarAVL(raiz->esquerda);
        liberarAVL(raiz->direita);
        free(raiz->ocorrencia);
        free(raiz);
    }
}

void simulacao(struct Pilha *pilha_atendimentos){ /* Vamos criar determinadas situacoes conforme o valor cair em coisas diferentes. */
    srand(time(NULL));
    carregarDados();
    int contadores[6]; // 0:nomes, 1:cpfs, 2:emails, 3:bairros, 4:nomeServicos, 5:enderecos
    inicializa(pilha_atendimentos); // Inicializa a pilha antes da simulacao

    int idsBairros = 0;
    int idsServicos = 0;
    int idsOcorrencias = 0;

    for(int i = 0; i < 6; i++){ /* Zerando os contadores. */
        contadores[i] = 0;
    }

    // Inicializa a tabela hash de bairros e cidadaos (se nao foi feita no main)
    inicializaTabela(); // Chama a funcao de inicializacao das tabelas hash

    int cont = 0;
    while (cont < 10) {
        int prioridade = rand() % 6; // Gravidade de 0 a 5
        int tipoOcorrencia = (rand() % 3) + 1; // 1:ambulancia, 2:bombeiro, 3:policia
        int numeroAle = rand() % 100;

        // Resetar contadores se atingirem o limite dos dados
        if (contadores[0] >= totalNome) contadores[0] = 0;
        if (contadores[1] >= totalCPF) contadores[1] = 0;
        if (contadores[2] >= totalEmail) contadores[2] = 0;
        if (contadores[3] >= totalBairro) contadores[3] = 0;
        if (contadores[4] >= nomeServicoss) contadores[4] = 0;
        if (contadores[5] >= enderecoss) contadores[5] = 0;


        if (numeroAle < 10){ /* 10 possibilidades diferentes. */
            printf("\nTempo: %d. Cadastro completo. \n", tempoAtual);

            char *bairro_nome = bairros[contadores[3]++];
            cadastrarBairro(idsBairros, bairro_nome); // Use idsBairros antes de incrementar para o cadastro
            idsBairros++; // Incrementa depois de usar

            char *cpf = cpfs[contadores[1]++];
            char *nome = nomes[contadores[0]++];
            char *email = emails[contadores[2]++];
            char *endereco_cidadao = enderecos[contadores[5]++];
            cadastrarCidadao(cpf, nome, email, endereco_cidadao, bairro_nome);

            char *nomeServico_cadastro = nomeServicos[contadores[4]++];
            char *endereco_servico = enderecos[contadores[5]++]; // Pode ser o mesmo do cidadao ou outro
            cadastrarServico(idsServicos, tipoOcorrencia, nomeServico_cadastro, endereco_servico, bairro_nome);
            idsServicos++;

        }
        else if (numeroAle < 20) {
            printf("\nTempo: %d. Cadastro de cidadao. \n", tempoAtual);
            char *cpf = cpfs[contadores[1]++];
            char *nome = nomes[contadores[0]++];
            char *email = emails[contadores[2]++];
            char *endereco_cidadao = enderecos[contadores[5]++];
            char *bairro_cidadao = bairros[contadores[3]++]; // Assegura que o bairro exista se for usado
            cadastrarCidadao(cpf, nome, email, endereco_cidadao, bairro_cidadao);
        }
        else if (numeroAle < 30) {
            printf("\nTempo: %d. Cadastro completo, com servico fixo. \n", tempoAtual);
            char *bairro_nome = bairros[contadores[3]++];
            cadastrarBairro(idsBairros, bairro_nome);
            idsBairros++;

            char *cpf = cpfs[contadores[1]++];
            char *nome = nomes[contadores[0]++];
            char *email = emails[contadores[2]++];
            char *endereco_cidadao = enderecos[contadores[5]++];
            cadastrarCidadao(cpf, nome, email, endereco_cidadao, bairro_nome);

            char *nomeServico_cadastro = nomeServicos[contadores[4]++];
            char *endereco_servico = enderecos[contadores[5]++];
            cadastrarServico(idsServicos, 2, nomeServico_cadastro, endereco_servico, bairro_nome); // Tipo 2 fixo (bombeiro)
            idsServicos++;

        }
        else if (numeroAle < 40) {
            printf("\nTempo: %d. Processando ocorrencia e cadastrando cidadao. \n", tempoAtual);
            processarOcorrencia();

            char *cpf = cpfs[contadores[1]++];
            char *nome = nomes[contadores[0]++];
            char *email = emails[contadores[2]++];
            char *endereco_cidadao = enderecos[contadores[5]++];
            char *bairro_cidadao = bairros[contadores[3]++];
            cadastrarCidadao(cpf, nome, email, endereco_cidadao, bairro_cidadao);
        }
        else if (numeroAle < 50) {
            printf("\nTempo: %d. Processando ocorrencia e registrando uma nova. \n", tempoAtual);
            processarOcorrencia();

            char *cpf = cpfs[contadores[1]++];
            // Tentar obter um ID de bairro ja existente ou criar um novo
            int idBairro = idsBairros % totalBairro; // Reutiliza IDs de bairro para nao crescer indefinidamente
            if (buscarBairroPorID(idBairro) == NULL && totalBairro > 0) { // Se o bairro nao existe e temos dados carregados
                cadastrarBairro(idBairro, bairros[idBairro]); // Cadastra o bairro se nao existe
            } else if (totalBairro == 0) { // Se nao houver bairros carregados, usa um ID fixo ou gera um novo
                idBairro = idsBairros++;
                cadastrarBairro(idBairro, "Bairro Generico"); // Cadastra um bairro generico
            }

            struct Ocorrencia *p = registrarOcorrencia(idsOcorrencias, cpf, prioridade, tipoOcorrencia, idBairro);
            if (p != NULL) {
                struct Ocorrencia *copia = criarCopiaOcorrencia(p);
                push(pilha_atendimentos, copia);
            }
            idsOcorrencias++;
        }
        else if (numeroAle < 60) {
            printf("\nTempo: %d. Registrando ocorrencia medica. \n", tempoAtual);
            char *cpf = cpfs[contadores[1]++];
            int idBairro = idsBairros % totalBairro; // Reutiliza IDs de bairro
            if (buscarBairroPorID(idBairro) == NULL && totalBairro > 0) {
                cadastrarBairro(idBairro, bairros[idBairro]);
            } else if (totalBairro == 0) {
                idBairro = idsBairros++;
                cadastrarBairro(idBairro, "Bairro Generico");
            }
            struct Ocorrencia *p = registrarOcorrencia(idsOcorrencias, cpf, prioridade, 1, idBairro); // Tipo 1 (ambulancia)
            if (p != NULL) {
                struct Ocorrencia *copia = criarCopiaOcorrencia(p);
                push(pilha_atendimentos, copia);
            }
            idsOcorrencias++;
        }
        else if (numeroAle < 70) {
            printf("\nTempo: %d. Registrando ocorrencia policial. \n", tempoAtual);
            char *cpf = cpfs[contadores[1]++];
            int idBairro = idsBairros % totalBairro; // Reutiliza IDs de bairro
            if (buscarBairroPorID(idBairro) == NULL && totalBairro > 0) {
                cadastrarBairro(idBairro, bairros[idBairro]);
            } else if (totalBairro == 0) {
                idBairro = idsBairros++;
                cadastrarBairro(idBairro, "Bairro Generico");
            }
            struct Ocorrencia *p = registrarOcorrencia(idsOcorrencias, cpf, prioridade, 3, idBairro); // Tipo 3 (policia)
            if (p != NULL) {
                struct Ocorrencia *copia = criarCopiaOcorrencia(p);
                push(pilha_atendimentos, copia);
            }
            idsOcorrencias++;
        }
        else if (numeroAle < 80) {
            printf("\nTempo: %d. Processando ocorrencia e cadastro completo. \n", tempoAtual);
            processarOcorrencia();

            char *bairro_nome = bairros[contadores[3]++];
            cadastrarBairro(idsBairros, bairro_nome);
            idsBairros++;

            char *cpf = cpfs[contadores[1]++];
            char *nome = nomes[contadores[0]++];
            char *email = emails[contadores[2]++];
            char *endereco_cidadao = enderecos[contadores[5]++];
            cadastrarCidadao(cpf, nome, email, endereco_cidadao, bairro_nome);

            char *nomeServico_cadastro = nomeServicos[contadores[4]++];
            char *endereco_servico = enderecos[contadores[5]++];
            cadastrarServico(idsServicos, tipoOcorrencia, nomeServico_cadastro, endereco_servico, bairro_nome);
            idsServicos++;
        }
        else if (numeroAle < 90) {
            printf("\nTempo: %d. Processando ocorrencia e cadastrando cidadao. \n", tempoAtual);
            processarOcorrencia();

            char *cpf = cpfs[contadores[1]++];
            char *nome = nomes[contadores[0]++];
            char *email = emails[contadores[2]++];
            char *endereco_cidadao = enderecos[contadores[5]++];
            char *bairro_cidadao = bairros[contadores[3]++];
            cadastrarCidadao(cpf, nome, email, endereco_cidadao, bairro_cidadao);
        }
        else {
            printf("\nTempo: %d. Cadastrando equipe medica e registrando ocorrencia. \n", tempoAtual);
            char *cpf = cpfs[contadores[1]++];
            int idBairro = idsBairros % totalBairro; // Reutiliza IDs de bairro
            if (buscarBairroPorID(idBairro) == NULL && totalBairro > 0) {
                cadastrarBairro(idBairro, bairros[idBairro]);
            } else if (totalBairro == 0) {
                idBairro = idsBairros++;
                cadastrarBairro(idBairro, "Bairro Generico");
            }
            struct Ocorrencia *p = registrarOcorrencia(idsOcorrencias, cpf, prioridade, 1, idBairro); // Tipo 1 (ambulancia)
            if (p != NULL) {
                struct Ocorrencia *copia = criarCopiaOcorrencia(p);
                push(pilha_atendimentos, copia);
            }
            idsOcorrencias++;
        }

        cont++;
    }
    printf("\n\n\n======================================================\n");
    printf("Exibindo os resultados da simulacao %d: \n", simulacoes);
    printf("======================================================\n\n\n");
    exibirTudo(pilha_atendimentos);
    simulacoes++;
}
