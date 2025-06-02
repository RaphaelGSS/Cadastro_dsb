#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
  #define strcasecmp _stricmp
#endif

#define MAX_STRING 100

// Níveis de acesso
#define COLABORADOR 0
#define SUPERVISOR 1
#define GERENTE 2
#define DONO 3

// Preços base para os lanches (por receita, sem adicionais)
double precos[] = {25.00, 35.00, 30.00, 28.00, 20.00, 30.00, 20.00, 15.00, 9.00, 6.00, 7.00, 9.00, 10.00, 12.00};

// ==================== FUNÇÕES AUXILIARES ====================

// Converte uma string para minúsculas.
void toLowerCase(char *str) {
    for (int i = 0; str[i] != '\0'; i++)
        str[i] = tolower((unsigned char)str[i]);
}

// Criptografa a senha (XOR com chave fixa e converte para hexadecimal).
void encryptPassword(const char* input, char* output) {
    int key = 0xAA;  // chave fixa para exemplo
    size_t len = strlen(input);
    for (size_t i = 0; i < len; i++) {
        sprintf(output + (i * 2), "%02X", (unsigned char)(input[i] ^ key));
    }
    output[len * 2] = '\0';
}

// Descriptografa a senha (faz o processo inverso).
void decryptPassword(const char* encrypted, char* output) {
    int key = 0xAA;
    size_t len = strlen(encrypted) / 2;
    for (size_t i = 0; i < len; i++) {
        char buffer[3];
        buffer[0] = encrypted[i * 2];
        buffer[1] = encrypted[i * 2 + 1];
        buffer[2] = '\0';
        int value;
        sscanf(buffer, "%x", &value);
        output[i] = (char)(value ^ key);
    }
    output[len] = '\0';
}

// ==================== FUNÇÕES DE USUÁRIO ====================

// Cadastra o usuário inicial (grava em "usuarios.txt").
void cadastrarUsuario() {
    FILE *file = fopen("usuarios.txt", "a");
    if (file == NULL) {
        printf("Erro ao criar/abrir o arquivo de usuarios.\n");
        return;
    }
    char login[MAX_STRING], senha[MAX_STRING], senhaCriptografada[MAX_STRING * 2];
    int role;
    printf("=== Cadastro de Usuario ===\n");
    printf("Login (primeiro nome): ");
    scanf(" %[^\n]", login);
    toLowerCase(login);
    printf("Senha: ");
    scanf(" %[^\n]", senha);
    printf("Nivel de acesso (1 - Supervisor, 2 - Gerente, 3 - Dono): ");
    scanf("%d", &role);
    encryptPassword(senha, senhaCriptografada);
    fprintf(file, "%s,%s,%d\n", login, senhaCriptografada, role);
    fclose(file);
    printf("Usuario cadastrado com sucesso!\n");
}

// Autentica o usuário; retorna o nível de acesso ou 0 se falhar.
int autenticar() {
    char login[MAX_STRING], senha[MAX_STRING];
    char linhaArquivo[MAX_STRING];
    char fileLogin[MAX_STRING], fileEncrypted[MAX_STRING * 2], decryptedPassword[MAX_STRING];
    int fileRole = 0;
    int encontrado = 0;

    printf("Login (primeiro nome): ");
    scanf(" %[^\n]", login);
    toLowerCase(login);
    printf("Senha: ");
    scanf(" %[^\n]", senha);

    FILE *file = fopen("usuarios.txt", "r");
    if (file == NULL) {
        // Não há usuários cadastrados ainda
        printf("Nenhum usuario cadastrado. Por favor, efetue o cadastro.\n");
        cadastrarUsuario();
        return autenticar();  // Retenta a autenticação após o cadastro
    }

    while (fgets(linhaArquivo, MAX_STRING, file)) {
        char *token = strtok(linhaArquivo, ",");
        if (token != NULL) {
            strcpy(fileLogin, token);
            toLowerCase(fileLogin);
        }
        token = strtok(NULL, ",");
        if (token != NULL) {
            strcpy(fileEncrypted, token);
            fileEncrypted[strcspn(fileEncrypted, "\n")] = '\0';
        }
        token = strtok(NULL, ",\n");
        if (token != NULL) {
            fileRole = atoi(token);
        }
        decryptPassword(fileEncrypted, decryptedPassword);
        if (strcmp(login, fileLogin) == 0 && strcmp(senha, decryptedPassword) == 0) {
            encontrado = 1;
            break;
        }
    }
    fclose(file);

    if (!encontrado) {
        char opcao;
        printf("Usuario nao cadastrado ou senha incorreta.\nDeseja se cadastrar? (S/N): ");
        scanf(" %c", &opcao);
        if (tolower(opcao) == 's') {
            cadastrarUsuario();
            return autenticar();  // Retentar a autenticação após o cadastro
        } else {
            return 0;
        }
    }

    printf("Autenticacao realizada com sucesso!\n");
    return fileRole;
}


