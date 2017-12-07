/**
 * \file pluginloadtest by Michael Klopfer, PhD (Univ. of Calif. Irvine) 2017 and Brandon Metcalf
 * \based on the Sperkfun file MP3Shield_Library_Demo.ino
 * \brief Example sketch of using the MP3Shield Arduino driver
 * \remarks comments are implemented with Doxygen Markdown format
 *
 * \author Bill Porter
 * \author Michael P. Flaga
 *
 * This sketch listens for commands from a serial terminal (like the Serial
 * Monitor in the Arduino IDE).
 *
 * Sketch assumes you have .053 files with filenames like "specAn_1.053",
 * "specAn_2.053", etc on an SD card loaded into the shield.
 */

#include <SPI.h>

//Add the SdFat Libraries
#include <SdFat.h>
#include <SdFatUtil.h>

//and the MP3 Shield Library
#include <SFEMP3Shield.h>

// Below is not needed if interrupt driven. Safe to remove if not using.
#if defined(USE_MP3_REFILL_MEANS) && USE_MP3_REFILL_MEANS == USE_MP3_Timer1
#include <TimerOne.h>
#elif defined(USE_MP3_REFILL_MEANS) && USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer
#include <SimpleTimer.h>
#endif

/**
 * \brief Object instancing the SdFat library.
 * principal object for handling all SdCard functions.
 */
SdFat sd;

/**
 * \brief Object instancing the SFEMP3Shield library.
 * principal object for handling all the attributes, members and functions for the library.
 */
SFEMP3Shield MP3player;

//With default library, this program will work on boards with the following pin configuration to the Arduino UNO: sck to pin 13, MISO to pin 12, MOSI to pin 11, CS to 9, XRESET to 8, X-DCS to 7, X-CS to 6, DREQ to 2
/**
 * \brief Setup the Arduino Chip's feature for our use.
 *
 * After Arduino's kernel has booted initialize basic features for this
 * application, such as Serial port and MP3player objects with .begin.
 * Along with displaying the Help Menu.
 */

#define BASE 0x1810

void SetBands(const short int frequency[10], int nBands);
void GetAnalysis(unsigned int data[10]);
void readFreq();
const short frequency[10] = {130,250,300,550,850,1000, 2150,3050,5080,8063}; 
int nBands = 10;
unsigned int data[10] = {0};

 
void setup() {

  uint8_t result; //result code from some function as to be tested at later time.
  Serial.begin(115200);
  SPI.begin();

  Serial.print(F("Free RAM = ")); // available in Version 1.0 F() bases the string to into Flash, to use less SRAM.
  Serial.print(FreeRam(), DEC);  // FreeRam() is provided by SdFatUtil.h
  Serial.println(F(" Should be a base line of 1095, on ATmega328 when using INTx"));


  //Initialize the SdCard.
  if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");

  //Initialize the MP3 Player Shield
  result = MP3player.begin();
  //check result, see readme for error codes.
  if (result != 0) {
    Serial.print(F("Error code: "));
    Serial.print(result);
    Serial.println(F(" when trying to start MP3 player"));
    if ( result == 6 ) {
      Serial.println(F("Warning: patch file not found, skipping.")); // can be removed for space, if needed.
      Serial.println(F("Use the \"d\" command to verify SdCard can be read")); // can be removed for space, if needed.
    }
  }
  help();
}

//------------------------------------------------------------------------------
/**
 * \brief Main Loop the Arduino Chip
 *
 * This is called at the end of Arduino kernel's main loop before recycling.
 * And is where the user's serial input of bytes are read and analyzed by
 * parsed_menu.
 *
 * Additionally, if the means of refilling is not interrupt based then the
 * MP3player object is serviced with the availaible function.
 *
 * \note Actual examples of the libraries public functions are implemented in
 * the parse_menu() function.
 */
void loop() {

  // Below is only needed if not interrupt driven. Safe to remove if not using.
#if defined(USE_MP3_REFILL_MEANS) \
    && ( (USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer) \
    ||   (USE_MP3_REFILL_MEANS == USE_MP3_Polled)      )

  MP3player.available();
#endif

  if (Serial.available()) {
    parse_menu(Serial.read()); // get command from serial input
  }

  delay(100);
}

//------------------------------------------------------------------------------
/**
 * \brief Decode the Menu.
 *
 * Parses through the characters of the users input, executing corresponding
 * MP3player library functions and features then displaying a brief menu and
 * prompting for next input command.
 */
