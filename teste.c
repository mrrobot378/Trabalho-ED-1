#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "EstruturasCodigos.c"
/*
Teste de verificacao.
*/

void menu() {
    printf("\nMenu\n\n");
    printf("1- Registrar o bairro.\n");
    printf("2- Registrar pessoa.\n");
    printf("3- Registrar servico.\n");
    printf("4- Registrar ocorrencia.\n");
    printf("5- Remover ocorrencia.\n");
    printf("6- Ver bairros.\n");
    printf("7- Ver pessoas.\n");
    printf("8- Ver servicos.\n");
    printf("9- Ver fila de ocorrencias.\n");
    printf("10- Simulacao.\n");
    printf("11 - Ver Historico de Ocorrencias.\n");
    printf("12- Ver servicos em um bairro.\n");
    printf("13- Ver bairros de um servico.\n");
    printf("14- Ver ocorrencias ordenadas por ID (BST).\n");
    printf("15- Ver ocorrencias ordenadas por gravidade (AVL).\n");
    printf("16- Buscar ocorrencia por ID (BST).\n");
    printf("17- Buscar ocorrencia por gravidade (AVL).\n");
    printf("0- Sair.\n");
    printf("Tempo atual: %d.\n", tempoAtual);

    printf("\nDigite a opcao desejada: ");
}

