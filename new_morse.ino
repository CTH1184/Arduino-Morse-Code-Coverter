/**
 * @author Chris Vanden Boom, Charlie Harrington
 * @version 8.15.24
 * This program asks user for terminal input and then converts this input into morse code
 * The LED flashes and piezzo buzzer beeps to show morse code
 * Use $<command> for custom commands in terminal
 */
const int PG = 11;
// RBG pins
const int VR = 9;
const int VG = 10;
const int VB = 3;
//Piezo Pin
const int PZ = 2;

// keeps track of custom color in RGB LED
String currentcol = "red";
String mode = "flash";

// times in between dits and dahs
const int dah = 750;     // time on for dash
const int dit = 250;     // time on for dot
const int intra = 250;   // in between dashes and dots
const int inter = 750;   // in between letters
const int space = 1000;  // in between words

// initialize list of all characters corresponding to morse codes
const int characters[37] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                             'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                             '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', ' ' };

// initialize 2D array of codes corresponding with 0's for dots, 1's for dashes, -1 for space, 2's are fillers
const int codes[][37] = { { 0, 1, 2, 2, 2 }, { 1, 0, 0, 0, 2 }, { 1, 0, 1, 0, 2 }, { 1, 0, 0, 2, 2 }, { 0, 2, 2, 2, 2 }, { 0, 0, 1, 0, 2 }, { 1, 1, 0, 2, 2 }, { 0, 0, 0, 0, 2 }, { 0, 0, 2, 2, 2 }, { 0, 1, 1, 1, 2 }, { 1, 0, 1, 0, 2 }, { 0, 1, 0, 0, 2 }, { 1, 1, 2, 2, 2 }, { 1, 0, 2, 2, 2 }, { 1, 1, 1, 2, 2 }, { 0, 1, 1, 0, 2 }, { 1, 1, 0, 1, 2 }, { 0, 1, 0, 2, 2 }, { 0, 0, 0, 2, 2 }, { 1, 2, 2, 2, 2 }, { 0, 0, 1, 2, 2 }, { 0, 0, 0, 1, 2 }, { 0, 1, 1, 2, 2 }, { 1, 0, 0, 1, 2 }, { 1, 0, 1, 1, 2 }, { 1, 1, 0, 0, 2 }, { 0, 1, 1, 1, 1 }, { 0, 0, 1, 1, 1 }, { 0, 0, 0, 1, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 0 }, { 1, 0, 0, 0, 0 }, { 1, 1, 0, 0, 0 }, { 1, 1, 1, 0, 0 }, { 1, 1, 1, 1, 0 }, { 1, 1, 1, 1, 1 }, { -1, 2, 2, 2, 2 } };

/**
 * Set up pins and serial monitor
 */
void setup() {
  pinMode(PG, OUTPUT);
  pinMode(VR, OUTPUT);
  pinMode(VG, OUTPUT);
  pinMode(VB, OUTPUT);
  pinMode(PZ, OUTPUT);
  Serial.begin(9600);
}

// driver code
void loop() {
// label
start:

  // get user input
  Serial.println("Enter a message to convert to Morse Code or");
  Serial.println("type $help for a list of commands");
  // wait for user to input data
  while (Serial.available() == 0) {
    digitalWrite(PG, HIGH);
  }
  // define message as user entered input
  String message = Serial.readString();
  // convert to lowercase for easier translation
  message.toLowerCase();
  // Turn off green
  digitalWrite(PG, LOW);
  // store length of message in integer to create the char array
  int len = message.length() + 1;
  // initialize char array
  char letters[len];
  // format Message string to char array
  message.toCharArray(letters, len);

  // detect if a command is entered
  if (message.charAt(0) == '$') {
    run_command(message.substring(1, len));
    goto start;
  }
  Serial.println("Converting: " + message);
  for (int i = 0; i < len - 2; i++) {
    int ind = get_code_index(letters[i]);
    execute(codes[ind]);
  }
}

// returns the index of the code corresponding to paramater c
int get_code_index(char c) {
  for (int i = 0; i < sizeof(characters); i++) {
    if (c == characters[i]) {
      return i;
    }
  }
  return -1;
}

/**
 * Flashes the correct color of RGB LED based off of field currentcol
 * Uses int array to determine length of flashes
 * @param code
 */
void flash_morse(int code[]) {
  // make sure LED is off
  off();
  // iterate through each 0, 1, and -1 and flash red LED with corresponding times
  for (int i = 0; i < 5; i++) {
    // 0 corresponds to a dot
    if (code[i] == 0) {
      flash();
      // delay for dit
      delay(dit);
      // turn off in between character
      off();
      delay(intra);
    }
    // 1 corresponds to a dash
    else if (code[i] == 1) {
      flash();
      // delay for dah
      delay(dah);
      // turn off in between character
      off();
      delay(intra);
    }
    // -1 is a space
    else if (code[i] == -1) {
      // delay for space
      delay(space);
    }
  }
  // delay in between letters
  off();
  delay(inter);
}

