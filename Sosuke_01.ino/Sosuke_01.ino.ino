//************************************************************
//************************ Include ***************************
//************************************************************

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
//test 02


//************************************************************
//************************ Define Variables ******************
//************************************************************
Adafruit_MPU6050 mpu;


//pins
  #define LED_PIN 1 // (Arduino is 13, Teensy is 11, Teensy++ is 6)

//NeoPixel 7
  #define NUMPIXELS 14
  #define PIXEL_DELAYVAL 100 // Time (in milliseconds) to pause between pixels
  Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRBW + NEO_KHZ800);


//TossUp variables

  bool FreeFall;  
  
  int t_now;
  int t_last;
  int t_cought;  
  
  float vSumAcc;
  float g_const = 9.80665;

  float Vel;
  float Vel_last;
  
  float ff_thold = 3;

  float low_toss_Vthresh = 8;
  float med_toss_Vthresh = 10;
  float high_toss_Vthresh = 12;
  

  bool in_hand;
  bool in_air;
  bool recently_in_air;
  bool recently_in_hand;




//************************************************************
//************************ Setup  ****************************
//************************************************************

void setup(void) {
  // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.begin(); 

  // configure LED for output
    pinMode(LED_PIN, OUTPUT);
    
  // initialize serial communication
    Serial.begin(115200);
    pixels.clear(); // Set all pixel colors to 'off'


  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  // set accelerometer range to +-8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  // set gyro range to +- 500 deg/s
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  // set filter bandwidth to 21 Hz
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);
}

//************************************************************
//************************ Main Loop  ************************
//************************************************************

void loop() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* calculate the vector sum of Acceleration */
  vSumAcc =(sqrt(sq(a.acceleration.x)+sq(a.acceleration.y)+sq(a.acceleration.z)));


//if you are in the air this will execute
  if (vSumAcc < ff_thold){                      
  //Serial.print("Weeeeeee!!!! \t");
     in_air = true;

//if you are in the air and were just tossed then do this:
     if (recently_in_hand){                          
     //Serial.println("Just tossed");
      recently_in_hand = false;
      Vel_last = 0;

//Set the color of the ball based on how hard you threw it
      if (Vel > high_toss_Vthresh){
          for(int i=0; i<NUMPIXELS; i++) { 
            pixels.setPixelColor(i, pixels.Color(0, 0, 0, 100)); 
          }
      }
       else if(Vel >= med_toss_Vthresh){
          for(int i=0; i<NUMPIXELS; i++) { 
            pixels.setPixelColor(i, pixels.Color(0, 0, 100, 0)); 
          }
       }
       else if(Vel >= low_toss_Vthresh){
          for(int i=0; i<NUMPIXELS; i++) { 
            pixels.setPixelColor(i, pixels.Color(0, 100, 0, 0)); 
          }
       }
       else if(Vel < low_toss_Vthresh){
          for(int i=0; i<NUMPIXELS; i++) { 
            pixels.setPixelColor(i, pixels.Color(100, 0, 0, 0)); 
          }
       }
     }
     recently_in_air = true;
 
  }


//if you are in the hand this code will execute  
  if (vSumAcc >= ff_thold){                      
  //Serial.print("Awwwww \t");
    in_hand = true;

//if you in the hand and were just cought then do this
    if (recently_in_air) {
//      Serial.println("Just cought");
      recently_in_air = false;
      t_cought = micros();
      t_last = t_cought;
      for(int i=0; i<NUMPIXELS; i++) { 
        pixels.setPixelColor(i, pixels.Color(0, 0, 0, 0)); 
      }
    }
    recently_in_hand = true;
    t_now = micros();
    Vel = Vel_last + vSumAcc * (t_now - t_last)/1000000;
    Vel_last = Vel;
    t_last = t_now;
    if ((t_now - t_cought) > 2000000){
      Vel_last = 0;
      pixels.clear();
      //Serial.print("boring! \t");
    }
  } 
  
  pixels.show(); // Set all pixel colors to 'show'
  Serial.println(Vel);
} 
 


 
  /*
    Serial.print(t_now);
    Serial.print("\t");
    Serial.print(t_cought);
    Serial.print("\t");
    Serial.print(t_now-t_cought);
    Serial.print("\t");
    Serial.println(Vel);
  Serial.print(micros());
  Serial.print("\t");
  Serial.print(FreeFall);
  Serial.print("\t");
  Serial.println(vSumAcc);
  pixels.show();   // Send the updated pixel colors to the hardware.

  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.print(a.acceleration.z);
  Serial.println(", ");
  Serial.print(g.gyro.x);
  Serial.print(",");
  Serial.print(g.gyro.y);
  Serial.print(",");
  Serial.print(g.gyro.z);
  Serial.println("");
*/
 
    