// ==================== FUNÇÕES DE PERSISTÊNCIA ====================

// Funcionários
void salvarFuncionario() {
    FILE *file = fopen("funcionarios.txt", "a");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de funcionarios!\n");
        return;
    }
    char nome[MAX_STRING], cargo[MAX_STRING];
    printf("Nome do funcionario: ");
    scanf(" %[^\n]", nome);
    printf("Cargo: ");
    scanf(" %[^\n]", cargo);
    fprintf(file, "%s,%s\n", nome, cargo);
    fclose(file);
    printf("Funcionario cadastrado com sucesso!\n");
}

void listarFuncionarios() {
    FILE *file = fopen("funcionarios.txt", "r");
    if (file == NULL) {
        printf("Nenhum funcionario cadastrado ainda.\n");
        return;
    }
    char linha[MAX_STRING];
    printf("\n==== Funcionarios Cadastrados ====\n");
    while (fgets(linha, MAX_STRING, file))
        printf("%s", linha);
    printf("===============================\n");
    fclose(file);
}

// Estoque
void salvarProdutoEstoque() {
    FILE *file = fopen("estoque.txt", "a");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de estoque!\n");
        return;
    }
    char nomeProduto[MAX_STRING];
    int quantidade;
    printf("Nome do produto: ");
    scanf(" %[^\n]", nomeProduto);
    printf("Quantidade disponivel: ");
    scanf("%d", &quantidade);
    fprintf(file, "%s,%d\n", nomeProduto, quantidade);
    fclose(file);
    printf("Produto cadastrado no estoque com sucesso!\n");
}

void listarEstoque() {
    FILE *file = fopen("estoque.txt", "r");
    if (file == NULL) {
        printf("Nenhum produto cadastrado ainda.\n");
        return;
    }
    char linha[MAX_STRING];
    printf("\n==== Estoque ====\n");
    while (fgets(linha, MAX_STRING, file))
        printf("%s", linha);
    printf("==================\n");
    fclose(file);
}

// Fluxo de Caixa
void registrarVenda(double valor) {
    FILE *fluxo = fopen("fluxocaixa.txt", "a");
    if (fluxo == NULL) {
        printf("Erro ao abrir o arquivo de fluxo de caixa!\n");
        return;
    }
    fprintf(fluxo, "Hamburguer,%.2lf\n", valor);
    fclose(fluxo);
}

void mostrarFluxoCaixa() {
    FILE *fluxo = fopen("fluxocaixa.txt", "r");
    if (fluxo == NULL) {
        printf("Nenhuma venda registrada.\n");
        return;
    }
    char linha[MAX_STRING];
    double total = 0.0, valor;
    char descricao[MAX_STRING];
    printf("\n=== Fluxo de Caixa ===\n");
    while (fgets(linha, MAX_STRING, fluxo)) {
        sscanf(linha, "%[^,],%lf", descricao, &valor);
        printf("Venda: %s - R$ %.2lf\n", descricao, valor);
        total += valor;
    }
    printf("Total de vendas: R$ %.2lf\n", total);
    printf("======================\n");
    fclose(fluxo);
}

