#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "game.structure.c"

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

#define YES "y"
#define NO "n"
#define FIRST_GAME 1

/*
 * @nameFunction "calculatePointsGreenYellow"
 * @brief Calcula el score de las letras correctas pero mal ubicadas (yellow).
 * 
 * @param int **attempts
 * @param int numOfGuesses
 * @return int totalScoreYellowLetters
*/
int calculatePointsGreenYellow(int **attempts, int numOfGuesses){
    int totalScoreYellowLetters = 0;

    for (int row = 0; row < numOfGuesses; row++)
    {
        for (int column = 0; column < MAX_LETTERS; column++)
        {
            int value = attempts[row][column];
            if(value != -1 && row != MAX_GUESSES - 1){
                totalScoreYellowLetters = totalScoreYellowLetters + value;
            }
        }
        
    }

    return totalScoreYellowLetters;
}

/*
 * @nameFunction "calculateScore"
 * @brief Calcula el score obtenido en la partida.
 * 
 * @param int **attempts
 * @param bool guessedCorrectly
 * @param int numOfGuesses
 * @return int score
*/
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

/*
 * @nameFunction "checkLettersGreen"
 * @brief Busca si existe una [G = "100 puntos"] en el intento actual.
 * Si la hay, establece 0 (NO_POINTS) a las siguientes filas en la misma posicion de la letra correcta ingresada.     
 * Si no la hay, pasa a la letra siguiente.
 * 
 * @param int **attempts
 * @param int numOfGuesses
 * @return void
*/
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

/*
 * @nameFunction "checkRepeatedLetterYellow"
 * @brief Busca la letra amarilla ingresada en el array de letras amarillas ingresadas en los turnos anteriores.
 * 
 * @param char *inputLetters
 * @param char yellowLetter
 * @return bool existLetterInArray
*/
bool checkRepeatedLetterYellow(char *inputYellowLetters, char yellowLetter)
{
    // inputYellowLetters -> array unidimensional de letras amarillas (correctas en lugar quivocado) ingresadas anteriormente.
    // letter -> letra amarilla de la palabra ingresada.

    bool existLetterInArray = strpbrk(inputYellowLetters, &yellowLetter) != 0; 
    if(existLetterInArray){
        return existLetterInArray;
    }
    
    int len = strlen(inputYellowLetters);
    inputYellowLetters[len] = yellowLetter;
    inputYellowLetters[len+1] = '\0';

    return existLetterInArray;
}

/*
 * @nameFunction "processGuess"
 * @brief Procesa y compara la palabra ingresada por el usuario contra la respuesta. 
 * @param char *answer
 * @param char *guess
 * @param char **attempts
 * @param int numOfGuesses
 * @param char *inputYellowLetters
 * @return bool guessedCorrectly
*/
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

/*
 * @nameFunction "endGame"
 * @brief Muestra informacion al usuario de la partida jugada. 
 * @param bool guessedCorrectly
 * @param int numOfGuesses
 * @param char *answer
 * @param int score
 * @return struct Game
*/
struct Game endGame(bool guessedCorrectly, int numOfGuesses, char *answer, int score)
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

    printf("Tú puntuación final es de %d puntos.\n\n", score);

    struct Game game;

    game.isVictory = guessedCorrectly;
    game.score = score;
    strcpy(game.answer, answer); 

    return game;
}

/*
 * @nameFunction "loadWords"
 * @brief Carga todas las palabras del archivo .txt a un array. 
 * @param char **wordList
 * @param char *fiveLetterWord
 * @return int wordCount
*/
int loadWords(char **wordsList, char *fiveLetterWord)
{

    FILE *wordsFile = fopen("words.txt", "r");
    int wordCount = 0;

    while (fscanf(wordsFile, "%s", fiveLetterWord) != EOF && wordCount < MAX_NUM_OF_WORDS)
    {
        wordsList[wordCount] = fiveLetterWord;
        wordCount++;

        fiveLetterWord = malloc(6 * sizeof(char));
    }
    fclose(wordsFile);
    return wordCount;
}

/*
 * @nameFunction "playGame"
 * @brief Inicia una partida. 
 * @param void
 * @return struct Game
*/
struct Game playGame(){

