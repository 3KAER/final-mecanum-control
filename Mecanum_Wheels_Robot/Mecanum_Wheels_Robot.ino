/*
 *- Các tập lệnh AT cho HC-06 ----------------------------|
 *    AT            kiểm tra kết nối                      |
 *    AT+NAME...    thay đổi tên                          |--> name: Car_HC-06
 *    AT+BAUD...    với 1 set to 1200 bps                 |
 *                      2 set to 2400   bps               |
 *                      3 set to 4800   bps               |
 *                      4 set to 9600   bps (mặc định)    |--> baud rate: 9600 bps
 *                      5 set to 19200  bps               |
 *                      6 set to 38400  bps               |
 *                      7 set to 57600  bps               |
 *                      8 set to 115200 bps               |
 *    AT+PIN...   cài đặt mã PIN (mặc định: 1234)         |--> pass: 1234
 *--------------------------------------------------------|
 *
 *- Các tín hiệu gửi liên tục ------------------------------------|
 *    S       Ko làm gì (stop)  |   . Xe đứng yên                 |
 *    F       Tiến              |   . (Chiều dọc) - Đi tới        |
 *    B       Lùi               |   . (Chiều dọc) - Đi lùi        |
 *    L       Trái              |   . (Chiều ngang) - Đi qua trái |
 *    R       Phải              |   . (Chiều ngang) - Đi qua phải |
 *  --------------------------- KO QUAY XE -----------------------|
 *    G       Tiến + Trái       | . Đi hướng Tây-Bắc              |
 *    I       Tiến + Phải       | . Đi hướng Đông-Bắc             |
 *    H       Lùi  + Trái       | . Đi hướng Tây-Nam              |
 *    J       Lùi  + Phải       | . Đi hướng Đông-Nam             |
 *----------------------------------------------------------------|
 *
 *- Các tín hiệu gửi 1 lần -|
 *    0     Tốc độ MIN      |
 *    1     ...             |
 *    2     ...             |
 *    3     ...             |
 *    4     ...             |
 *    5     ...             |
 *    6     ...             |
 *    7     ...             |
 *    8     ...             |
 *    9     ...             |
 *    q     Tốc độ MAX      |
 *  ------------------------|
 *    W       Bật đèn trước |
 *    w       Tắt đèn trước |
 *    U       Bật đèn sau   |
 *    u       Tắt đèn sau   |
 *    V       Bật còi       |
 *    v       Tắt còi       |
 *  ------------------------|
 *    X       Bật cảnh báo  |
 *    x       Tắt cảnh báo  |
 *  ------------------------|-------------------|
 *    D       Tắt hết tất cả (stop everything)  |
 *----------------------------------------------|
 */


#include "Pin_Connect.h"
#include <SoftwareSerial.h>
#include "PS2X_lib.h" 
#include "Control_Car_Bluetooth.h"




// --------------------------------------------------------------------------------------------------------------------------------- //

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you conect the controller, 
//or call config_gamepad(pins) again after connecting the controller.
int error = 0; 
byte type = 0;
byte vibrate = 0;
void setup()
{
  
  Serial.begin(115200);

  error = ps2x.config_gamepad(3,12,2,13, false, false);   //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  
  if(error == 0){
    Serial.println("Found Controller, configured successful");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Go to www.billporter.info for updates and to report bugs.");
  }
   
  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
   
  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
   
  type = ps2x.readType(); 
    
  switch(type) {
    case 0:
      Serial.println("Unknown Controller type");
      break;
    case 1:
      Serial.println("DualShock Controller Found");
      break;
    case 2:
      Serial.println("GuitarHero Controller Found");
      break;
    default:
      break;
     
  }



  /*
   *  Chiều kim đồng hồ         : Clockwise           | CW  : 1
   *  Ngược chiều kim đồng hồ   : Counter-Clockwise   | CCW : 0
   * 
   *  !!! Đây là chiều quay của các Pin dưới đây khi kích "HIGH"
   */

  pinMode(MOTOR_FR_IN1, OUTPUT);  // Front - Right    | + CW  : 1
  pinMode(MOTOR_FR_IN2, OUTPUT);  //                  | - CCW : 0
  pinMode(MOTOR_FL_IN3, OUTPUT);  // Front - Left     | + CW  : 1
  pinMode(MOTOR_FL_IN4, OUTPUT);  //                  | - CCW : 0

  pinMode(MOTOR_BR_IN1, OUTPUT);  // Behind - Right   | + CW  : 1
  pinMode(MOTOR_BR_IN2, OUTPUT);  //                  | - CCW : 0
  pinMode(MOTOR_BL_IN3, OUTPUT);  // Behing - Left    | + CW  : 1
  pinMode(MOTOR_BL_IN4, OUTPUT);  //                  | - CCW : 0
}