// Pedido Genérico (ate o momento implementado anteriormente)
void realizarBaixadeEstoque() {
    FILE *file = fopen("pedidos.txt", "a");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de pedidos!\n");
        return;
    }
    char clienteNome[MAX_STRING], descricao[MAX_STRING], produtoNome[MAX_STRING];
    int quantidadePedido;

    printf("Nome do cliente: ");
    scanf(" %[^\n]", clienteNome);
    printf("Descricao do pedido: ");
    scanf(" %[^\n]", descricao);
    printf("Nome do produto no estoque: ");
    scanf(" %[^\n]", produtoNome);
    printf("Quantidade desejada: ");
    scanf("%d", &quantidadePedido);

    fprintf(file, "%s,%s,%s,%d\n", clienteNome, descricao, produtoNome, quantidadePedido);
    fclose(file);
    printf("Pedido realizado com sucesso!\n");

    // Atualizacao do estoque para pedido genérico (sem receita padrao)
    FILE *estoqueFile = fopen("estoque.txt", "r");
    FILE *tempFile = fopen("estoque_temp.txt", "w");
    if (estoqueFile == NULL || tempFile == NULL) {
        printf("Erro ao acessar o estoque para atualizacao!\n");
        return;
    }
    char linha[MAX_STRING], nomeProdTemp[MAX_STRING];
    int quantidadeAtual, encontrado = 0;
    while (fgets(linha, MAX_STRING, estoqueFile)) {
        sscanf(linha, "%[^,],%d", nomeProdTemp, &quantidadeAtual);
        if (strcmp(nomeProdTemp, produtoNome) == 0 && quantidadeAtual >= quantidadePedido) {
            quantidadeAtual -= quantidadePedido;
            encontrado = 1;
        }
        fprintf(tempFile, "%s,%d\n", nomeProdTemp, quantidadeAtual);
    }
    fclose(estoqueFile);
    fclose(tempFile);
    if (encontrado) {
        remove("estoque.txt");
        rename("estoque_temp.txt", "estoque.txt");
        printf("Estoque atualizado!\n");
    } else {
        remove("estoque_temp.txt");
        printf("Produto nao encontrado ou estoque insuficiente!\n");
    }
}

// ==================== DEFINIÇÃO DE RECEITAS ====================

typedef struct {
    char nome[MAX_STRING];
    int quantidade; // Quantidade utilizada (g, unidades ou outros)
} Ingrediente;

typedef struct {
    char nome[MAX_STRING];
    int numIngredientes;
    Ingrediente ingredientes[10];   // Suporta até 10 ingredientes
} Receita;

Receita receitas[] = {
    { "Smash Burguer Clasico", 8, {
        {"Carne", 80},
        {"Cebola em cubos", 10},
        {"Queijo cheddar", 1},
        {"Alface americana", 2},
        {"Picles em rodelas", 3},
        {"Pao brioche", 1},
        {"Ketchup", 5},
        {"Mostarda", 5}
    }},
    { "Smash Burguer Duplo", 8, {
        {"Carne", 160},
        {"Cebola em cubos", 10},
        {"Queijo cheddar", 2},
        {"Alface americana", 2},
        {"Picles em rodelas", 3},
        {"Pao brioche", 1},
        {"Ketchup", 5},
        {"Mostarda", 5}
    }},
    { "Smash Burguer com Bacon", 9, {
        {"Carne", 80},
        {"Cebola em cubos", 10},
        {"Queijo cheddar", 1},
        {"Alface americana", 2},
        {"Picles em rodelas", 3},
        {"Pao brioche", 1},
        {"Ketchup", 5},
        {"Mostarda", 5},
        {"Bacon em fatias", 2}
    }},
    { "Smash Burguer Especial", 10, {
        {"Carne", 160},
        {"Cebola em cubos", 10},
        {"Queijo cheddar", 2},
        {"Bacon em fatias", 4},
        {"Alface americana", 2},
        {"Tomate", 2},
        {"Pao australiano", 1},
        {"Ketchup", 5},
        {"Maionese verde", 5},
        {"Mostarda", 5}
    }},
    { "Smash Veggie", 8, {
        {"Burger de grao-de-bico", 1},
        {"Cebola em cubos", 10},
        {"Alface americana", 3},
        {"Picles em rodelas", 3},
        {"Tomate", 2},
        {"Pao brioche", 1},
        {"Ketchup", 5},
        {"Mostarda", 5}
    }},
    { "Batata suprema", 3, {
        {"Batata frita", 150},
        {"Bacon em cubos", 50},
        {"Queijo mussarela", 4}
    }},
    { "Onion rings", 1, {
        {"Onion rings", 150}
    }},
    { "Batata frita", 1, {
        {"Batata frita", 150}
    }},
    { "Coca-Cola 600 ml", 1, {
        {"Coca-Cola 600 ml", 1}
    }},
    { "Sprite lata", 1, {
        {"Sprite lata", 1}
    }},
    { "Schweppes lata", 1, {
        {"Schweppes lata", 1}
    }},
    { "Suco de laranja", 1, {
        {"Suco de laranja", 1}
    }},
    { "Suco de uva", 1, {
        {"Suco de uva", 1}
    }},
    { "Cerveja", 1, {
        {"Cerveja", 1}
    }}
};
int numReceitas = sizeof(receitas) / sizeof(receitas[0]);

