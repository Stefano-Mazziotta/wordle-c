/*
 * links importantes
 *   https://es.stackoverflow.com/questions/376010/c%C3%B3mo-funcionan-realmente-los-arrays-bidimensionales-din%C3%A1micos-en-c
 *   https://www.knowprogram.com/c-programming/2d-array-of-strings-in-c/
 *   https://beginnersbook.com/2014/01/2d-arrays-in-c-example/
 *   https://stackoverflow.com/questions/25531341/strcpy-segmentation-fault-c
 *   https://stackoverflow.com/questions/36890624/malloc-a-2d-array-in-c
 *   https://stackoverflow.com/questions/54244461/dynamically-allocating-memory-for-2d-char-array
 *
 * Desarrollar sistema de puntos
 * Puntaje:
 *
 *  ● El usuario inicia con 5.000 puntos.
 *  ● Si acierta la palabra en el primer intento, gana la jugada con una puntuación de 10.000.
 *  ● A medida que realice intentos sin lograr descubrir la palabra, es decir use otra fila, se le  descuenta 500.
 *  ● Además, en cada intento suman 50 puntos las nuevas letras acertadas (en lugar incorrecto) y 100 las nuevas letras ubicadas correctamente.
 *  ● Finalmente al ganar recibe 2.000 puntos adicionales.
 *
 *  Si no logra descubrir la palabra la puntuación final es 0.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_NUM_OF_WORDS 100
#define MAX_GUESSES 6
#define MAX_LETTERS 5

#define INT_VALUE_FOR_INITIALIZE -1
#define INT_VALUE_FOR_GREEN 100
#define INT_VALUE_FOR_YELLOW 50
#define NO_POINTS 0

// constantes para calcular el score.
#define POINTS_WIN_FIRST_TRY 10000
#define POINTS_WIN 2000
#define POINTS_GAME_OVER 0
#define POINTS_MOVE_NEXT_ROW 500

#define MAX_SIZE_MEMORY_YELLOW_LETTERS 30

int calculatePointsGreenYellow(int **attempts, int numOfGuesses){
    int total = 0;

    for (int row = 0; row < numOfGuesses; row++)
    {
        for (int column = 0; column < MAX_LETTERS; column++)
        {
            int value = attempts[row][column];
            if(value != -1 && row != MAX_GUESSES - 1){
                total = total + value;
            }
        }
        
    }

    return total;
}

int calculateScore(int **attempts, bool guessedCorrectly, int numOfGuesses)
{
    int score = 5000;

    if (numOfGuesses == 1)
    {
        score = POINTS_WIN_FIRST_TRY;
        return score;
    }

    if (!guessedCorrectly && numOfGuesses == 6)
    {
        score = POINTS_GAME_OVER;
        return score;
    }
    
    // -1 porque no se cuenta el ultimo intentos.
    score = score - (POINTS_MOVE_NEXT_ROW * (numOfGuesses - 1));

    // calcular puntos letras verdes y letras amarillas
    int pointsGreenYellow = calculatePointsGreenYellow(attempts, numOfGuesses); 
    score = score + POINTS_WIN + pointsGreenYellow;

    return score;
}

// busca si existe una [G = "100 puntos"] dentro de la pista del intento actual -> linea 126.
//  - si la hay, establece 0 (NO_POINTS) a las siguientes filas en la misma posicion de la letra correcta ingresada.
//  - si no la hay, pasa a la letra siguiente.
void checkLettersGreen(int **attempts, int numOfGuesses)
{
    int row = numOfGuesses - 1;
    for (int i = 0; i < MAX_LETTERS; i++)
    {
        int attemptValue = attempts[row][i];

        if (attemptValue == 100)
        {
            for (int j = MAX_GUESSES - 1; j > row; j--)
            {
                attempts[j][i] = NO_POINTS;
            }
        }
    }
}

bool checkRepeatedLetterYellow(char *inputLetters, char yellowLetter)
{
    
    // inputLetters -> array unidimensional de letras amarillas (correctas en lugar quivocado) ingresadas anteriormente.
    // letter -> letra amarilla de la palabra ingresada.
    // verificar que la letra ingresada esta en inputLetters.

    //  si esta, retorno true
    //  si no esta, la agrego al array y retorno false;
    bool existLetterInArray = strpbrk(inputLetters, &yellowLetter) != 0; 
    if(existLetterInArray){
        return true;
    }
    
    int len = strlen(inputLetters);
    inputLetters[len] = yellowLetter;
    inputLetters[len+1] = '\0';

    return false;
}

bool processGuess(const char *answer, const char *guess, int **attempts, int numOfGuesses, char *inputYellowLetters)
{
    // the clue
    char clue[MAX_LETTERS + 1] = {'-', '-', '-', '-', '-', '\0'};
    int row = numOfGuesses - 1;

    // first pass, look for exact matches
    for (int i = 0; i < 5; i++)
    {
        if (guess[i] == answer[i])
        {

            clue[i] = 'G';
            if(attempts[row][i] == INT_VALUE_FOR_INITIALIZE){
                attempts[row][i] = INT_VALUE_FOR_GREEN;
            }
        }
    }

    // second pass, look there but elsewhere
    for (int i = 0; i < 5; i++)
    {
        if (clue[i] == '-')
        {
            for (int j = 0; j < 5; j++)
            {
                bool existLetterInAnswer = guess[i] == answer[j];
                bool isDiferentFromGreen = (attempts[row][j] != INT_VALUE_FOR_GREEN || attempts[row][j] != NO_POINTS );

                if ( existLetterInAnswer && isDiferentFromGreen)
                {
                    clue[i] = 'Y';
                    char yellowLetter;
                    yellowLetter = guess[i]; 

                    // si existe pongo 0 puntos, si no existe pongo 50 puntos.
                    bool existLetterYellow = checkRepeatedLetterYellow(inputYellowLetters, yellowLetter);
                    if(existLetterYellow){
                        attempts[row][i] = NO_POINTS;
                        break;    
                    }

                    attempts[row][i] = INT_VALUE_FOR_YELLOW;
                    break;
                }
            }
        }
    }

    checkLettersGreen(attempts, numOfGuesses);

    printf("%s\n", clue);
    return strcmp(clue, "GGGGG") == 0;
}

void endGame(bool guessedCorrectly, int numOfGuesses, char *answer, int score)
{
    // display end of game message
    if (guessedCorrectly)
    {

        bool wonFirstTime = numOfGuesses == 1;
        char *txtIntento = wonFirstTime ? "intento" : "intentos";

        printf("Felicitaciones! Adivinaste la palabra en %d %s!\n", numOfGuesses, txtIntento);
    }

    if (!guessedCorrectly)
    {
        printf("Perdiste! Usaste las 6 vidas sin adivinar... la palabra correcta es '%s'\n", answer);
    }

    printf("Tú puntuación final es de %d puntos.\n", score);
}

int loadWords(char **wordsList, char *fiveLetterWord, int wordCount)
{

    FILE *wordsFile = fopen("words.txt", "r");

    while (fscanf(wordsFile, "%s", fiveLetterWord) != EOF && wordCount < MAX_NUM_OF_WORDS)
    {
        wordsList[wordCount] = fiveLetterWord;
        wordCount++;

        fiveLetterWord = malloc(6 * sizeof(char));
    }
    fclose(wordsFile);
    return wordCount;
}

int main()
{

    char **wordsList = calloc(MAX_NUM_OF_WORDS, sizeof(char *));
    char *fiveLetterWord = malloc(6 * sizeof(char));
    char *inputYellowLetters = (char*)malloc ( MAX_SIZE_MEMORY_YELLOW_LETTERS * sizeof (char));

    int wordCount = 0;
    wordCount = loadWords(wordsList, fiveLetterWord, wordCount);

    // pick a word randomly
    srand(time(NULL));
    char *answer = wordsList[rand() % wordCount];

    // start the game
    // inicializar intentos en array bidimensional
    int **attempts = malloc(MAX_GUESSES * sizeof(int *));
    for (int i = 0; i != MAX_GUESSES; ++i)
    {
        attempts[i] = malloc(MAX_LETTERS * sizeof(int));
        for (int j = 0; j != MAX_LETTERS; ++j)
        {
            attempts[i][j] = INT_VALUE_FOR_INITIALIZE;
        }
    }

    // do the game loop
    int numOfGuesses = 0;
    bool guessedCorrectly = false;
    char *guess = malloc(6 * sizeof(char));

    while (numOfGuesses < MAX_GUESSES && !guessedCorrectly)
    {

        printf("Intento nro. %d \n", numOfGuesses + 1);
        printf("Ingrese un palabra de 5 letras y presione enter: ");
        scanf("%s", guess);

        printf("Ingresaste '%s' \n", guess);

        const int lenGuess = strlen(guess);
        if (lenGuess != MAX_LETTERS)
        {
            printf("La palabra debe contener 5 letras.\n");
            continue;
        }

        numOfGuesses += 1;
        guessedCorrectly = processGuess(answer, guess, attempts, numOfGuesses, inputYellowLetters);
        
    }
    
    int score = calculateScore(attempts, guessedCorrectly, numOfGuesses);
    endGame(guessedCorrectly, numOfGuesses, answer, score);

    free(wordsList);
    free(fiveLetterWord);
    free(guess);
    free(attempts);
    free(inputYellowLetters);

    return 0;
}
