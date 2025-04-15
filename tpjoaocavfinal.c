//João Pedro de Sousa Cavalcante 
//Matrícula 24.2.4034

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


typedef struct {
    int bomba;
    int aberta;
    int vizinhos;
    int bandeira;
} Celula;

// Estrutura para armazenar o estado do jogo
typedef struct {
    Celula **tabuleiro;
    int linhas;
    int colunas;
    int bombas;
    time_t tempo_inicio;
    char nome_jogador[30];
} JogoAtual;

typedef struct {
    char nome[100];
    int tempo;
    int linhas;
    int colunas;
    int bombas;
} Recorde;

// Protótipos de função
Celula** geraMatriz(int l, int c, int bombas);
void imprimeMatriz(Celula **m, int l, int c);
void imprimeMatrizFinal(Celula **m, int l, int c);
int vitoriaDerrota(Celula **m, int l, int c);
int jogar(JogoAtual *atual, Celula **m, int l, int c);
void salvarJogo(JogoAtual jogo);
JogoAtual carregarJogo();
void atualizarRanking(char* nome, int tempo, int l, int c, int bombas);
void mostrarRanking();
void ajuda();
void menu();

//Função para criar a matriz do campo minado

Celula** geraMatriz(int l, int c, int bombas) 
{
    //Alocação de acordo com o tamanho
    Celula **m = malloc(l * sizeof(Celula*));
    for (int i = 0; i < l; i++) 
    {
        m[i] = malloc(c * sizeof(Celula));

        //Verifica se a alocacao falhou
        if (!m[i]) 
        {
            printf("Erro ao alocar memória para a linha %d\n", i);
            exit(1);
        }
    }

    //Preenche a matriz inteira com 0

    for (int i = 0; i < l; i++) {
        for (int j = 0; j < c; j++) {
            m[i][j].bomba = 0;
            m[i][j].aberta = 0;
            m[i][j].vizinhos = 0;
            m[i][j].bandeira = 0;
        }
    }

    //Gera as bombas de modo aleatório

    srand(time(NULL));
    for (int i = 0; i < bombas; i++) 
    {
        int a = rand() % l;
        int b = rand() % c;
        if (m[a][b].bomba == 0)
            m[a][b].bomba = 1;
        else
            i--;
    }


    //Cálculo do número de bombas vizinhas para cada célula

    for (int i = 0; i < l; i++) 
    {
        for (int j = 0; j < c; j++) 
        {
            int qtbombas = 0;
            if (i - 1 >= 0 && m[i - 1][j].bomba == 1) qtbombas++; //Acima
            if (i + 1 < l && m[i + 1][j].bomba == 1) qtbombas++;  //Abaixo
            if (j + 1 < c && m[i][j + 1].bomba == 1) qtbombas++;  //Direita
            if (j - 1 >= 0 && m[i][j - 1].bomba == 1) qtbombas++;  //Esquerda
            if (i - 1 >= 0 && j - 1 >= 0 && m[i - 1][j - 1].bomba == 1) qtbombas++; //Diagonal de cima, esquerda
            if (i - 1 >= 0 && j + 1 < c && m[i - 1][j + 1].bomba == 1) qtbombas++;  //Diagonal de cima, direita
            if (i + 1 < l && j - 1 >= 0 && m[i + 1][j - 1].bomba == 1) qtbombas++; //Diagonal de baixo, esquerda
            if (i + 1 < l && j + 1 < c && m[i + 1][j + 1].bomba == 1) qtbombas++;   //Diagonal de baixo, direita

            m[i][j].vizinhos = qtbombas;  //Célula posicao [i][j] recebe a quantidade de bombas na vizinhança
        }
    }

    return m; //Retorna a matriz preenchida
}

//Função que imprime a matriz gerada