// ==================== FUNÇÃO PARA ATUALIZAR O ESTOQUE CONFORME RECEITA ====================

typedef struct {
    char nome[MAX_STRING];
    int quantidade;
} ProdutoEstoque;

void atualizarEstoquePorReceita(Receita rec, int numPedidos) {
    // Carrega o estoque em memória
    ProdutoEstoque produtos[100];
    int nProdutos = 0;
    FILE *fEstoque = fopen("estoque.txt", "r");
    if (fEstoque == NULL) {
        printf("Erro ao abrir o arquivo de estoque!\n");
        return;
    }
    char linha[MAX_STRING];
    while(fgets(linha, MAX_STRING, fEstoque) && nProdutos < 100) {
        sscanf(linha, "%[^,],%d", produtos[nProdutos].nome, &produtos[nProdutos].quantidade);
        nProdutos++;
    }
    fclose(fEstoque);

    // Verifica disponibilidade dos ingredientes
    for (int i = 0; i < rec.numIngredientes; i++) {
        int totalNecessario = rec.ingredientes[i].quantidade * numPedidos;
        int encontrado = 0;
        for (int j = 0; j < nProdutos; j++) {
            if (strcasecmp(produtos[j].nome, rec.ingredientes[i].nome) == 0) {
                encontrado = 1;
                if (produtos[j].quantidade < totalNecessario) {
                    printf("Estoque insuficiente para o ingrediente: %s\n", rec.ingredientes[i].nome);
                    return;
                }
                break;
            }
        }
        if (!encontrado) {
            printf("Ingrediente %s nao encontrado no estoque!\n", rec.ingredientes[i].nome);
            return;
        }
    }

    // Atualiza os estoques
    for (int i = 0; i < rec.numIngredientes; i++) {
        int totalNecessario = rec.ingredientes[i].quantidade * numPedidos;
        for (int j = 0; j < nProdutos; j++) {
            if (strcasecmp(produtos[j].nome, rec.ingredientes[i].nome) == 0) {
                produtos[j].quantidade -= totalNecessario;
                break;
            }
        }
    }

    // Regrava o estoque atualizado
    FILE *fEstoqueOut = fopen("estoque.txt", "w");
    if (fEstoqueOut == NULL) {
        printf("Erro ao atualizar o estoque!\n");
        return;
    }
    for (int j = 0; j < nProdutos; j++) {
        fprintf(fEstoqueOut, "%s,%d\n", produtos[j].nome, produtos[j].quantidade);
    }
    fclose(fEstoqueOut);
    printf("Estoque atualizado para o pedido de %d \"%s\".\n", numPedidos, rec.nome);
}

// ==================== FUNÇÃO PARA PROCESSAR PEDIDO ====================

