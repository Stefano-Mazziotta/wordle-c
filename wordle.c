#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_NUM_OF_WORDS 100

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
      printf("Input a 5-letter word and press enter: ");
      scanf("%s", guess);
      printf("You have guessed %s \n", guess);

      numOfGuesses += 1;

      // process guess
      guessedCorrectly = processGuess(answer, guess);
   }

   // display end of game message
   if (guessedCorrectly) {
      printf("Congratulations! You guessed the correct word in %d times!\n", numOfGuesses);
   } else {
      printf("You have used up yours guesses... the correct word is %s\n", answer);
   }

   for (int i = 0; i < wordCount; i++) {
      free(wordsList[i]);
   }
   free(wordsList);
   free(fiveLetterWord);
   free(guess);

   return 0;
}