/*
 *  SƠ ĐỒ HƯỚNG LẮP BÁNH CỦA XE
 * 
 *              ^
 *      FL      |      FR
 *       \      |      /
 *        \-----|-----/
 *        |\         /|
 *        |           |
 *        |           |
 *        |     .     |
 *        |           |
 *        |           |
 *        |/         \|
 *        /-----------\
 *       /             \
 *      BL             BR
 */

// --------------------------------------------------------------------------------------------------------------------------------- //-------------------------------------------------------------------------------------------------------------- //

void loop()
{
  

 ps2x.read_gamepad(false, vibrate);          //read controller and set large motor to spin at 'vibrate' speed
    
    if(ps2x.Button(PSB_START))                   //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");
         
    if(ps2x.Button(PSB_PAD_UP)) {         //will be TRUE as long as button is pressed
      Serial.println("UP is being held");
       stop_car();
      go_forward();

    }
    if(ps2x.Button(PSB_PAD_RIGHT)){
      Serial.println("Right is being held");
      stop_car();
      go_right(); 
    }
    if(ps2x.Button(PSB_PAD_LEFT)){
      Serial.println("LEFT is being held");
      stop_car();
      go_left();
    }
    if(ps2x.Button(PSB_PAD_DOWN)){
         Serial.println("DOWN is being held");
          stop_car();
         back_forward(); 
    }
    
    if (ps2x.NewButtonState())               //will be TRUE if any button changes state (on to off, or off to on)
    {
        if(ps2x.Button(PSB_L3))
          Serial.println("L3 pressed");
        if(ps2x.Button(PSB_R3))
          Serial.println("R3 pressed");
        if(ps2x.Button(PSB_L2))
          Serial.println("L2 pressed");
        if(ps2x.Button(PSB_R2))
          Serial.println("R2 pressed");
    }   
         
    if(ps2x.ButtonPressed(PSB_RED))             //will be TRUE if button was JUST pressed
          Serial.println("Circle just pressed");

    if(ps2x.ButtonReleased(PSB_RED))             //will be TRUE if button was JUST released
          Serial.println("Circle just released"); 


    if(ps2x.ButtonPressed(PSB_PINK))             //will be TRUE if button was JUST pressed
          Serial.println("Square just pressed");
                  
    if(ps2x.ButtonReleased(PSB_PINK))             //will be TRUE if button was JUST released
          Serial.println("Square just released");     

    if(ps2x.ButtonPressed(PSB_GREEN))             //will be TRUE if button was JUST pressed
          Serial.println("Triangle just pressed");
                  
    if(ps2x.ButtonReleased(PSB_GREEN))             //will be TRUE if button was JUST released
          Serial.println("Triangle just released");   

    if(ps2x.ButtonPressed(PSB_BLUE)){           //will be TRUE if button was JUST pressed
          Serial.println("X just pressed")
          ;
          stop_car(); }

                  
    if(ps2x.ButtonReleased(PSB_BLUE)){             //will be TRUE if button was JUST released
         Serial.println("X just released");}


         
    //if(ps2x.NewButtonState(PSB_BLUE))            //will be TRUE if button was JUST pressed OR released
    //     Serial.println("X just changed");    
    
    if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) // print stick values if either is TRUE
    {
        Serial.print("Stick Values:");
        Serial.print(ps2x.Analog(PSS_LY), DEC); // LY
        Serial.print(",");
        Serial.print(ps2x.Analog(PSS_LX), DEC); // LX
        Serial.print(",");
        Serial.print(ps2x.Analog(PSS_RY), DEC); // RY
        Serial.print(",");
        Serial.println(ps2x.Analog(PSS_RX), DEC); // RX
    } 
    
 delay(50);}
 
// --------------------------------------------------------------------------------------------------------------------------------- //
