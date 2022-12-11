#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_NUM_OF_WORDS 100

// constantes para calcular el score.
#define POINTS_WIN_FIRST_TRY 10000
#define POINTS_WIN 2000
#define POINTS_GAME_OVER 0
#define POINTS_MOVE_NEXT_ROW 500
#define POINTS_FOR_G_LETTER 100
#define POINTS_FOR_Y_LETTER 50

/* Desarrollar sistema de puntos
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

int getMatchesQuantity(char* str1, char letter[1])
{
    int matchesQuantity = 0;

    for (int i = 0; i < strlen(str1); i++) {
        bool isMatch = str1[i] == letter[0];
        if(isMatch){
            matchesQuantity++;
        }
    }
    
    return matchesQuantity;
}

int calculateScore(int score, char* clue, bool guessedCorrectly, int numOfGuesses )
{
    int newScore = score;

    if(guessedCorrectly && numOfGuesses == 1)
    {
        newScore = POINTS_WIN_FIRST_TRY;
        return newScore;
    }

    if(!guessedCorrectly && numOfGuesses == 6)
    {
        newScore = POINTS_GAME_OVER;
        return newScore;
    }

    if(!guessedCorrectly && numOfGuesses < 6)
    {
        newScore = newScore - POINTS_MOVE_NEXT_ROW;
        
        int quantityOfG = getMatchesQuantity(clue,"G");
        int quantityOfY = getMatchesQuantity(clue,"Y");

        newScore = newScore + (POINTS_FOR_G_LETTER * quantityOfG);
        newScore = newScore + (POINTS_FOR_Y_LETTER * quantityOfY);
    }

    if(guessedCorrectly)
    {
        newScore = newScore + POINTS_WIN;
    }

    return newScore;
}

char* processGuess(const char* answer, const char* guess)
{
    // the clue
    char clue[6] = {'-', '-', '-', '-', '-', '\0'};

    // a set of flags indicating if that letter in the answer in used as clue
    bool answerFlags[5] = {false, false, false, false, false};

    // first pass, look for exact matches
    for (int i = 0; i < 5; i++)
    {
        if (guess[i] == answer[i])
        {
            clue[i] = 'G';
            answerFlags[i] = true;
        }
    }

    // second pass, look there but elsewhere
    for (int i = 0; i < 5; i++)
    {
        if (clue[i] == '-')
        {
            for (int j = 0; j < 5; j++)
            {
                if (guess[i] == answer[j] && !answerFlags[j])
                {
                    clue[i] = 'Y';
                    answerFlags[j] = true;
                    break;
                }
            }
        }
    }

    printf("%s\n", clue);
    char* clueGlobal = malloc(6*sizeof(char));
    strcpy(clueGlobal, clue);
    return clueGlobal;
}

void endGame(bool guessedCorrectly, int numOfGuesses, int score, char* answer)
{
    // display end of game message
    if (guessedCorrectly)
    {

        bool wonFirstTime = numOfGuesses == 1;
        char* txtIntento =  wonFirstTime ? "intento": "intentos";

        printf("Felicitaciones! Adivinaste la palabra en %d %s!\n", numOfGuesses, txtIntento);
    }

    if(!guessedCorrectly)
    {
        printf("Perdiste! Usaste las 6 vidas sin adivinar... la palabra correcta es '%s'\n", answer);
    }

    printf("Tú puntuación final es de %d puntos.\n", score);

}

int main()
{

    // load the words
    char** wordsList = calloc(MAX_NUM_OF_WORDS, sizeof(char*));

    int wordCount = 0;
    int score = 5000;
    char* fiveLetterWord = malloc(6*sizeof(char));

    FILE* wordsFile = fopen("words.txt", "r");

    while ( fscanf(wordsFile, "%s", fiveLetterWord) != EOF && wordCount < MAX_NUM_OF_WORDS)
    {
        wordsList[wordCount] = fiveLetterWord;
        wordCount++;

        fiveLetterWord = malloc(6*sizeof(char));
    }
    fclose(wordsFile);

    // start the game
    // pick a word randomly
    srand(time(NULL));
    char* answer = wordsList[rand()%wordCount];

    // do the game loop
    int numOfGuesses = 0;
    bool guessedCorrectly = false;
    char* guess = malloc(6*sizeof(char));

    while (numOfGuesses < 6 && !guessedCorrectly)
    {

        printf("Ingrese un palabra de 5 letras y presione enter: ");
        scanf("%s", guess);

        printf("Ingresaste '%s' \n", guess);

        int lenGuess = strlen(guess);
        if(lenGuess != 5 )
        {
            printf("La palabra debe contener 5 letras.\n");
            continue;
        }

        numOfGuesses += 1;

        char* clue = processGuess(answer, guess);
        guessedCorrectly = strcmp(clue, "GGGGG") == 0;
        score = calculateScore(score, clue, guessedCorrectly, numOfGuesses);

    }
    endGame(guessedCorrectly, numOfGuesses, score, answer);

    // clean memory
    for (int i = 0; i < wordCount; i++)
    {
        free(wordsList[i]);
    }

    free(wordsList);
    free(fiveLetterWord);
    free(guess);

    return 0;
}