void processarPedido() {
    char clientePedido[MAX_STRING];
    printf("Informe o nome do cliente para o pedido: ");
    scanf(" %[^\n]", clientePedido);

    int continuar = 1;
    double totalVenda = 0.0;
    char detalhesPedido[2000];
    detalhesPedido[0] = '\0';  // Inicializa o buffer de detalhes do pedido

    while (continuar) {
        int tipo;
        printf("\nSelecione o tipo de item do pedido:\n");
        printf("1 - Smash Burguer\n");
        printf("2 - Porcao\n");
        printf("3 - Bebida\n");
        printf("0 - Finalizar Pedido\n");
        printf("Opcao: ");
        scanf("%d", &tipo);

        if (tipo == 0) {
            break;
        }
        else if (tipo == 1) {
            int escolha, quantidade;
            printf("\n--- Smash Burguer ---\n");
            for (int i = 0; i < 5; i++) {
                printf("%d - %s (R$ %.2lf)\n", i + 1, receitas[i].nome, precos[i]);
            }
            printf("Escolha o Smash Burguer (número): ");
            scanf("%d", &escolha);
            if (escolha < 1 || escolha > numReceitas) {
                printf("Opcao invalida!\n");
                continue;
            }
            printf("Quantidade: ");
            scanf("%d", &quantidade);

            atualizarEstoquePorReceita(receitas[escolha - 1], quantidade);
            double venda = quantidade * precos[escolha - 1];
            totalVenda += venda;
            char itemDesc[MAX_STRING];
            sprintf(itemDesc, "%d x %s (R$ %.2lf)\n", quantidade, receitas[escolha - 1].nome, venda);
            strcat(detalhesPedido, itemDesc);
            printf("Adicionado: %s", itemDesc);
        }
        else if (tipo == 2) {
            Receita porcoes[3];
            // Porcao 1: Onion Rings
            strcpy(porcoes[0].nome, "Porcao de Onion Rings");
            porcoes[0].numIngredientes = 1;
            strcpy(porcoes[0].ingredientes[0].nome, "Onion rings");
            porcoes[0].ingredientes[0].quantidade = 150;
            // Porcao 2: Batata Frita
            strcpy(porcoes[1].nome, "Porcao de Batata Frita");
            porcoes[1].numIngredientes = 1;
            strcpy(porcoes[1].ingredientes[0].nome, "Batata frita");
            porcoes[1].ingredientes[0].quantidade = 200;
            // Porcao 3: Batata Suprema
            strcpy(porcoes[2].nome, "Porcao de Batata Suprema");
            porcoes[2].numIngredientes = 3;
            strcpy(porcoes[2].ingredientes[0].nome, "Batata frita");
            porcoes[2].ingredientes[0].quantidade = 150;
            strcpy(porcoes[2].ingredientes[1].nome, "Queijo mussarela");
            porcoes[2].ingredientes[1].quantidade = 3;
            strcpy(porcoes[2].ingredientes[2].nome, "Bacon");
            porcoes[2].ingredientes[2].quantidade = 2;

            double precosPorcoes[3] = {15.00, 20.00, 22.00};
            int porcaoEscolha, porcaoQuantidade;
            printf("\n--- Porcoes ---\n");
            for (int i = 0; i < 3; i++) {
                printf("%d - %s (R$ %.2lf)\n", i + 1, porcoes[i].nome, precosPorcoes[i]);
            }
            printf("Escolha a porcao (número): ");
            scanf("%d", &porcaoEscolha);
            if (porcaoEscolha < 1 || porcaoEscolha > 3) {
                printf("Opcao invalida!\n");
                continue;
            }
            printf("Quantidade: ");
            scanf("%d", &porcaoQuantidade);

            atualizarEstoquePorReceita(porcoes[porcaoEscolha - 1], porcaoQuantidade);
            double venda = porcaoQuantidade * precosPorcoes[porcaoEscolha - 1];
            totalVenda += venda;
            char itemDesc[MAX_STRING];
            sprintf(itemDesc, "%d x %s (R$ %.2lf)\n", porcaoQuantidade, porcoes[porcaoEscolha - 1].nome, venda);
            strcat(detalhesPedido, itemDesc);
            printf("Adicionado: %s", itemDesc);
        }
        else if (tipo == 3) {
            Receita bebidas[6];
            strcpy(bebidas[0].nome, "Coca-Cola 600 ml");
            bebidas[0].numIngredientes = 1;
            strcpy(bebidas[0].ingredientes[0].nome, "Coca-Cola 600 ml");
            bebidas[0].ingredientes[0].quantidade = 1;

            strcpy(bebidas[1].nome, "Sprite lata");
            bebidas[1].numIngredientes = 1;
            strcpy(bebidas[1].ingredientes[0].nome, "Sprite lata");
            bebidas[1].ingredientes[0].quantidade = 1;

            strcpy(bebidas[2].nome, "Schweppes lata");
            bebidas[2].numIngredientes = 1;
            strcpy(bebidas[2].ingredientes[0].nome, "Schweppes lata");
            bebidas[2].ingredientes[0].quantidade = 1;

            strcpy(bebidas[3].nome, "Suco de laranja");
            bebidas[3].numIngredientes = 1;
            strcpy(bebidas[3].ingredientes[0].nome, "Suco de laranja");
            bebidas[3].ingredientes[0].quantidade = 1;

            strcpy(bebidas[4].nome, "Suco de uva");
            bebidas[4].numIngredientes = 1;
            strcpy(bebidas[4].ingredientes[0].nome, "Suco de uva");
            bebidas[4].ingredientes[0].quantidade = 1;

            strcpy(bebidas[5].nome, "Cerveja");
            bebidas[5].numIngredientes = 1;
            strcpy(bebidas[5].ingredientes[0].nome, "Cerveja");
            bebidas[5].ingredientes[0].quantidade = 1;

            double precosBebidas[6] = {7.00, 6.00, 8.00, 5.00, 6.00, 10.00};
            int bebidaEscolha, bebidaQuantidade;
            printf("\n--- Bebidas ---\n");
            for (int i = 0; i < 6; i++) {
                printf("%d - %s (R$ %.2lf)\n", i + 1, bebidas[i].nome, precosBebidas[i]);
            }
            printf("Escolha a bebida (número): ");
            scanf("%d", &bebidaEscolha);
            if (bebidaEscolha < 1 || bebidaEscolha > 6) {
                printf("Opcao invalida!\n");
                continue;
            }
            printf("Quantidade: ");
            scanf("%d", &bebidaQuantidade);

            atualizarEstoquePorReceita(bebidas[bebidaEscolha - 1], bebidaQuantidade);
            double venda = bebidaQuantidade * precosBebidas[bebidaEscolha - 1];
            totalVenda += venda;
            char itemDesc[MAX_STRING];
            sprintf(itemDesc, "%d x %s (R$ %.2lf)\n", bebidaQuantidade, bebidas[bebidaEscolha - 1].nome, venda);
            strcat(detalhesPedido, itemDesc);
            printf("Adicionado: %s", itemDesc);
        }
        else {
            printf("Opcao invalida!\n");
        }
        printf("Deseja adicionar mais itens? (1-Sim, 0-Nao): ");
        scanf("%d", &continuar);
    }

    // Registra o pedido completo: nome do cliente, itens e total, no arquivo "pedidos.txt"
    FILE *fPedido = fopen("pedidos.txt", "a");
    if (fPedido == NULL) {
        printf("Erro ao abrir o arquivo de pedidos!\n");
        return;
    }
    fprintf(fPedido, "Cliente: %s\n%sTotal: R$ %.2lf\n--------------------------\n", clientePedido, detalhesPedido, totalVenda);
    fclose(fPedido);

    registrarVenda(totalVenda);

    // Mensagem final exibida para o cliente (com os itens do pedido e o valor total)
    printf("\nPedido finalizado para %s!\n", clientePedido);
    printf("Itens do pedido:\n%s", detalhesPedido);
    printf("Valor total do pedido: R$ %.2lf\n", totalVenda);
}

