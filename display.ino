//Different pins for the seven segment display
//Bottom left, bottom, bottom right, etc.
#define BL 13
#define B 12
#define BR 11
#define MD 7
#define TL 6
#define T 5
#define TR 4


//Intializing the pins as well as initializing 0 to be the first display
//The system will show how many cats are outside
void initDisplay() {
  pinMode(BL, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(BR, OUTPUT);
  pinMode(MD, OUTPUT);
  pinMode(TL, OUTPUT);
  pinMode(T, OUTPUT);
  pinMode(TR, OUTPUT);

  showNumber(0);
}

//States how to display each number on a seven segment display
const uint8_t SEVDISPLAY[] = {BL, B, BR, MD, TL, T, TR};

const uint8_t ERROR[] = {HIGH, HIGH, LOW, HIGH, HIGH, HIGH, LOW};
const uint8_t ZERO[] = {HIGH, HIGH, HIGH, LOW, HIGH, HIGH, HIGH};
const uint8_t ONE[] = {LOW, LOW, HIGH, LOW, LOW, LOW, HIGH};
const uint8_t TWO[] = {HIGH, HIGH, LOW, HIGH, LOW, HIGH, HIGH};
const uint8_t THREE[] = {LOW, HIGH, HIGH, HIGH, LOW, HIGH, HIGH};
const uint8_t FOUR[] = {LOW, LOW, HIGH, HIGH, HIGH, LOW, HIGH};
const uint8_t FIVE[] = {LOW, HIGH, HIGH, HIGH, HIGH, HIGH, LOW};
//67
const uint8_t SIX[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW};
const uint8_t SEVEN[] = {LOW, LOW, HIGH, LOW, LOW, HIGH, HIGH};

const uint8_t EIGHT[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
const uint8_t NINE[] = {LOW, LOW, HIGH, HIGH, HIGH, HIGH, HIGH};

const uint8_t *NUMS[] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE};

//Helper function that helps display the number
void showArr(uint8_t *arr) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(SEVDISPLAY[i], arr[i]);
  }
}

//Shows the number of cats are outside, gives an error when there are more than 9 cats outside
void showNumber(uint8_t num) {
  if (num > 9) {
    showArr(ERROR);
    return;
  }
  showArr(NUMS[num]);
}

/*
  Cycles through each of the states of the 
  7-segment display at .125s increments.
*/
void testDisplay() {
  showArr(ERROR);
  delay(125);
  
  for (int i = 0; i < 10; i++) {
    showNumber(i);
    delay(125);
  } 
}
