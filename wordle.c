#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_NUM_OF_WORDS 100

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

bool processGuess(const char* answer, const char* guess){
   // the clue
   char clue[6] = {'-', '-', '-', '-', '-', '\0'};

   // a set of flags indicating if that letter in the answer in used as clue
   bool answerFlags[5] = {false, false, false, false, false};

   // first pass, look for exact matches
   for (int i = 0; i < 5; i++){
      if (guess[i] == answer[i]){
         clue[i] = 'G';
         answerFlags[i] = true;
      }
   }

   // second pass, look there but elsewhere
   for (int i = 0; i < 5; i++){
      if (clue[i] == '-') {
         for (int j = 0; j < 5; j++){
            if (guess[i] == answer[j] && !answerFlags[j]){
               clue[i] = 'Y';
               answerFlags[j] = true;
               break;
            }
         }
      }
   }

   printf("%s\n", clue);
   return strcmp(clue, "GGGGG") == 0;
}

int main() {
   
   // load the words
   char** wordsList = calloc(MAX_NUM_OF_WORDS, sizeof(char*));

   int wordCount = 0;
   int points = 5000;
   char* fiveLetterWord = malloc(6*sizeof(char));

   FILE* wordsFile = fopen("words.txt", "r");

   while ( fscanf(wordsFile, "%s", fiveLetterWord) != EOF && wordCount < MAX_NUM_OF_WORDS) {
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

   while (numOfGuesses < 6 && !guessedCorrectly) {
      // get guess from player
      printf("Ingrese un palabra de 5 letras y presione enter: ");
      scanf("%s", guess);

      printf("Ingresaste '%s' \n", guess);

      int lenGuess = strlen(guess);
      if(lenGuess != 5 ){
         printf("La palabra debe contener 5 letras.\n");
         continue;
      }

      numOfGuesses += 1;

      // process guess
      guessedCorrectly = processGuess(answer, guess);
   }

   // display end of game message
   if (guessedCorrectly) {
      
      // revisar esto
      char txtIntento = "intentos!";

      if(numOfGuesses == 1){
         points = 10000;
         txtIntento = "intento!";
      }

      printf("Felicitaciones! Adivinaste la palabra en %d %s!\n", numOfGuesses, txtIntento);
   } 

   if(!guessedCorrectly) {
      printf("Perdiste! Usaste las 6 vidas sin adivinar... la palabra correcta es '%s'\n", answer);
      points = 0;
   }

   printf("Tú puntuación final es de %d puntos.\n", points);

   // clean memory
   for (int i = 0; i < wordCount; i++) {
      free(wordsList[i]);
   }
   free(wordsList);
   free(fiveLetterWord);
   free(guess);

   return 0;
}