// --- Nova função para listar os pedidos completos por cliente ---
void listarPedidosPorCliente() {
    FILE *fPedido = fopen("pedidos.txt", "r");
    if (fPedido == NULL) {
        printf("Nenhum pedido registrado.\n");
        return;
    }
    char linha[MAX_STRING];
    printf("\n=== Pedidos Realizados ===\n");
    while(fgets(linha, MAX_STRING, fPedido)) {
        printf("%s", linha);
    }
    printf("\n=========================\n");
    fclose(fPedido);
}

void darBaixaPedidos() {
    // Lê todos os pedidos do arquivo "pedidos.txt"
    FILE *fp = fopen("pedidos.txt", "r");
    if (fp == NULL) {
        printf("Nenhum pedido registrado.\n");
        return;
    }

    // Supomos que haja no máximo 100 pedidos e cada pedido tem até 2000 caracteres.
    char orders[100][2000];
    int orderCount = 0;
    char line[500];
    orders[orderCount][0] = '\0';  // inicializa o primeiro pedido

    while(fgets(line, sizeof(line), fp)) {
        // Considera que cada pedido termina com a linha delimitadora "--------------------------\n"
        if (strncmp(line, "--------------------------", 26) == 0) {
            strcat(orders[orderCount], line);
            orderCount++;
            if(orderCount < 100) {
                orders[orderCount][0] = '\0';
            }
        } else {
            strcat(orders[orderCount], line);
        }
    }
    fclose(fp);

    if(orderCount == 0 && strlen(orders[0]) > 0)
        orderCount = 1;

    if(orderCount == 0) {
        printf("Nenhum pedido para dar baixa.\n");
        return;
    }

    // Exibe os pedidos registrados
    printf("\n=== Pedidos Registrados ===\n");
    for (int i = 0; i < orderCount; i++) {
        printf("Pedido %d:\n%s\n", i+1, orders[i]);
    }

    int escolha;
    printf("\nDigite o número do pedido a dar baixa (0 para cancelar): ");
    scanf("%d", &escolha);

    if(escolha <= 0 || escolha > orderCount) {
        printf("Operacao cancelada ou pedido invalido.\n");
        return;
    }

    // Reescreve o arquivo "pedidos.txt" omitindo o pedido selecionado
    FILE *fpOut = fopen("pedidos.txt", "w");
    if(fpOut == NULL) {
        printf("Erro ao abrir o arquivo para atualizar os pedidos.\n");
        return;
    }
    for (int i = 0; i < orderCount; i++) {
        if(i == (escolha - 1)) continue;
        fprintf(fpOut, "%s", orders[i]);
    }
    fclose(fpOut);

    printf("Pedido %d dado baixa com sucesso.\n", escolha);
}