    char **wordsList = calloc(MAX_NUM_OF_WORDS, sizeof(char *));
    char *fiveLetterWord = malloc(6 * sizeof(char));
    char *inputYellowLetters = (char*)malloc ( MAX_SIZE_MEMORY_YELLOW_LETTERS * sizeof (char));

    int wordCount = loadWords(wordsList, fiveLetterWord);

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
    struct Game game = endGame(guessedCorrectly, numOfGuesses, answer, score);

    free(wordsList);
    free(fiveLetterWord);
    free(guess);
    free(attempts);
    free(inputYellowLetters);

    return game;
};

/*
 * @nameFunction "selectNUmberOfGames"
 * @brief Solicita al usuario la cantidad de partidas que desea jugar y lo devuelve a main. 
 * @param void
 * @return int sessionsQuantity
*/
int selectNumberOfGames(){
    int sessionsQuantity = 0;

    while(sessionsQuantity < 1 || sessionsQuantity > 8){
        
        printf("Cuantas partidas deseas jugar? (max 8): ");
        scanf("%d", &sessionsQuantity);
        
        // si el sessionsQuantityingresa un char equivodacademente.
        while (sessionsQuantity == 0) {
            fprintf(stderr, "Debe ingresar un numero del 1 al 8: ");
            do {
                sessionsQuantity = getchar();
            } while ((sessionsQuantity != EOF) && (sessionsQuantity != '\n'));
            sessionsQuantity = scanf("%d", &sessionsQuantity);
        }

        if(sessionsQuantity < sessionsQuantity || sessionsQuantity > 8){
            printf("Ingrese una sessionsQuantitya cantidad de partidas. \n\n");
            sessionsQuantity;
        }
    }
    return sessionsQuantity;
}

/*
 * @nameFunction "askFinishSession"
 * @brief Pregunta al usuario si desea terminar la sesion. 
 * @param void
 * @return bool isFinishSession
*/
bool askFinishSession(){
    printf("Desea finalizar el juego? [y/n]: ");
    char finishSession[1] = {"x"};

    while(finishSession[0] != YES[0] && finishSession[0] != NO[0]){
        scanf("%s", &finishSession[0]);
        if(finishSession[0] != YES[0] && finishSession[0] != NO[0]){
            printf("Debe ingresar una respuesta valida. [y/n] \n");
        }
        continue;
    }

    return finishSession[0] == YES[0] ? true : false;
}

/*
 * @nameFunction "showWordsPlayedAndScore"
 * @brief Muestra al usuario las palabras empleadas en cada partida con los puntajes obtenidos.
 * @param struct game Games[]
 * @param int gamesQuantity
 * @return void
*/
void showWordsPlayedAndScore(struct Game games[], int gamesQuantity){

    // int lengthArrayGames = sizeof(games) / sizeof(struct Game); 
    for (int gameIndex = 0; gameIndex < gamesQuantity; gameIndex++)
    {
        struct Game game = games[gameIndex];
        int gameNumber = gameIndex + 1;
        printf("\n");
        printf("Partida nro. %d: \n", gameNumber);
        printf("    respuesta: %s \n", game.answer);
        printf("    puntaje: %d\n\n", game.score);
    }
}

/*
 * @nameFunction "findAndShowGamesMaxScore"
 * @brief Muestra al usuario en cuál o cuáles partidas se obtuvo más alto.
 * @param struct game Games[]
 * @param int gamesQuantity
 * @return void
*/
void findAndShowGamesMaxScore(struct Game games[], int gamesQuantity){

    int maxScore = -1;
    int numberGameMaxScore = 0;

    for (int gameIndex = 0; gameIndex < gamesQuantity; gameIndex++)
    {
        struct Game game = games[gameIndex];
    
        if(game.score > maxScore){
            maxScore = game.score;
        }
    }    

    int countGamesMaxScore = 0;
    for (int gameIndex = 0; gameIndex < gamesQuantity; gameIndex++)
    {
        struct Game game = games[gameIndex];
        
        if(game.score == maxScore){
            countGamesMaxScore++;
        }
    }

    int gamesMaxScore[countGamesMaxScore];
    int indexGameMaxScore = 0;
    
    for (int gameIndex = 0; gameIndex < gamesQuantity; gameIndex++)
    {
        struct Game game = games[gameIndex];
        
        if(game.score == maxScore){
            gamesMaxScore[indexGameMaxScore] = game.gameNumber;
            indexGameMaxScore++;
        }
    }

    printf("Partidas con mas puntaje: ");
    for (int i = 0; i < countGamesMaxScore; i++)
    {
        printf("%d-", gamesMaxScore[i]);
    }    
    printf("\n");
}