void imprimeMatriz(Celula **m, int l, int c) 
{
    printf("\n\n    ");  // Ajusta o espaçamento do cabeçalho

    // Imprime os números das colunas
    for (int j = 0; j < c; j++)
        printf("%3d ", j);
    printf("\n   ");

    // Linha superior da matriz
    for (int j = 0; j < c; j++)
        printf("+---");
    printf("+\n");

    for (int i = 0; i < l; i++) {
        printf("%2d |", i); // Número da linha
        for (int j = 0; j < c; j++) 
        {
            if (m[i][j].bandeira == 1)
                printf(" F |");
            else if (m[i][j].aberta == 1) 
            {
                if (m[i][j].bomba == 1)
                    printf(" # |"); // Bomba revelada
                else
                    printf(" %d |", m[i][j].vizinhos); // Número de bombas vizinhas
            }
            else
                printf("   |"); // Espaço vazio para células fechadas
        }

        printf("\n   ");

        // Linha divisória entre as células
        for (int j = 0; j < c; j++)
            printf("+---");
        printf("+\n");
    }
}
//Imprime a matriz toda revelada ao final do jogo

void imprimeMatrizFinal(Celula **m, int l, int c) {
    printf("\n\n\t   ");

    for (int j = 0; j < c; j++)
        printf("%4d ", j);
    printf("\n\t  ");

    for (int j = 0; j < c; j++)
        printf("+----");
    printf("+\n");


    //Para cada coluna j na linha i, imprime a celula:

    for (int i = 0; i < l; i++) {
        printf("\t%2d |", i);
        for (int j = 0; j < c; j++) {
            if (m[i][j].bandeira == 1) { //Se tiver bambeira
                if (m[i][j].bomba == 1) //E for uma bomba
                    printf(" F  |");  // Bandeira correta
                else
                    printf(" X  |");  // Bandeira errada (mostra como X)
            }
            else if (m[i][j].bomba == 1) {
                printf(" #  |");  // Bomba não marcada
            }
            else {
                printf(" %2d |", m[i][j].vizinhos);  // Mostra o número de bombas vizinhas
            }
        }

        printf("\n\t  ");

        for (int j = 0; j < c; j++)
            printf("+----");
        printf("+\n");
    }
}



//Verifica o estado do jogo e se o jogador venceu ou não

int vitoriaDerrota(Celula **m, int l, int c) 
{
    //Inicializa a variável como 0;
    int resultado = 0;

    //Varre a matriz
    for (int i = 0; i < l; i++) 
    {
        for (int j = 0; j < c; j++) 
        {
            //Se houver alguma célula nao aberta (0) e sem bomba(0), ele incrementa o contador
            if (m[i][j].aberta == 0 && m[i][j].bomba == 0)
                resultado++;
        }
    }

    //Lógica de vitória: resultado = 0;

    return resultado;
}