// ==================== MENUS DO SISTEMA ====================

// Menu para Colaborador (opções de clientes foram removidas)
void menuColaborador() {
    int opcao;
    do {
        printf("\n=== SISTEMA DON SMASH BURGUER (Colaborador) ===\n");
        printf("1. Cadastrar Produto no Estoque\n");
        printf("2. Listar Estoque\n");
        printf("3. Realizar Baixa de Estoque\n");
        printf("4. Realizar Pedido\n");
        printf("5. Listar Pedidos\n");
        printf("6. Dar baixa nos pedidos\n");
        printf("7. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        switch(opcao) {
            case 1:
                salvarProdutoEstoque();
                break;
            case 2:
                listarEstoque();
                break;
            case 3:
                realizarBaixadeEstoque();
                break;
            case 4:
                processarPedido();
                break;
            case 5:
                listarPedidosPorCliente();
                break;
            case 6:
                darBaixaPedidos();
                break;
            case 7:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while(opcao != 7);
}

// Menu para Supervisor/Gerente/Dono (removidas as opcoes de clientes)
void menuGerencial() {
    int opcao;
    do {
        printf("\n=== SISTEMA DON SMASH BURGUER (Gerencial) ===\n");
        printf("1. Cadastrar Funcionario\n");
        printf("2. Listar Funcionarios\n");
        printf("3. Cadastrar Produto no Estoque\n");
        printf("4. Listar Estoque\n");
        printf("5. Realizar Baixa de Estoque\n");
        printf("6. Realizar Pedido\n");
        printf("7. Visualizar Fluxo de Caixa\n");
        printf("8. Listar Pedidos\n");
        printf("9. Dar baixa nos pedidos\n");
        printf("10. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        switch(opcao) {
            case 1:
                salvarFuncionario();
                break;
            case 2:
                listarFuncionarios();
                break;
            case 3:
                salvarProdutoEstoque();
                break;
            case 4:
                listarEstoque();
                break;
            case 5:
                realizarBaixadeEstoque();
                break;
            case 6:
                processarPedido();
                break;
            case 7:
                mostrarFluxoCaixa();
                break;
            case 8:
                listarPedidosPorCliente();
                break;
            case 9:
                darBaixaPedidos();
                break;
            case 10:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while(opcao != 10);
}

// ==================== FUNÇÃO PRINCIPAL (MAIN) ====================

int main() {
    int acesso = 0;
    FILE *f = fopen("usuarios.txt", "r");
    if (f == NULL) {
        // Se o arquivo de usuários não existir, cadastra o usuário inicial.
        printf("Nenhum usuario cadastrado. Cadastre um usuario inicial.\n");
        cadastrarUsuario();
    } else {
        fclose(f);
    }

    // Loop de autenticação
    do {
        acesso = autenticar();
        if (acesso == 0) {
            printf("Tente novamente.\n");
        }
    } while (acesso == 0);

    // Chama o menu conforme o nível de acesso
    if (acesso == COLABORADOR)
        menuColaborador();
    else
        menuGerencial();

    return 0;
}