/**
 * Prints input morse in dots and dashes for easier bug testing
 * @param code
 */
void print_morse(int code[]) {
  // create temporary string to hold result
  String temp = "";
  // iterate through each 0, 1, and -1 and flash red LED with corresponding times
  for (int i = 0; i < 5; i++) {
    // 0 corresponds to a dot
    if (code[i] == 0) {
      temp += ".";
    }
    // 1 corresponds to a dash
    else if (code[i] == 1) {
      temp += "-";
    } else if (code[i] == -1) {
      temp += " / ";
    }
  }
  temp += " ";
  Serial.print(temp);
  Serial.print("\n");
}

/**
 * Prints or flashes code based on field mode
 * @param code
 */
void execute(int code[]) {
  if (mode.equals("print")) {
    print_morse(code);
  } else if (mode.equals("flash")) {
    flash_morse(code);
  } else if (mode.equals("both")) {
    print_morse(code);
    flash_morse(code);
  }
}

/**
 * Turns LED on baesd on field currentcol
 */
void flash() {
  digitalWrite(PZ, HIGH);
  if (currentcol.equals("red")) {
    to_red();
  } else if (currentcol.equals("green")) {
    to_green();
  } else if (currentcol.equals("blue")) {
    to_blue();
  } else if (currentcol.equals("purple")) {
    to_purple();
  } else if (currentcol.equals("yellow")) {
    to_yellow();
  } else if (currentcol.equals("cyan")) {
    to_cyan();
  } else if (currentcol.equals("white")) {
    to_white();
  }
}

/**
 * Turns off RGB LED
 */
void off() {
  digitalWrite(VR, LOW);
  digitalWrite(VG, LOW);
  digitalWrite(VB, LOW);
  digitalWrite(PZ, LOW);
}

/**
 * Turns LED on in red
 */
void to_red() {
  digitalWrite(VR, HIGH);
  digitalWrite(VG, LOW);
  digitalWrite(VB, LOW);
}

/**
 * Turns LED on in green
 */
void to_green() {
  digitalWrite(VG, HIGH);
  digitalWrite(VR, LOW);
  digitalWrite(VB, LOW);
}

/**
 * Turns LED on in blue
 */
void to_blue() {
  digitalWrite(VB, HIGH);
  digitalWrite(VR, LOW);
  digitalWrite(VG, LOW);
}

/**
 * Turns LED on in purple
 */
void to_purple() {
  digitalWrite(VR, HIGH);
  digitalWrite(VB, HIGH);
  digitalWrite(VG, LOW);
}

/**
 * Turns LED on in yellow
 */
void to_yellow() {
  digitalWrite(VR, HIGH);
  digitalWrite(VG, HIGH);
  digitalWrite(VB, LOW);
}

/**
 * Turns LED on in cyan
 */
void to_cyan() {
  digitalWrite(VG, HIGH);
  digitalWrite(VB, HIGH);
  digitalWrite(VR, LOW);
}

/**
 * Turns LED on in white
 */
void to_white() {
  digitalWrite(VG, HIGH);
  digitalWrite(VB, HIGH);
  digitalWrite(VR, HIGH);
}

void run_command(String command) {
  command.trim();
  if (command == "print") {
    Serial.println("Mode set to print");
    mode = "print";
  } else if (command == "flash") {
    Serial.println("Mode set to flash");
    mode = "flash";
  } else if (command == "both") {
    Serial.println("Mode set to both flash and print");
    mode = "both";
  } else if (command == "help") {
    Serial.println("HELP");
    Serial.println("*******************************");
    Serial.println("Colors - ");
    Serial.println("$red\n$blue\n$green\n$purple\n$yellow\n$cyan\n$white");
    Serial.println("*******************************");
    Serial.println("$flash - sets mode to flash");
    Serial.println("$print - sets mode to print");
    Serial.println("$both - prints and flashes");
    Serial.println("$thanks - sends a thank you and provides my links!");
    Serial.println("*******************************");
  } else if (command == "red") {
    currentcol = "red";
    Serial.println("Color changed to red");
  } else if (command == "blue") {
    currentcol = "blue";
    Serial.println("Color changed to blue");
  } else if (command == "green") {
    currentcol = "green";
    Serial.println("Color changed to green");
  } else if (command == "purple") {
    currentcol = "purple";
    Serial.println("Color changed to purple");
  } else if (command == "yellow") {
    currentcol = "yellow";
    Serial.println("Color changed to yellow");
  } else if (command == "cyan") {
    currentcol = "cyan";
    Serial.println("Color changed to cyan");
  } else if (command == "white") {
    currentcol = "white";
    Serial.println("Color changed to white");
  } else if (command == "thanks") {
    Serial.println("Thank you for checking this out!");
    Serial.println("Feel free to reach out to me and view more of my work via...");
    Serial.println("Email: ctharring@gmail.com");
    Serial.println("LinkedIn: https://www.linkedin.com/in/charlie-harrington-654b822b0/");
    Serial.println("Github: https://github.com/CTH1184");

  } else {
    Serial.println("Error: command not recognized");
  }
}