/*
 * @nameFunction "findAndShowGamesMinScore"
 * @brief Muestra al usuario en cuál o cuáles partidas se obtuvo más bajo.
 * @param struct game Games[]
 * @param int gamesQuantity
 * @return void
*/
void findAndShowGamesMinScore(struct Game games[], int gamesQuantity){

    int minScore = 100000000;
    int numberGameMinScore = 0;

    for (int gameIndex = 0; gameIndex < gamesQuantity; gameIndex++)
    {
        struct Game game = games[gameIndex];
    
        if(game.score < minScore){
            minScore = game.score;
        }        
    }    

    int countGamesMinScore = 0;
    for (int gameIndex = 0; gameIndex < gamesQuantity; gameIndex++)
    {
        struct Game game = games[gameIndex];
        
        if(game.score == minScore){
            countGamesMinScore++;
        }
    }

    int gamesMinScore[countGamesMinScore];
    int indexGameMinScore = 0;
    
    for (int gameIndex = 0; gameIndex < gamesQuantity; gameIndex++)
    {
        struct Game game = games[gameIndex];
        
        if(game.score == minScore){
            gamesMinScore[indexGameMinScore] = game.gameNumber;
            indexGameMinScore++;
        }
    }

    printf("Partidas con menos puntaje: ");
    for (int i = 0; i < countGamesMinScore; i++)
    {
        printf("%d-", gamesMinScore[i]);
    }    
    printf("\n");

    // printf("La partida nro. %d obtuvo el puntaje más bajo \n", numberGameMinScore);
}

/*
 * @nameFunction "getAverageScoreOfWins"
 * @brief Devuelve el promedio de los puntajes en que logró una victoria.
 * @param struct game Games[]
 * @param int gamesQuantity
 * @return float averageScoreOfWins
*/
float getAverageScoreOfWins(struct Game games[], int gamesQuantity){
    int totalScoreOfWins = 0;

    for(int gameIndex = 0; gameIndex < gamesQuantity; gameIndex++){
        struct Game game = games[gameIndex];
        totalScoreOfWins = totalScoreOfWins + game.score;
    }

    float averageScoreOfWins = totalScoreOfWins / gamesQuantity;
    return averageScoreOfWins;
}

int main()
{    
    int gamesQuantity;
    gamesQuantity = selectNumberOfGames();

    // reservar memoria dinamicamente
    struct Game games[8];    

    int gameNumber = FIRST_GAME;
    bool endSession;

    while(gameNumber <= gamesQuantity && !endSession){
        int actualGame = 0;
        endSession = true;
        
        printf("Partida nro. %d de %d. \n\n", gameNumber, gamesQuantity);
        
        struct Game game = playGame();
        game.gameNumber = gameNumber;

        // guardar partidas en array "games".
        games[gameNumber - 1] = game; 

        // codigo una vez terminado el juego.
        if(gamesQuantity > gameNumber){

            endSession = askFinishSession();
            if(!endSession){
                gameNumber++;
            }
        }
        
    }
    gamesQuantity = gameNumber;

    // indicar las palabras empleadas en cada partida con los puntajes obtenidos.
    showWordsPlayedAndScore(games,gamesQuantity);

    // Señalar en cuál o cuáles partidas se obtuvo el puntaje más alto y el más bajo.
    if(gamesQuantity == 1){
        printf("En una sesion, juega 2 o más partidas para comparar puntaje. \n\n");
    }

    if(gamesQuantity > 1){
        findAndShowGamesMinScore(games, gamesQuantity);
        findAndShowGamesMaxScore(games, gamesQuantity);
    }

    // Señalar el promedio de los puntajes en que logró una victoria.
    float averageScoreOfWins = getAverageScoreOfWins(games,gamesQuantity);
    printf("El puntaje promedio de las victorias es de %.6f \n\n\n", averageScoreOfWins);

    return 0;
}