int jogar(JogoAtual *atual, Celula **m, int l, int c) 
{
    char input[50]; //String para armazenar o comando do jogador
    char acao; //caractere para definir a ação do jogador
    int linha = -1, coluna = -1; 
    int verifica = 0; //variável para controle das coordenadas

    do {
        verifica = 0; //Reinicia a variável de verificação das coordenadas
        imprimeMatriz(m, l, c); //Imprime a matriz para o jogador acompanhar o progresso de jogo

        printf("\nDigite a ação (<lin col> para abrir, m <lin col> para marcar, d <lin col> para desmarcar uma bandeira, desistir para revelar o tabuleiro ou voltar para o menu): \n");

        fgets(input, sizeof(input), stdin); //Entrada de dados
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "desistir") == 0) 
        {
            printf("\nVocê desistiu! Mostrando o tabuleiro completo...\n");
            printf("\n\n");
            imprimeMatrizFinal(m, l, c);
            printf("\n\n");
            printf("\n\nVocê não pode mais continuar jogando.\n\n");
            return 0;
        }

        else if(strcmp(input, "DESISTIR") == 0) 
        {
            printf("\nVocê desistiu! Mostrando o tabuleiro completo...\n");
            printf("\n\n");
            imprimeMatrizFinal(m, l, c);
            printf("\n\n");
            printf("\n\nVocê não pode mais continuar jogando.\n\n");
            return 0;
        }

        else if(strcmp(input, "Desistir") == 0) 
        {
            printf("\nVocê desistiu! Mostrando o tabuleiro completo...\n");
            printf("\n\n");
            imprimeMatrizFinal(m, l, c);
            printf("\n\n");
            printf("\n\nVocê não pode mais continuar jogando.\n\n");
            return 0;
        }

        else if (strcmp(input, "voltar") == 0) 
        {
            printf("\nVoltando ao menu...\n");
            return 1;
        }

        else if (strcmp(input, "Voltar") == 0) 
        {
            printf("\nVoltando ao menu...\n");
            return 1;
        }

        else if (strcmp(input, "VOLTAR") == 0) 
        {
            printf("\nVoltando ao menu...\n");
            return 1;
        }

        else if (sscanf(input, "%d %d", &linha, &coluna) == 2) 
        {
            acao = 'a';
            if (linha >= 0 && linha < l && coluna >= 0 && coluna < c) {
                verifica = 1; //Coordenadas válidas
            } 
            else 
            {
                printf("\nErro: Coordenadas fora dos limites! (Limites: 0-%d para linhas, 0-%d para colunas)\n", l-1, c-1);
            }
        }
        else if (sscanf(input, " %c %d %d", &acao, &linha, &coluna) == 3) 
        {
            if ((acao == 'm' || acao == 'd' || acao == 'M' || acao =='D') && linha >= 0 && linha < l && coluna >= 0 && coluna < c) 
            {
                verifica = 1;
            } 
            else 
            {
                printf("\nErro: Ação deve ser 'm' ou 'd' e coordenadas dentro dos limites (0-%d, 0-%d)\n", l-1, c-1);
            }
        }
        else 
        {
            printf("\nErro: Entrada inválida!\n");
        }

        //Se o jogador nao desistiu, nao voltou e as coordenadas forem válidas

        if (verifica) 
        {
            if (acao == 'm' || acao == 'd' || acao == 'M' || acao == 'D') 
            {
                m[linha][coluna].bandeira = !m[linha][coluna].bandeira; //altera entre marcado e desmarcado
            } 
            else if (acao == 'a') //Se não marcou, nem desmarcou bandeira
            {
                if (m[linha][coluna].bandeira == 1) //mas a célula selecionada tiver bandeira
                {
                    printf("Célula marcada com bandeira! Remova a bandeira antes de abrir\n"); //Alerta
                } 
                else 
                {
                    m[linha][coluna].aberta = 1; //Abre a posição

                    if (m[linha][coluna].vizinhos == 0) //Se não houver bombas na vizinhança, abre o quadrado 3xz
                    {
                        for (int i = linha - 1; i <= linha + 1; i++) 
                        {
                            for (int j = coluna - 1; j <= coluna + 1; j++) 
                            {
                                if (i >= 0 && i < l && j >= 0 && j < c && !m[i][j].aberta && !m[i][j].bandeira) 
                                {
                                    m[i][j].aberta = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    } while (vitoriaDerrota(m, l, c) != 0 && !(linha >= 0 && linha < l && coluna >= 0 && coluna < c && m[linha][coluna].bomba == 1));

    if (linha >= 0 && linha < l && coluna >= 0 && coluna < c && m[linha][coluna].bomba == 1) 
    {
        imprimeMatrizFinal(m, l, c);  // Mostra o tabuleiro completo
        printf("\n");
        printf("\n");
        printf("\t\tBOMBA! VOCÊ PERDEU!\n\n");

  
    }
    else if (vitoriaDerrota(m, l, c) == 0) 
    {
        imprimeMatrizFinal(m, l, c);  // Mostra o tabuleiro completo
        printf("\n");
        printf("\tPARABÉNS! VOCÊ GANHOU!\n\n");
        time_t fim = time(NULL);
        int tempo = (int)difftime(fim, atual->tempo_inicio); //Calcula o tempo pela diferença
        
        if (l > 0 && c > 0 && atual->bombas < (l * c)) {
            atualizarRanking(atual->nome_jogador, tempo, l, c, atual->bombas); //atualiza o ranking
        }
    }
    
    return 0;
}

void salvarJogo(JogoAtual jogo) {
    char nome_arquivo[100];
    printf("Digite o nome do arquivo para salvar: ");
    scanf("%99s", nome_arquivo);

    FILE *arquivo = fopen(nome_arquivo, "wb");
    if (!arquivo) {
        printf("Erro ao criar arquivo!\n");
        return;
    }

    // 1. Determina o tipo de jogo (F, M, D, P)
    char tipo;
    if (jogo.linhas == 9 && jogo.colunas == 9 && jogo.bombas == 10) tipo = 'F'; // Fácil
    else if (jogo.linhas == 15 && jogo.colunas == 15 && jogo.bombas == 25) tipo = 'M'; // Médio
    else if (jogo.linhas == 20 && jogo.colunas == 20 && jogo.bombas == 45) tipo = 'D'; // Difícil
    else tipo = 'P'; // Personalizado

    fwrite(&tipo, sizeof(char), 1, arquivo);

    // 2. Salva dimensões e bombas (int)
    fwrite(&jogo.linhas, sizeof(int), 1, arquivo);
    fwrite(&jogo.colunas, sizeof(int), 1, arquivo);
    fwrite(&jogo.bombas, sizeof(int), 1, arquivo);

    // 3. Nome do jogador (30 bytes fixos)
    char nome_formatado[30] = {0};
    strncpy(nome_formatado, jogo.nome_jogador, 29);
    fwrite(nome_formatado, sizeof(char), 30, arquivo);

    // 4. Tempo decorrido (double)
    double tempo_decorrido = difftime(time(NULL), jogo.tempo_inicio);
    fwrite(&tempo_decorrido, sizeof(double), 1, arquivo);

    // 5. Matriz de valores (int[linhas][colunas])
    for (int i = 0; i < jogo.linhas; i++) {
        for (int j = 0; j < jogo.colunas; j++) {
            int valor = jogo.tabuleiro[i][j].bomba ? 9 : jogo.tabuleiro[i][j].vizinhos;
            fwrite(&valor, sizeof(int), 1, arquivo);
        }
    }

    // 6. Matriz de status (char[linhas][colunas])
    for (int i = 0; i < jogo.linhas; i++) {
        for (int j = 0; j < jogo.colunas; j++) {
            char status = jogo.tabuleiro[i][j].aberta ? 'A' : 
                         (jogo.tabuleiro[i][j].bandeira ? 'B' : 'F');
            fwrite(&status, sizeof(char), 1, arquivo);
        }
    }

    fclose(arquivo);
    printf("Jogo salvo com sucesso em %s\n", nome_arquivo);
}

JogoAtual carregarJogo() {
    JogoAtual jogo = {0};
    
    char nome_arquivo[100];
    printf("Digite o nome do arquivo para carregar: ");
    scanf("%99s", nome_arquivo);
    while (getchar() != '\n');

    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (!arquivo) {
        printf("Erro ao abrir arquivo!\n");
        return jogo;
    }

    // 1. Lê o tipo de jogo (F, M, D, P)
    char tipo;
    fread(&tipo, sizeof(char), 1, arquivo);

    // 2. Lê dimensões e bombas (int)
    fread(&jogo.linhas, sizeof(int), 1, arquivo);
    fread(&jogo.colunas, sizeof(int), 1, arquivo);
    fread(&jogo.bombas, sizeof(int), 1, arquivo);

    // 3. Lê nome do jogador (30 bytes)
    char nome_formatado[30];
    fread(nome_formatado, sizeof(char), 30, arquivo);
    strncpy(jogo.nome_jogador, nome_formatado, 30);

    // 4. Lê tempo decorrido e ajusta tempo_inicio
    double tempo_decorrido;
    fread(&tempo_decorrido, sizeof(double), 1, arquivo);
    jogo.tempo_inicio = time(NULL) - (time_t)tempo_decorrido;

    // 5. Aloca e lê a matriz de valores (int[linhas][colunas])
    jogo.tabuleiro = malloc(jogo.linhas * sizeof(Celula *));
    for (int i = 0; i < jogo.linhas; i++) {
        jogo.tabuleiro[i] = malloc(jogo.colunas * sizeof(Celula));
        for (int j = 0; j < jogo.colunas; j++) {
            int valor;
            fread(&valor, sizeof(int), 1, arquivo);
            jogo.tabuleiro[i][j].bomba = (valor == 9);
            jogo.tabuleiro[i][j].vizinhos = (valor == 9) ? 0 : valor;
        }
    }

    // 6. Lê a matriz de status (char[linhas][colunas])
    for (int i = 0; i < jogo.linhas; i++) {
        for (int j = 0; j < jogo.colunas; j++) {
            char status;
            fread(&status, sizeof(char), 1, arquivo);
            jogo.tabuleiro[i][j].aberta = (status == 'A');
            jogo.tabuleiro[i][j].bandeira = (status == 'B');
        }
    }

    fclose(arquivo);
    printf("Jogo carregado com sucesso! Tipo: %c\n", tipo);
    return jogo;
}

void atualizarRanking(char* nome, int tempo, int l, int c, int bombas) {
    Recorde recordes[100];
    int total_recordes = 0;
    
    FILE *arquivo = fopen("recordes.txt", "r");
    if (arquivo) {
        char linha[256];
        char nivel_atual[20] = {0};
        
        while (fgets(linha, sizeof(linha), arquivo)) {
            linha[strcspn(linha, "\n")] = '\0';
            
            if (strstr(linha, "Fácil")) {
                strcpy(nivel_atual, "Fácil");
                continue;
            } else if (strstr(linha, "Médio")) {
                strcpy(nivel_atual, "Médio");
                continue;
            } else if (strstr(linha, "Difícil")) {
                strcpy(nivel_atual, "Difícil");
                continue;
            } else if (strstr(linha, "Personalizado")) {
                strcpy(nivel_atual, "Personalizado");
                continue;
            } else if (strlen(linha) == 0) {
                continue;
            }
            
            Recorde r;
            if (strcmp(nivel_atual, "Personalizado") == 0) {
                sscanf(linha, "%s %d %d %d %d", r.nome, &r.tempo, &r.linhas, &r.colunas, &r.bombas);
            } else {
                sscanf(linha, "%s %d", r.nome, &r.tempo);
                if (strcmp(nivel_atual, "Fácil") == 0) {
                    r.linhas = 9; r.colunas = 9; r.bombas = 10;
                } else if (strcmp(nivel_atual, "Médio") == 0) {
                    r.linhas = 15; r.colunas = 15; r.bombas = 25;
                } else if (strcmp(nivel_atual, "Difícil") == 0) {
                    r.linhas = 20; r.colunas = 20; r.bombas = 45;
                }
            }
            
            if (total_recordes < 100) {
                recordes[total_recordes++] = r;
            }
        }
        fclose(arquivo);
    }

    Recorde novo;
    strcpy(novo.nome, nome);
    novo.tempo = tempo;
    novo.linhas = l;
    novo.colunas = c;
    novo.bombas = bombas;
    recordes[total_recordes++] = novo;

    for (int i = 0; i < total_recordes-1; i++) {
        for (int j = i+1; j < total_recordes; j++) {
            if (recordes[i].tempo > recordes[j].tempo) {
                Recorde temp = recordes[i];
                recordes[i] = recordes[j];
                recordes[j] = temp;
            }
        }
    }

    Recorde facil[5] = {0}, medio[5] = {0}, dificil[5] = {0}, personalizado[5] = {0};
    int count_facil = 0, count_medio = 0, count_dificil = 0, count_personalizado = 0;
    
    for (int i = 0; i < total_recordes; i++) {
        if (recordes[i].linhas == 9 && recordes[i].colunas == 9 && recordes[i].bombas == 10) {
            if (count_facil < 5) facil[count_facil++] = recordes[i];
        } 
        else if (recordes[i].linhas == 15 && recordes[i].colunas == 15 && recordes[i].bombas == 25) {
            if (count_medio < 5) medio[count_medio++] = recordes[i];
        } 
        else if (recordes[i].linhas == 20 && recordes[i].colunas == 20 && recordes[i].bombas == 45) {
            if (count_dificil < 5) dificil[count_dificil++] = recordes[i];
        } 
        else {
            if (count_personalizado < 5) personalizado[count_personalizado++] = recordes[i];
        }
    }

    arquivo = fopen("recordes.txt", "w");
    if (!arquivo) {
        printf("Erro ao salvar ranking!\n");
        return;
    }
    
    if (count_facil > 0) {
        fprintf(arquivo, "Fácil (9x9, 10 bombas)\n");
        for (int i = 0; i < count_facil; i++) {
            fprintf(arquivo, "%s %d\n", facil[i].nome, facil[i].tempo);
        }
        fprintf(arquivo, "\n");
    }
    
    if (count_medio > 0) {
        fprintf(arquivo, "Médio (15x15, 25 bombas)\n");
        for (int i = 0; i < count_medio; i++) {
            fprintf(arquivo, "%s %d\n", medio[i].nome, medio[i].tempo);
        }
        fprintf(arquivo, "\n");
    }
    
    if (count_dificil > 0) {
        fprintf(arquivo, "Difícil (20x20, 45 bombas)\n");
        for (int i = 0; i < count_dificil; i++) {
            fprintf(arquivo, "%s %d\n", dificil[i].nome, dificil[i].tempo);
        }
        fprintf(arquivo, "\n");
    }
    
    if (count_personalizado > 0) {
        fprintf(arquivo, "Personalizado\n");
        for (int i = 0; i < count_personalizado; i++) {
            fprintf(arquivo, "%s %d %d %d %d\n", personalizado[i].nome, personalizado[i].tempo,
                    personalizado[i].linhas, personalizado[i].colunas, personalizado[i].bombas);
        }
    }
    
    fclose(arquivo);
}

void mostrarRanking() {
    printf("\n=== RANKING ===\n");
    
    FILE *arquivo = fopen("recordes.txt", "r");
    if (!arquivo) {
        printf("Nenhum recorde encontrado. Jogue e vença para criar o primeiro!\n");
        return;
    }
    
    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        linha[strcspn(linha, "\n")] = '\0';
        
        if (strlen(linha) == 0) continue;
        
        if (strstr(linha, "Fácil") || strstr(linha, "Médio") || 
            strstr(linha, "Difícil") || strstr(linha, "Personalizado")) {
            printf("\n%s\n", linha);
            continue;
        }
        
        printf("  %s\n", linha);
    }
    
    fclose(arquivo);
}

void ajuda() 
{
    printf("\nInstruções:\n");
    printf("- <lin col>: Revelar posição.\n");
    printf("- m<lin col>: Marcar com bandeira.\n");
    printf("- d<lin col>: Desmarcar posição.\n");
    printf("- desistir: Mostrar tudo e finalizar.\n");
    printf("- voltar: Voltar ao menu sem finalizar.\n");
    printf("Objetivo: Revelar todas as posições sem minas.\n");
}

void menu()
{
    JogoAtual atual = {NULL, 0, 0, 0, 0};
    int opcao = 0;    
    while(1)
    {
        printf("\n(0) Sair\n");
        printf("\n(1) Novo jogo padrão\n");
        printf("\n  - (F) Fácil: jogo 9x9 e 10 minas\n");
        printf("\n  - (M) Médio: jogo 15x15 e 25 minas\n");
        printf("\n  - (D) Difícil: jogo 20x20 e 45 minas\n");
        printf("\n(2) Novo jogo personalizado\n");
        printf("\n(3) Voltar para o jogo atual\n");
        printf("\n(4) Carregar um jogo salvo\n");
        printf("\n(5) Salvar o jogo atual\n");
        printf("\n(6) Mostrar o ranking\n");
        printf("\n(7) Ajuda com as instruções de como jogar\n");

        printf("\n\nSelecione uma opção: ");

     
        scanf(" %d", &opcao);
        while (getchar() != '\n');
        char SN[5];
        char dificuldade;
        char nome[100];



        switch (opcao){

        case 0:
            printf("\nDeseja sair? (Sim/Não) \n");
            scanf("%s", SN);
            while (getchar() != '\n');

            if(strcmp(SN, "Não") == 0 || strcmp(SN, "não") == 0 || strcmp(SN, "Nao") == 0 
                    || strcmp(SN, "nao") == 0 || strcmp(SN, "NAO") == 0 || strcmp(SN, "NÃO") == 0)
            {
                printf("\n\n\nVoltando ao menu...\n\n\n");
                continue;
            }
            else if(strcmp(SN, "Sim") == 0 || strcmp(SN, "sim") == 0 
                || strcmp(SN, "SIM") == 0){
                // Libera memória antes de sair
                if (atual.tabuleiro) {
                    for (int i = 0; i < atual.linhas; i++)
                        free(atual.tabuleiro[i]);
                    free(atual.tabuleiro);
                }
                printf("\n\n\nSaindo...\n\n\n");
                return;
            }
            else{
                printf("\nErro, valor inválido. Digite novamente \n");
                continue;
            }
            break;

        case 1:
            printf("\nDigite o seu nome: ");
            fgets(nome, 100, stdin);
            nome[strlen(nome) - 1] = '\0';
            strcpy(atual.nome_jogador, nome);

            do{
                printf("\nSelecione a dificuldade: (F, M ou D): ");
                scanf(" %c", &dificuldade);
                while (getchar() != '\n');


                if (dificuldade != 'F' && dificuldade != 'f' &&
                    dificuldade != 'M' && dificuldade != 'm' &&
                    dificuldade != 'D' && dificuldade != 'd') 
                {
                    printf("Erro: Dificuldade inválida! Digite novamente.\n");
                }

            }while(dificuldade != 'F' && dificuldade != 'f' &&
                   dificuldade != 'M' && dificuldade != 'm' &&
                   dificuldade != 'D' && dificuldade != 'd');

            int l, c, bombas;

            switch(dificuldade)
            {
                case 'F':
                case 'f':
                    l = 9;
                    c = 9;
                    bombas = 10;
                     break;
                case 'M':
                case 'm':
                    l = 15;
                    c = 15;
                    bombas = 25;
                    break;
                case 'D':
                case 'd':
                    l = 20;
                    c = 20;
                    bombas = 45;
                    break;
                default:
                    printf("Erro, valor inválido\n");
            }

            if (atual.tabuleiro) {
                for (int i = 0; i < atual.linhas; i++) 
                    free(atual.tabuleiro[i]);
                free(atual.tabuleiro);
            }

            atual.tabuleiro = geraMatriz(l, c, bombas);
            atual.linhas = l;
            atual.colunas = c;
            atual.bombas = bombas;
            atual.tempo_inicio = time(NULL);

            if (!jogar(&atual, atual.tabuleiro, atual.linhas, atual.colunas)) {
                for (int i = 0; i < atual.linhas; i++) 
                    free(atual.tabuleiro[i]);
                free(atual.tabuleiro);
                atual.tabuleiro = NULL;
            }
            break;

        case 2:
            printf("\nDigite seu nome: ");
            fgets(atual.nome_jogador, 30, stdin);
            atual.nome_jogador[strcspn(atual.nome_jogador, "\n")] = '\0';

            int lin, col, qtbombas;

            do{
                printf("\nDigite as linhas e colunas do tabuleiro:  ");
                scanf(" %d %d", &lin, &col);
                if(lin < 0 || col < 0 || lin == 0 || col == 0)
                    printf("\n\nERRO, digite coordenadas válidas\n");
            }while(lin < 0 || col < 0 || lin == 0 || col == 0);

            do{
                printf("\nDigite a quantidade de bombas: ");
                scanf(" %d", &qtbombas);
                while (getchar() != '\n');

                if(qtbombas == 0 || qtbombas < 0 || qtbombas > (lin*col)-1)
                    printf("\n\nERRO, digite uma quantidade válida\n");
            }while(qtbombas == 0 || qtbombas < 0 || qtbombas > (lin*col)-1);

            if (atual.tabuleiro) {
                for (int i = 0; i < atual.linhas; i++) 
                    free(atual.tabuleiro[i]);
                free(atual.tabuleiro);
            }

            atual.tabuleiro = geraMatriz(lin, col, qtbombas);
            atual.linhas = lin;
            atual.colunas = col;
            atual.bombas = qtbombas;
            atual.tempo_inicio = time(NULL);

            if (!jogar(&atual, atual.tabuleiro, atual.linhas, atual.colunas)) {
                for (int i = 0; i < atual.linhas; i++) 
                    free(atual.tabuleiro[i]);
                free(atual.tabuleiro);
                atual.tabuleiro = NULL;
            }
            break;

        case 3:
            if (atual.tabuleiro) 
            {
                printf("\nRetornando ao jogo atual (%dx%d, %d bombas)...\n",
                        atual.linhas, atual.colunas, atual.bombas);
                if (!jogar(&atual, atual.tabuleiro, atual.linhas, atual.colunas)) {
                    for (int i = 0; i < atual.linhas; i++) 
                        free(atual.tabuleiro[i]);
                    free(atual.tabuleiro);
                    atual.tabuleiro = NULL;
                }
            } 
            else 
            {
                printf("\nNenhum jogo em andamento.\n");
            }
            break;

        case 4:
            {
                JogoAtual carregado = carregarJogo();
                if (carregado.tabuleiro) {
                    if (atual.tabuleiro) {
                        for (int i = 0; i < atual.linhas; i++) 
                            free(atual.tabuleiro[i]);
                        free(atual.tabuleiro);
                    }
                    atual = carregado;
                    
                    if (!jogar(&atual, atual.tabuleiro, atual.linhas, atual.colunas)) {
                        for (int i = 0; i < atual.linhas; i++) 
                            free(atual.tabuleiro[i]);
                        free(atual.tabuleiro);
                        atual.tabuleiro = NULL;
                    }
                }
            }
            break;

        case 5: 
            if (atual.tabuleiro) 
            {
                salvarJogo(atual);
            } 
            else 
            {
                printf("\nNenhum jogo para salvar!\n");
            }
            break;

        case 6: 
            mostrarRanking();
            break;

        case 7: 
            ajuda();
            printf("\n\n\n");
            break;
        default:
            printf("\nDigite uma opção válida (0 a 7)\n");
        }

    }
}

int main()
{
    // Verifica se o arquivo de ranking existe
    FILE *arquivo = fopen("recordes.txt", "r");
    if (!arquivo) {
        // Se não existir, cria um vazio
        arquivo = fopen("recordes.txt", "w");
        if (arquivo) {
            fclose(arquivo);
        }
    } else {
        fclose(arquivo);
    }

    menu();
    return 0;
}