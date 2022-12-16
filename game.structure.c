// #include <stdbool.h>

struct Game{
   int score;
   bool isVictory;
   int gameNumber;
   char answer[6]; 
};

struct GameHighestLowestScore{
   int score;
   int gameNumber;
};