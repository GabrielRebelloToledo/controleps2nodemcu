/**
 * ESP-NOW
 * 
 * Sender
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include <PS2X_lib.h>  //for v1.6

#define PS2_DAT 2   //14 2
#define PS2_CMD 12  //15 12
#define PS2_SEL 0   //16 0
#define PS2_CLK 14  //17 14

#define buzzer 4

//#define pressures   true
#define pressures false
//#define rumble      true
#define rumble false

PS2X ps2x;  // create PS2 Controller Class
// Mac address of the slave
uint8_t peer1[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
typedef struct message {
  boolean Botao_0;
  boolean Botao_1;
  boolean Botao_2;
  boolean Botao_3;
  boolean Botao_4;
  boolean Botao_5;
  boolean Botao_6;
  boolean Botao_7;
  boolean Botao_JoyA;
  boolean Botao_JoyB;
  byte Joystick_AX;
  byte Joystick_AY;
  byte Joystick_BX;
  byte Joystick_BY;
  byte Pot_A;
  byte Pot_B;
};

int error = 0;
byte type = 0;
byte vibrate = 0;

struct message myMessage;
void onSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.println("Status:");
  Serial.println(sendStatus);
  
}
void setup() {
  Serial.begin(115200);
  delay(300);

  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  if (error == 0) {
    Serial.println("Found Controller, configured successful");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Go to www.billporter.info for updates and to report bugs.");
  }

  else if (error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

  else if (error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if (error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

  //Serial.print(ps2x.Analog(1), HEX);

  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.println("Unknown Controller type");
      break;
    case 1:
      Serial.println("DualShock Controller Found");
      // Sounds the buzzer at the frequency relative to the note C in Hz
      tone(buzzer, 261);
      // Waits some time to turn off
      delay(200);
      //Turns the buzzer off
      noTone(buzzer);
      // Sounds the buzzer at the frequency relative to the note D in Hz
      tone(buzzer, 293);
      delay(200);
      noTone(buzzer);
      // Sounds the buzzer at the frequency relative to the note E in Hz
      tone(buzzer, 329);
      delay(200);
      noTone(buzzer);
      // Sounds the buzzer at the frequency relative to the note F in Hz
      tone(buzzer, 349);
      delay(200);
      noTone(buzzer);
      // Sounds the buzzer at the frequency relative to the note G in Hz
      tone(buzzer, 392);
      delay(200);
      noTone(buzzer);
      break;
    case 2:
      Serial.println("GuitarHero Controller Found");
      break;
  }


  WiFi.mode(WIFI_STA);
  // Get Mac Add
  Serial.print("Mac Address: ");
  Serial.print(WiFi.macAddress());
  Serial.println("ESP-Now Sender");
  // Initializing the ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Problem during ESP-NOW init");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  // Register the peer
  Serial.println("Registering a peer");
  esp_now_add_peer(peer1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  Serial.println("Registering send callback function");
  esp_now_register_send_cb(onSent);
}
void loop() {
  if (error == 1)  //skip loop if no controller found
    return;

  if (type == 2) {  //Guitar Hero Controller

    ps2x.read_gamepad();  //read controller

    if (ps2x.ButtonPressed(GREEN_FRET))
      Serial.println("Green Fret Pressed");
    if (ps2x.ButtonPressed(RED_FRET))
      Serial.println("Red Fret Pressed");
    if (ps2x.ButtonPressed(YELLOW_FRET))
      Serial.println("Yellow Fret Pressed");
    if (ps2x.ButtonPressed(BLUE_FRET))
      Serial.println("Blue Fret Pressed");
    if (ps2x.ButtonPressed(ORANGE_FRET))
      Serial.println("Orange Fret Pressed");


    if (ps2x.ButtonPressed(STAR_POWER))
      Serial.println("Star Power Command");

    if (ps2x.Button(UP_STRUM))  //will be TRUE as long as button is pressed
      Serial.println("Up Strum");
    if (ps2x.Button(DOWN_STRUM))
      Serial.println("DOWN Strum");


    if (ps2x.Button(PSB_START))  //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if (ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");


    if (ps2x.Button(ORANGE_FRET))  // print stick value IF TRUE
    {
      Serial.print("Wammy Bar Position:");
      Serial.println(ps2x.Analog(WHAMMY_BAR), DEC);
    }
  }

  else {  //DualShock Controller

    ps2x.read_gamepad(false, vibrate);  //read controller and set large motor to spin at 'vibrate' speed

    if (ps2x.Button(PSB_START))  //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if (ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");


    if (ps2x.Button(PSB_PAD_UP)) {  //will be TRUE as long as button is pressed
      Serial.print("Up held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
    }
    if (ps2x.Button(PSB_PAD_RIGHT)) {
      Serial.print("Right held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
    }
    if (ps2x.Button(PSB_PAD_LEFT)) {
      Serial.print("LEFT held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
    }
    if (ps2x.Button(PSB_PAD_DOWN)) {
      Serial.print("DOWN held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
    }


    vibrate = ps2x.Analog(PSAB_BLUE);  //this will set the large motor vibrate speed based on
                                       //how hard you press the blue (X) button

    if (ps2x.NewButtonState())  //will be TRUE if any button changes state (on to off, or off to on)
    {
      if (ps2x.Button(PSB_L3))
        Serial.println("L3 pressed");
      if (ps2x.Button(PSB_R3))
        Serial.println("R3 pressed");
      if (ps2x.Button(PSB_L2))
        Serial.println("L2 pressed");
      if (ps2x.Button(PSB_R2))
        Serial.println("R2 pressed");
      if (ps2x.Button(PSB_GREEN))
        Serial.println("Triangle pressed");
    }


    if (ps2x.ButtonPressed(PSB_RED))  //will be TRUE if button was JUST pressed
      Serial.println("Circle just pressed");

    if (ps2x.ButtonReleased(PSB_PINK))  //will be TRUE if button was JUST released
      Serial.println("Square just released");

    if (ps2x.NewButtonState(PSB_BLUE))  //will be TRUE if button was JUST pressed OR released
      Serial.println("X just changed");



    /* Serial.print("Stick Values:"); */
    myMessage.Joystick_AX = (ps2x.Analog(PSS_LY));  //Left stick, Y axis. Other options: LX, RY, RX
    /* Serial.print(","); */
    myMessage.Joystick_AY = (ps2x.Analog(PSS_LX));
    /* Serial.print(","); */
    myMessage.Joystick_BX = (ps2x.Analog(PSS_RY));
    /* Serial.print(","); */
    myMessage.Joystick_BY = (ps2x.Analog(PSS_RX));

    Serial.print("Stick Values:");
    Serial.print(ps2x.Analog(PSS_LY), DEC);  //Left stick, Y axis. Other options: LX, RY, RX
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_LX), DEC);
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_RY), DEC);
    Serial.print(",");
    Serial.println(ps2x.Analog(PSS_RX), DEC);
  }




  Serial.println("Send a new message");
  esp_now_send(NULL, (uint8_t *)&myMessage, sizeof(myMessage));
  delay(200);
}