/* Aviso

  necesitas instalar las siguiente  librerias:
  Arduino MIDI Library : https://playground.arduino.cc/Main/MIDILibrary
  Hell Drum: https://www.arduino.cc/reference/en/libraries/hello-drum/ (se instala a traves de Library Manager del IDE de Arduino)


*/

#include <hellodrum.h>
#include <MIDI.h>
#include <LiquidCrystal.h>

MIDI_CREATE_DEFAULT_INSTANCE();

//LCD pin define pin46 contrast
LiquidCrystal lcd(48, 49, 52, 53, 50, 51); //(rs, en, d4, d5, d6, d7)

//Entrada analogica de los pads
HelloDrum kick(0);
HelloDrum snare(1);
HelloDrum hihat(2);
HelloDrum hihatPedal(15);
HelloDrum ride(3);
HelloDrum tom1(14);
HelloDrum tom2(13);
HelloDrum ftom1(8);
//HelloDrum ftom2(9);
HelloDrum lcymbal(10);
HelloDrum rcymbal(11);
 

//botones de control
HelloDrumButton button(38, 39, 36, 34, 37); //(EDIT,UP,DOWN,NEXT,BACK)

void setup()
{
  //LCD Contrast
 pinMode(46, OUTPUT);  
 analogWrite(46, 0);
  
  
  //if you use ESP32, you have to uncomment the next line.
  //EEPROM_ESP.begin(512);
  //If you use Hairless MIDI, you have to comment out the next line.
  MIDI.begin(10);
  //And uncomment the next two lines. Please set the baud rate of Hairless to 38400.
  //MIDI.begin();
  //Serial.begin(38400);

  //Dale un nombre a cada Pad para que aparezca en el LCD
  //Si o si tiene que ser en el mismo orden que la declaracion de las entradas analogicas
  kick.settingName("KICK");
  snare.settingName("SNARE");
  hihat.settingName("HIHAT");
  hihatPedal.settingName("HIHAT PEDAL");
  ride.settingName("RIDE");
  tom1.settingName("TOM 1");
  tom2.settingName("TOM 2");
  ftom1.settingName("FLOOR TOM 1");
  //ftom2.settingName("FLOOR TOM 2");
  lcymbal.settingName("LEFT CYMBAL");
  rcymbal.settingName("RIGHT CYMBAL");

  //Load settings from EEPROM.
  //Si o si tiene que ser en el mismo orden que la declaracion de las entradas analogicas
  kick.loadMemory();
  snare.loadMemory();
  hihat.loadMemory();
  hihatPedal.loadMemory();
  ride.loadMemory();
  tom1.loadMemory();
  tom2.loadMemory();
  ftom1.loadMemory();
  //ftom2.loadMemory();
  lcymbal.loadMemory();
  rcymbal.loadMemory();

  //boot message
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("MP DRUM");
  lcd.setCursor(0, 1);
  lcd.print("ver 1.1");
}