void parse_menu(byte key_command) {

  uint8_t result; // result code from some function as to be tested at later time.
  
  // Note these buffer may be desired to exist globably.
  // but do take much space if only needed temporarily, hence they are here.
  char title[30]; // buffer to contain the extract the Title from the current filehandles
  char artist[30]; // buffer to contain the extract the artist name from the current filehandles
  char album[30]; // buffer to contain the extract the album name from the current filehandles

  Serial.print(F("Received command: "));
  Serial.write(key_command);
  Serial.println(F(" "));


  //if X, stop the current track
  if (key_command == 'X') {
    Serial.println(F("Stopping"));
    MP3player.stopTrack();

    //if 1-9, play corresponding track
  } else if (key_command >= '1' && key_command <= '9') {
    //convert ascii numbers to real numbers
    key_command = key_command - 48;

#if USE_MULTIPLE_CARDS
    sd.chvol(); // assign desired sdcard's volume.
#endif
    //tell the MP3 Shield to play a track
    result = MP3player.playTrack(key_command);

    //check result, see readme for error codes.
    if (result != 0) {
      Serial.print(F("Error code: "));
      Serial.print(result);
      Serial.println(F(" when trying to play track"));
    } else {

      Serial.println(F("Playing:"));

      //we can get track info by using the following functions and arguments
      //the functions will extract the requested information, and put it in the array we pass in
      MP3player.trackTitle((char*)&title);
      MP3player.trackArtist((char*)&artist);
      MP3player.trackAlbum((char*)&album);

      //print out the arrays of track information
      Serial.write((byte*)&title, 30);
      Serial.println();
      Serial.print(F("by:  "));
      Serial.write((byte*)&artist, 30);
      Serial.println();
      Serial.print(F("Album:  "));
      Serial.write((byte*)&album, 30);
      Serial.println();
    }
  }
    

  else if (key_command == 'T') {
  Serial.println(F("Applying ADMon EQ Plugin"));
  char pluginname[] = "admoneq.053"; //Name of plugin
  result = MP3player.VSLoadUserCode(pluginname); //load plugin
  if (result != 0) {
    Serial.print(F("Error code: "));   // return Any Value other than zero indicates a problem occurred. * - 0 indicates that upload was successful.  * - 1 indicates the upload can not be performed while currently streaming music. * - 2 indicates that desired file was not found.* - 3 indicates that the VSdsp is in reset.
    Serial.print(result);
    Serial.println(F("  Load function terminated"));
    }
    else {
      Serial.println(F("Plugin Loaded!"));
    }
//    MP3player.Mp3WriteRegister(SCI_AIADDR, 0x0220);
//    Serial.println(MP3player.Mp3ReadRegister(SCI_AIADDR));
//    MP3player.Mp3WriteRegister(SCI_MODE, 0x0800);
  }

 else if (key_command == 'S') {
  Serial.println(F("Attempting to load Spectrum Analyzer plugin"));
  char pluginname[] = "sa.053"; //Name of plugin
  result = MP3player.VSLoadUserCode(pluginname); //load plugin
  if (result != 0) {
    Serial.print(F("Error code: "));   // return Any Value other than zero indicates a problem occurred. * - 0 indicates that upload was successful.  * - 1 indicates the upload can not be performed while currently streaming music. * - 2 indicates that desired file was not found.* - 3 indicates that the VSdsp is in reset.
    Serial.print(result);
    Serial.println(F("  Load function terminated"));
    }
    else {
      Serial.println(F("Plugin Loaded!"));
    }
   SetBands(frequency,nBands);
   //GetAnalysis(data,frequency);
   readFreq();
   Serial.println(nBands);
  }
  
else if (key_command == 'A') {
  GetAnalysis(data,frequency);
  readFreq();
}
 else if (key_command == 'P') {

   Serial.println(F("Activating loaded plugins"));
 //Settings for Plugin control  
  int ADM_volume= -3; //set volume level between -3 and -31 (-31 is min)
//already defined in the SFEMP3shield.h file
  #define SCI_AIADDR            0x0A //register address (dec of 10)
  #define SM_LINE1            0x4000
  #define SCI_AICTRL0           0x0C //this is one of 3
  
  union twobyte MP3AIADDR;
  union twobyte MP3AICTRL0;

 MP3AIADDR.word = MP3player.Mp3ReadRegister(SCI_AIADDR); //two byte response
 Serial.println(MP3AIADDR.word);
    MP3player.Mp3WriteRegister(SCI_AICTRL0, 0xbb80);
    MP3player.Mp3WriteRegister(SCI_AICTRL1, 0x1000);
    MP3player.Mp3WriteRegister(SCI_AICTRL2, 0x0080);
    MP3player.Mp3WriteRegister(SCI_AICTRL3, 0x0000);

   // Enable Mixer Patch
    MP3AIADDR.word = 0x0220; //value to enable
    Serial.println(SCI_AIADDR);
    MP3player.Mp3WriteRegister(SCI_AIADDR, MP3AIADDR.word);
    MP3AIADDR.word = MP3player.Mp3ReadRegister(SCI_AIADDR); //two byte response
    Serial.println(MP3AIADDR.word);
 }
 
  else if (key_command == 'R') {
    MP3player.stopTrack();
    MP3player.vs_init();
    Serial.println(F("Reseting VS10xx chip"));

  } else if (key_command == 't') {
    int8_t teststate = MP3player.enableTestSineWave(126);
    if (teststate == -1) {
      Serial.println(F("Un-Available while playing music or chip in reset."));
    } else if (teststate == 1) {
      Serial.println(F("Enabling Test Sine Wave"));
    } else if (teststate == 2) {
      MP3player.disableTestSineWave();
      Serial.println(F("Disabling Test Sine Wave"));
    }

  }else if (key_command == 'O') {
    MP3player.end();
    Serial.println(F("VS10xx placed into low power reset mode."));

  } else if (key_command == 'o') {
    MP3player.begin();
    Serial.println(F("VS10xx restored from low power reset mode."));

  }else if (key_command == 'h') {
    help();
  }

  // print prompt after key stroke has been processed.
  Serial.println(F("Enter 1-9,X,P,A,S,R,T,t,O,o,h :"));
}

