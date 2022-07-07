/* 
PROGRAMAÇÃO DE BAIXO NÍVEL - 128/138
Trabalho 2 
Gabriela Panta Zorzo 
Morgana Weber 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Para usar strings

#ifdef WIN32
#include <windows.h> // Apenas para Windows
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>   // Funções da OpenGL
#include <GL/glu.h>  // Funções da GLU
#include <GL/glut.h> // Funções da FreeGLUT
#endif

// SOIL é a biblioteca para leitura das imagens
#include <SOIL.h>

// Um pixel RGB (24 bits)
typedef struct
{
    unsigned char r, g, b;
} RGB8;

// Uma imagem RGB
typedef struct
{
    int width, height;
    RGB8 *img;
} Img;

// Protótipos
void load(char *name, Img *pic);
void uploadTexture();
void seamcarve(int targetWidth); // executa o algoritmo
void freemem();                  // limpa memória (caso tenha alocado dinamicamente)

// Funções da interface gráfica e OpenGL
void init();
void draw();
void keyboard(unsigned char key, int x, int y);
void arrow_keys(int a_keys, int x, int y);

// Largura e altura da janela
int width, height;

// Largura desejada (selecionável)
int targetW;

// Identificadores de textura
GLuint tex[3];

// As 3 imagens
Img pic[3];
Img *source;
Img *mask;
Img *target;

// Imagem selecionada (0,1,2)
int sel;

// Carrega uma imagem para a struct Img
void load(char *name, Img *pic)
{
    int chan;
    pic->img = (RGB8 *)SOIL_load_image(name, &pic->width, &pic->height, &chan, SOIL_LOAD_RGB);
    if (!pic->img)
    {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
        exit(1);
    }
    printf("Load: %d x %d x %d\n", pic->width, pic->height, chan);
}

// Calcula o menor caminho -> não usamos pois optamos por usar o primeiro menor caminho 
// da energia acumulada 

// int custoCaminho(int linha, int coluna, int custo, int rows, int columns, int energyPlus[rows][columns])
// {
//     int smallest;
//     int cSmallest;

//     if (linha > 0)
//     {
//         smallest = energyPlus[linha - 1][coluna];
//         cSmallest = coluna;

//         if (coluna > 0) {
//            if (energyPlus[linha - 1][coluna - 1] < smallest)
//            {
//               smallest = energyPlus[linha - 1][coluna - 1];
//               cSmallest = coluna - 1;
//            }
//         }   
//         if (coluna < targetW - 1) {
//             if (energyPlus[linha - 1][coluna + 1] < smallest)
//             {
//                 smallest = energyPlus[linha - 1][coluna + 1];
//                 cSmallest = coluna + 1;
//             }
//         }    
//         return custoCaminho(linha - 1, cSmallest, custo + smallest, rows, columns, energyPlus);
//     }
//     return custo;
// }

// Implemente AQUI o seu algoritmo
void seamcarve(int targetWidth)
{
    // Aplica o algoritmo e gera a saida em target->img...
    // RGB8 (*ptr1) = source;
    // RGB8 (*ptr2) = source;

    // Ao entrar a primeira vez no código, copia a imagem (source) para a imagem de saída (target)
    static int firstTime = 0;

    if (firstTime == 0)
    {
        RGB8(*ptrT)
        [target->width] = (RGB8(*)[target->width])target->img;
        RGB8(*ptrS)
        [source->width] = (RGB8(*)[source->width])source->img;

        for (int y = 0; y < source->height; y++)
        {
            for (int x = 0; x < source->width; x++)
            {
                ptrT[y][x].r = ptrS[y][x].r;
                ptrT[y][x].g = ptrS[y][x].g;
                ptrT[y][x].b = ptrS[y][x].b;
            }
        }
        firstTime = 1;
    }

    RGB8(*ptr3) [target->width] = (RGB8(*)[target->width])target->img;

    // Matriz com a energia de cada pixel
    int energy[target->height][targetWidth];
    // Variação da energia em x
    int deltaRx;
    int deltaGx;
    int deltaBx;
    // Variação da energia em y
    int deltaRy;
    int deltaGy;
    int deltaBy;
    // Variação total da energia em x
    int deltaX;
    // Variação total da energia em y
    int deltaY;

    // Cálculo da energia
    for (int linha = 0; linha < target->height; linha++)
    {
        for (int coluna = 0; coluna < targetWidth; coluna++)
        {
            if (coluna == 0)
            {
                deltaRx = abs((int)(ptr3[linha][coluna + 2].r - ptr3[linha][coluna + 1].r));
                deltaGx = abs((int)(ptr3[linha][coluna + 2].g - ptr3[linha][coluna + 1].g));
                deltaBx = abs((int)(ptr3[linha][coluna + 2].b - ptr3[linha][coluna + 1].b));
            }
            else if (coluna == targetWidth - 1)
            {
                deltaRx = abs((int)(ptr3[linha][coluna - 2].r - ptr3[linha][coluna - 1].r));
                deltaGx = abs((int)(ptr3[linha][coluna - 2].g - ptr3[linha][coluna - 1].g));
                deltaBx = abs((int)(ptr3[linha][coluna - 2].b - ptr3[linha][coluna - 1].b));
            }
            else
            {
                deltaRx = abs((int)(ptr3[linha][coluna + 1].r - ptr3[linha][coluna - 1].r));
                deltaGx = abs((int)(ptr3[linha][coluna + 1].g - ptr3[linha][coluna - 1].g));
                deltaBx = abs((int)(ptr3[linha][coluna + 1].b - ptr3[linha][coluna - 1].b));
            }

            if (linha == 0)
            {
                deltaRy = abs((int)(ptr3[linha + 2][coluna].r - ptr3[linha + 1][coluna].r));
                deltaGy = abs((int)(ptr3[linha + 2][coluna].g - ptr3[linha + 1][coluna].g));
                deltaBy = abs((int)(ptr3[linha + 2][coluna].b - ptr3[linha + 1][coluna].b));
            }
            else if (linha == target->height - 1)
            {
                deltaRy = abs((int)(ptr3[linha - 2][coluna].r - ptr3[linha - 1][coluna].r));
                deltaGy = abs((int)(ptr3[linha - 2][coluna].g - ptr3[linha - 1][coluna].g));
                deltaBy = abs((int)(ptr3[linha - 2][coluna].b - ptr3[linha - 1][coluna].b));
            }
            else
            {
                deltaRy = abs((int)(ptr3[linha + 1][coluna].r - ptr3[linha - 1][coluna].r));
                deltaGy = abs((int)(ptr3[linha + 1][coluna].g - ptr3[linha - 1][coluna].g));
                deltaBy = abs((int)(ptr3[linha + 1][coluna].b - ptr3[linha - 1][coluna].b));
            }

            deltaX = deltaRx * deltaRx + deltaGx * deltaGx + deltaBx * deltaBx;
            deltaY = deltaRy * deltaRy + deltaGy * deltaGy + deltaBy * deltaBy;

            energy[linha][coluna] = deltaX + deltaY;
        }
    }
    
    // Utiliza a máscara para aumentar a energia dos pontos verdes
    // e diminuir a energia dos pontos vermelhos
    RGB8(*ptrM)
    [mask->width] = (RGB8(*)[mask->width])mask->img;

    for (int linha = 0; linha < target->height; linha++)
    {
        for (int coluna = 0; coluna < targetWidth; coluna++)
        {
            if(ptrM[linha][coluna].r >= 170){
                if(ptrM[linha][coluna].b <= 30 && ptrM[linha][coluna].g <= 30){
                    energy[linha][coluna] -= 800000;
                }
            }
            else if(ptrM[linha][coluna].g >= 170){
                energy[linha][coluna] += 400000;
            }
        }
    }

    // Matriz com a energia acumulada
    int energyPlus[target->height][targetWidth];

    for (int linha = 0; linha < target->height; linha++)
    {
        for (int coluna = 0; coluna < targetWidth; coluna++)
        {
            if (linha == 0)
            {
                energyPlus[linha][coluna] = energy[linha][coluna];
            }
            else
            {
                int smallest = energyPlus[linha - 1][coluna];

                if (energyPlus[linha - 1][coluna - 1] < smallest && coluna > 0)
                {
                    smallest = energyPlus[linha - 1][coluna - 1];
                }
                if (energyPlus[linha - 1][coluna + 1] < smallest && coluna < targetWidth - 1)
                {
                    smallest = energyPlus[linha - 1][coluna + 1];
                }
                energyPlus[linha][coluna] = energy[linha][coluna] + smallest;
            }
        }
    }

    // Calcula o caminho com a menor energia -> seria utilizado para chamar o método custoCaminho

    // int colunaMenorCusto = 0;
    // int menorCusto = custoCaminho(target->height - 1, 0, energyPlus[target->height - 1][0], target->height, targetWidth, energyPlus);

    // for (int coluna = 1; coluna < targetWidth; coluna++)
    // {
    //     int custo = custoCaminho(target->height - 1, coluna, energyPlus[target->height - 1][coluna], target->height, targetWidth, energyPlus);

    //     if (custo < menorCusto)
    //     {
    //         menorCusto = custo;
    //         colunaMenorCusto = coluna;
    //     }
    // }

    // Pega o menor valor da última linha da matriz de energia acumulada
    int colunaMenorCusto = 0;
    int pathCost = energyPlus[target->height-1][0];
    for (int x=1; x<targetWidth; x++)
    {
        if (pathCost > energyPlus[target->height-1][x]) {
            pathCost = energyPlus[target->height-1][x];
            colunaMenorCusto = x;
        }
    }

    // Encontra o menor caminho para ser removido

    int smallest;
    int smallestPath[target->height];
    int cAux;

    int p = 0;
    for (int linha = target->height - 1; linha > 0; linha--)
    {
        smallestPath[p] = colunaMenorCusto;

        cAux = colunaMenorCusto;

        smallest = energyPlus[linha-1][colunaMenorCusto];

        if (colunaMenorCusto > 0){
            if (energyPlus[linha-1][colunaMenorCusto - 1] < smallest)
            {
                smallest = energyPlus[linha-1][colunaMenorCusto - 1];
                cAux = colunaMenorCusto - 1;
            }
        }
        if(colunaMenorCusto < targetWidth - 1){
            if (energyPlus[linha-1][colunaMenorCusto + 1] < smallest)
            {
                smallest = energyPlus[linha-1][colunaMenorCusto + 1];
                cAux = colunaMenorCusto + 1;
            }
        }
        colunaMenorCusto = cAux;
        p++;
    }
    smallestPath[p] = colunaMenorCusto;

    // Remove caminho com menor energia 

    RGB8(*ptr)
    [target->width] = (RGB8(*)[target->width])target->img;

    RGB8(*ptrMask)
    [mask->width] = (RGB8(*)[mask->width])mask->img;

    int pSP = 0;

    for (int y = target->height - 1; y >= 0; y--)
    {

        for (int c = smallestPath[pSP]; c < targetWidth - 1; c++)
        {
            ptr[y][c].r = ptr[y][c + 1].r;
            ptr[y][c].g = ptr[y][c + 1].g;
            ptr[y][c].b = ptr[y][c + 1].b;

            ptrMask[y][c] = ptrMask[y][c + 1];
        }
        pSP++;

        for (int x = targetWidth; x < target->width; x++)
        {
            ptr[y][x].r = ptr[y][x].g = ptr[y][x].b = 0;
        }
    }

    // Chame uploadTexture a cada vez que mudar
    // a imagem (pic[2])
    uploadTexture();
    glutPostRedisplay();
}

void freemem()
{
    // Libera a memória ocupada pelas 3 imagens
    free(pic[0].img);
    free(pic[1].img);
    free(pic[2].img);
}

/********************************************************************
 * 
 *  VOCÊ NÃO DEVE ALTERAR NADA NO PROGRAMA A PARTIR DESTE PONTO!
 *
 ********************************************************************/
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("seamcarving [origem] [mascara]\n");
        printf("Origem é a imagem original, mascara é a máscara desejada\n");
        exit(1);
    }
    glutInit(&argc, argv);

    // Define do modo de operacao da GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // pic[0] -> imagem original
    // pic[1] -> máscara desejada
    // pic[2] -> resultado do algoritmo

    // Carrega as duas imagens
    load(argv[1], &pic[0]);
    load(argv[2], &pic[1]);

    if (pic[0].width != pic[1].width || pic[0].height != pic[1].height)
    {
        printf("Imagem e máscara com dimensões diferentes!\n");
        exit(1);
    }

    // A largura e altura da janela são calculadas de acordo com a maior
    // dimensão de cada imagem
    width = pic[0].width;
    height = pic[0].height;

    // A largura e altura da imagem de saída são iguais às da imagem original (1)
    pic[2].width = pic[1].width;
    pic[2].height = pic[1].height;

    // Ponteiros para as structs das imagens, para facilitar
    source = &pic[0];
    mask = &pic[1];
    target = &pic[2];

    // Largura desejada inicialmente é a largura da janela
    targetW = target->width;

    // Especifica o tamanho inicial em pixels da janela GLUT
    glutInitWindowSize(width, height);

    // Cria a janela passando como argumento o titulo da mesma
    glutCreateWindow("Seam Carving");

    // Registra a funcao callback de redesenho da janela de visualizacao
    glutDisplayFunc(draw);

    // Registra a funcao callback para tratamento das teclas ASCII
    glutKeyboardFunc(keyboard);

    // Registra a funcao callback para tratamento das setas
    glutSpecialFunc(arrow_keys);

    // Cria texturas em memória a partir dos pixels das imagens
    tex[0] = SOIL_create_OGL_texture((unsigned char *)pic[0].img, pic[0].width, pic[0].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
    tex[1] = SOIL_create_OGL_texture((unsigned char *)pic[1].img, pic[1].width, pic[1].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

    // Exibe as dimensões na tela, para conferência
    printf("Origem  : %s %d x %d\n", argv[1], pic[0].width, pic[0].height);
    printf("Máscara : %s %d x %d\n", argv[2], pic[1].width, pic[0].height);
    sel = 0; // pic1

    // Define a janela de visualizacao 2D
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, width, height, 0.0);
    glMatrixMode(GL_MODELVIEW);

    // Aloca memória para a imagem de saída
    pic[2].img = malloc(pic[1].width * pic[1].height * 3); // W x H x 3 bytes (RGB)
    // Pinta a imagem resultante de preto!
    memset(pic[2].img, 0, width * height * 3);

    // Cria textura para a imagem de saída
    tex[2] = SOIL_create_OGL_texture((unsigned char *)pic[2].img, pic[2].width, pic[2].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

    // Entra no loop de eventos, não retorna
    glutMainLoop();
}

// Gerencia eventos de teclado
void keyboard(unsigned char key, int x, int y)
{
    if (key == 27)
    {
        // ESC: libera memória e finaliza
        freemem();
        exit(1);
    }
    if (key >= '1' && key <= '3')
        // 1-3: seleciona a imagem correspondente (origem, máscara e resultado)
        sel = key - '1';
    if (key == 's')
    {
        seamcarve(targetW);
    }
    glutPostRedisplay();
}

void arrow_keys(int a_keys, int x, int y)
{
    switch (a_keys)
    {
    case GLUT_KEY_RIGHT:
        if (targetW <= pic[2].width - 1)
            targetW += 1;
        seamcarve(targetW);
        break;
    case GLUT_KEY_LEFT:
        if (targetW > 1)
            targetW -= 1;
        seamcarve(targetW);
        break;
    default:
        break;
    }
}
// Faz upload da imagem para a textura,
// de forma a exibi-la na tela
void uploadTexture()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 target->width, target->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, target->img);
    glDisable(GL_TEXTURE_2D);
}

// Callback de redesenho da tela
void draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Preto
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Para outras cores, veja exemplos em /etc/X11/rgb.txt

    glColor3ub(255, 255, 255); // branco

    // Ativa a textura corresponde à imagem desejada
    glBindTexture(GL_TEXTURE_2D, tex[sel]);
    // E desenha um retângulo que ocupa toda a tela
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex2f(0, 0);

    glTexCoord2f(1, 0);
    glVertex2f(pic[sel].width, 0);

    glTexCoord2f(1, 1);
    glVertex2f(pic[sel].width, pic[sel].height);

    glTexCoord2f(0, 1);
    glVertex2f(0, pic[sel].height);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Exibe a imagem
    glutSwapBuffers();
}