void loop()
{

  /////////// 1. LCD & SETTING  /////////////

  bool buttonPush = button.GetPushState();
  bool editStart = button.GetEditState();
  bool editDone = button.GetEditdoneState();
  bool display = button.GetDisplayState();

  char *padName = button.GetPadName();
  char *item = button.GetSettingItem();
  int settingValue = button.GetSettingValue();

  button.readButtonState();

  kick.settingEnable();
  snare.settingEnable();
  hihat.settingEnable();
  hihatPedal.settingEnable();
  ride.settingEnable();
  tom1.settingEnable();
  tom2.settingEnable();
  ftom1.settingEnable();
  //ftom2.settingEnable();
  lcymbal.settingEnable();
  rcymbal.settingEnable();

  if (buttonPush == true)
  {
    lcd.clear();
    lcd.print(padName);
    lcd.setCursor(0, 1);
    lcd.print(item);
    lcd.setCursor(13, 1);
    lcd.print(settingValue);
  }

  if (editStart == true)
  {
    lcd.clear();
    lcd.print("Inicio Edicion");
    delay(500);
    lcd.clear();
    lcd.print(padName);
    lcd.setCursor(0, 1);
    lcd.print(item);
    lcd.setCursor(13, 1);
    lcd.print(settingValue);
  }

  if (editDone == true)
  {
    lcd.clear();
    lcd.print("Edicion Finalizada");
    delay(500);
    lcd.clear();
    lcd.print(padName);
    lcd.setCursor(0, 1);
    lcd.print(item);
    lcd.setCursor(13, 1);
    lcd.print(settingValue);
  }

  //mostrar el pad golpeado y el valor de velociti del midi
  if (display == true)
  {
    int velocity = button.GetVelocity();
    char *hitPad = button.GetHitPad();

    lcd.clear();
    lcd.print(hitPad);
    lcd.setCursor(0, 1);
    lcd.print(velocity);

    if (hihat.hit == true)
    {
      //pedal HH abiero
      if (hihatPedal.openHH == true)
      {
        lcd.setCursor(6, 0);
        lcd.print("OPEN");
      }
      //Pedal HH cerrado
      else if (hihatPedal.closeHH == true)
      {
        lcd.setCursor(6, 0);
        lcd.print("CLOSE");
      }
    }
  }

  ////////// 2. obtener valor & enviar MIDI////////////

  //sensar los valores de los pads
  kick.singlePiezo();
  snare.singlePiezo();
  hihat.HH();
  hihatPedal.hihatControl();
  ride.singlePiezo();
  tom1.singlePiezo();
  tom2.singlePiezo();
  ftom1.singlePiezo();
  //ftom2.singlePiezo();
  lcymbal.singlePiezo();
  rcymbal.singlePiezo();

  //mandar los valores de los pads a traves de MIDI
  //KICK//
  if (kick.hit == true)
  {
    MIDI.sendNoteOn(kick.note, kick.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(kick.note, 0, 10);
  }

  //SNARE//
  if (snare.hit == true)
  {
    MIDI.sendNoteOn(snare.note, snare.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(snare.note, 0, 10);
  }

  //HIHAT//
  if (hihat.hit == true)
  {
    //controlando apertura
    //1.abierto
    if (hihatPedal.openHH == true)
    {
      MIDI.sendNoteOn(hihat.noteOpen, hihat.velocity, 10); //(note of open, velocity, channel)
      MIDI.sendNoteOff(hihat.noteOpen, 0, 10);
    }
    //2.cerrado
    else if (hihatPedal.closeHH == true)
    {
      MIDI.sendNoteOn(hihat.noteClose, hihat.velocity, 10); //(note of close, velocity, channel)
      MIDI.sendNoteOff(hihat.noteClose, 0, 10);
    }
  }

  //HIHAT CONTROLLER//
  //esta cerrado?
  if (hihatPedal.hit == true)
  {
    MIDI.sendNoteOn(hihatPedal.note, hihatPedal.velocity, 10); //(note of pedal, velocity, channel)
    MIDI.sendNoteOff(hihatPedal.note, 0, 10);
  }

  //valor variable del pedal usando CC MIDI
  if (hihatPedal.moving == true)
  {
    MIDI.sendControlChange(4, hihatPedal.pedalCC, 10);
  }

  //RIDE//
  //1.bow
  if (ride.hit == true)
  {
    MIDI.sendNoteOn(ride.note, ride.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(ride.note, 0, 10);
  }
 
 //TOM 1//
  if (tom1.hit == true)
  {
    MIDI.sendNoteOn(tom1.note, tom1.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(tom1.note, 0, 10);
  }

   //TOM 2//
  if (tom2.hit == true)
  {
    MIDI.sendNoteOn(tom2.note, tom2.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(tom2.note, 0, 10);
  }

   //FLOORTOM 1//
  if (ftom1.hit == true)
  {
    MIDI.sendNoteOn(ftom1.note, ftom1.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(ftom1.note, 0, 10);
  }

 /*    //FLOORTOM 2//
if (ftom2.hit == true)
  {
    MIDI.sendNoteOn(ftom2.note, ftom2.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(ftom2.note, 0, 10);
  }
*/
     //LEFT CYMBAL//
  if (lcymbal.hit == true)
  {
    MIDI.sendNoteOn(lcymbal.note, lcymbal.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(lcymbal.note, 0, 10);
  }

      //RIGHT CYMBAL//
  if (rcymbal.hit == true)
  {
    MIDI.sendNoteOn(rcymbal.note, rcymbal.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(rcymbal.note, 0, 10);
  }
  
  //habilitar los valores del borde del ride (actualmente no esta bien hecho)
 /* //2.edge
  else if (ride.hitRim == true)
  {
    MIDI.sendNoteOn(ride.noteRim, ride.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(ride.noteRim, 0, 10);
  }

  //3.cup
  else if (ride.hitCup == true)
  {
    MIDI.sendNoteOn(ride.noteCup, ride.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(ride.noteCup, 0, 10);
  }
//habilitar parada del ride con la mano (actualmente no esta bien hecho)
  //4.choke
  if (ride.choke == true)
  {
    MIDI.sendPolyPressure(ride.note, 127, 10);
    MIDI.sendPolyPressure(ride.noteRim, 127, 10);
    MIDI.sendPolyPressure(ride.noteCup, 127, 10);
    MIDI.sendPolyPressure(ride.note, 0, 10);
    MIDI.sendPolyPressure(ride.noteRim, 0, 10);
    MIDI.sendPolyPressure(ride.noteCup, 0, 10);
  }*/
}