int main() {
    setlocale(LC_ALL, "Portuguese");

    inicializaTabela();
    inicializarListasCruzadas();

    int opcao;
    struct Pilha pilha_atendimento;

    do {
        menu();
        scanf("%d", &opcao);
        getchar(); // Limpar buffer

        switch (opcao) {
            case 1: {
                int id;
                char nome[30];

                printf("Digite o ID do bairro: ");
                scanf("%d", &id);
                getchar();

                printf("Digite o nome do bairro: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';

                cadastrarBairro(id, nome);
                break;
            }

            case 2: {
                char CPF[20], nome[30], email[30], endereco[30], bairro[30];

                printf("Digite o CPF: ");
                fgets(CPF, sizeof(CPF), stdin);
                CPF[strcspn(CPF, "\n")] = '\0';

                printf("Digite o nome: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';

                printf("Digite o email: ");
                fgets(email, sizeof(email), stdin);
                email[strcspn(email, "\n")] = '\0';

                printf("Digite o endereco: ");
                fgets(endereco, sizeof(endereco), stdin);
                endereco[strcspn(endereco, "\n")] = '\0';

                printf("Digite o bairro: ");
                fgets(bairro, sizeof(bairro), stdin);
                bairro[strcspn(bairro, "\n")] = '\0';

                cadastrarCidadao(CPF, nome, email, endereco, bairro);
                break;
            }

            case 3: {
                int id, tipo;
                char nome[50], endereco[50], bairro[30];

                printf("Digite o ID do servico: ");
                scanf("%d", &id);
                getchar();

                printf("Digite o tipo de servico (1 = ambulancia, 2 = bombeiro, 3 = policia): ");
                scanf("%d", &tipo);
                getchar();

                printf("Digite o nome da unidade: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';

                printf("Digite o endereco da unidade: ");
                fgets(endereco, sizeof(endereco), stdin);
                endereco[strcspn(endereco, "\n")] = '\0';

                printf("Digite bairro da unidade: ");
                fgets(bairro, sizeof(bairro), stdin);
                bairro[strcspn(bairro, "\n")] = '\0';

                cadastrarServico(id, tipo, nome, endereco, bairro);
                break;
            }

            case 4: {
                int idOcorrencia, prioridade, tipo, bairroId;
                char cpf[20];

                printf("Digite o ID da ocorrencia: ");
                scanf("%d", &idOcorrencia);
                getchar();

                printf("Digite o CPF do cidadao solicitante: ");
                fgets(cpf, sizeof(cpf), stdin);
                cpf[strcspn(cpf, "\n")] = '\0';

                printf("Digite o nivel de gravidade (0 a 5): ");
                scanf("%d", &prioridade);
                getchar();

                printf("Digite o tipo da ocorrencia (1 = ambulancia, 2 = bombeiro, 3 = policia): ");
                scanf("%d", &tipo);
                getchar();

                printf("Digite o ID do bairro alvo: ");
                scanf("%d", &bairroId);
                getchar();

                struct Ocorrencia *p = registrarOcorrencia(idOcorrencia, cpf, prioridade, tipo, bairroId);
                // Criar uma copia antes de empilhar, pois a ocorrencia 'p' pode ser liberada da fila.
                struct Ocorrencia *copia_para_pilha = criarCopiaOcorrencia(p);
                push(&pilha_atendimento, copia_para_pilha);
                break;
            }

            case 5: {
                processarOcorrencia(); // Remove a ocorrencia de maior prioridade
                break;
            }

            case 6: {
                int op;
                printf("1- Exibir todos os bairros\n");
                printf("2- buscar bairro por ID\n");
                scanf("%d", &op);
                if(op == 1){
                exibirBairrosRegistrados();
                }
                else if(op == 2){
                    buscarPorId();
                }
                else{
                    printf("Opcao invalida");
                }
                break;
            }

            case 7: {
                int op;
                printf("1- Exibir todos os cidadaos\n");
                printf("2- buscar por CPF\n");
                scanf("%d", &op);
                if(op == 1){
                  exibirCidadoesRegistrados();
                }
                else if(op == 2){
                    buscarCidadaoCPF();
                }
                else{
                    printf("Opcao invalida");
                }
                break;
            }

            case 8: {
                exibirServicosRegistrados();
                break;
            }

            case 9: {
                exibirOcorrencias();
                break;
            }

            case 10: {
                printf("\nIniciando simulacao. Ira ocorrer 10 coisas aleatorias: \n\n");
                simulacao(&pilha_atendimento);
                break;
            }

            case 11:
                imprimePilha(&pilha_atendimento);
            break;

            case 12: {
                int idBairro;
                printf("Digite o ID do bairro: ");
                scanf("%d", &idBairro);
                exibirServicosNoBairro(idBairro);
                break;
             }

            case 13: {
                int idServico;
                printf("Digite o ID do servico: ");
                scanf("%d", &idServico);
                exibirBairrosDoServico(idServico);
                break;
            }

            case 14: {
                printf("\nOcorrencias ordenadas por ID (BST):\n");
                percursoEmOrdemBST(ocorrenciasBST);
                break;
            }

            case 15: {
                printf("\nOcorrencias ordenadas por gravidade (AVL):\n");
                percursoEmOrdemAVL(ocorrenciasAVL);
                break;
            }

            case 16: {
                int idBusca;
                printf("Digite o ID da ocorrencia a buscar: ");
                scanf("%d", &idBusca);
                struct NoArvoreBST* resultado = buscarBST(ocorrenciasBST, idBusca);
                if (resultado != NULL) {
                    printf("Ocorrencia encontrada (BST):\n");
                    exibeOcorrencia(resultado->ocorrencia);
                } else {
                    printf("Ocorrencia com ID %d nao encontrada na BST.\n", idBusca);
                }
                break;
            }

            case 17: {
                int gravidadeBusca;
                printf("Digite a gravidade da ocorrencia a buscar: ");
                scanf("%d", &gravidadeBusca);
                struct NoArvoreAVL* resultado = buscarAVL(ocorrenciasAVL, gravidadeBusca);
                if (resultado != NULL) {
                    printf("Ocorrencia encontrada (AVL):\n");
                    exibeOcorrencia(resultado->ocorrencia);
                } else {
                    printf("Ocorrencia com gravidade %d nao encontrada na AVL.\n", gravidadeBusca);
                }
                break;
            }


            case 0: {
                printf("\n\nPrograma encerrado.\n\n");
                limparTudo(&pilha_atendimento);
                break;
            }

            default:
                printf("Opcao invalida! Tente novamente.\n");
        }

    } while (opcao != 0);

    system("pause");
    return 0;
}