//------------------------------------------------------------------------------
/**
 * \brief Print Help Menu.
 * Prints a full menu of the commands available along with descriptions.
 */
void help() {
  Serial.println(F("Arduino SFEMP3Shield Library Example:"));
  Serial.println(F(" courtesy of Bill Porter & Michael P. Flaga"));
  Serial.println(F("COMMANDS:"));
  Serial.println(F(" [1-9] to play song from SD Card"));
  Serial.println(F(" [P] to activate plugins"));
  Serial.println(F(" [X] to stop playing a track"));
  Serial.println(F(" [A] to get data"));
  Serial.println(F(" [S] to Load spectrum analyzer app2 Patch"));
  Serial.println(F(" [R] Resets and initializes VS10xx chip."));
  Serial.println(F(" [T] to test the loading of the ADmon EQ.053 plugin"));
  Serial.println(F(" [t] to toggle sine wave test"));
  Serial.println(F(" [O} turns OFF the VS10xx into low power reset."));
  Serial.println(F(" [o} turns ON the VS10xx out of low power reset."));
  Serial.println(F(" [h] this help"));
}


// Function modified from user 'CheeseBurger' from vsdsp-forum.com
void GetAnalysis(unsigned int data[10],const short int frequency[10]) {
  if(!frequency) return;
  delay(100);
  
  Serial.println("loading");
  //Get nBands
  MP3player.Mp3WriteRegister(SCI_WRAMADDR, BASE + 2);
  
  Serial.println("loading1");
  int nBands = MP3player.Mp3ReadRegister(SCI_WRAM); //direct integer write of address
  
  Serial.print("There are ");
  Serial.print(nBands,DEC);
  Serial.println(" bands\n");
  //nBands = 14; // Ignore incorrect nBands

  //Read analysis
  MP3player.Mp3WriteRegister(SCI_WRAMADDR, BASE + 4);
  for(int i = 0; i < nBands; i++) {
    //data[i] = MP3player.Mp3ReadRegister(6);
    data[i] = MP3player.Mp3ReadRegister(6) & 31; //Current value is in bits 0-5
    Serial.print("Spectrum Data: ");
    Serial.println(data[i]);
  }
}


// Function modified from user 'CheeseBurger' from vsdsp-forum.com
void SetBands(const short int frequency[10], int nBands) {
  if(!frequency || nBands > 23) return;

  //Set band frequencies
  int i;
  MP3player.Mp3WriteRegister(SCI_WRAMADDR, BASE + 58);
  for(i = 0; i < nBands; i++) {
    MP3player.Mp3WriteRegister(SCI_WRAM, frequency[i]);
  }
// Register(SCI_WRAM, 25000); // Terminate partial frequency lists with 25000
  
  //Reactivate Analyzer
  MP3player.Mp3WriteRegister(SCI_WRAMADDR, BASE + 1);
  MP3player.Mp3WriteRegister(SCI_WRAM, 0);
}


void readFreq(){
  MP3player.Mp3WriteRegister(SCI_WRAMADDR, BASE + 1);
  short int frequency[10] = {0};
  int rate = MP3player.Mp3ReadRegister(SCI_WRAM);
  int bands = MP3player.Mp3ReadRegister(SCI_WRAM);

  Serial.print(" rate = "); Serial.println(rate);
  Serial.print(" bands = "); Serial.println(bands);

  int i;
  for(i = 0; i < bands ; i++){
    int a;
//    MP3player.Mp3WriteRegister(SCI_WRAMADDR, BASE + 0x1c + 3 * i); // from doc
    MP3player.Mp3WriteRegister(SCI_WRAMADDR, BASE + 0x15 + 3 * i); //from forum for appl-2
    a = MP3player.Mp3ReadRegister(SCI_WRAM);
    frequency[i] = (long) rate * a >> 11;

    Serial.print("i = "); Serial.print(i); Serial.print(" ");
    Serial.print(", a = ");Serial.print(a); Serial.print(" ");
    Serial.print(", freq[i] = ");Serial.print(frequency[i]); Serial.println(" ");
  }
}

