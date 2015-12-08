// This #include statement was automatically added by the Particle IDE.
#include "InternetButton/InternetButton.h"

#include "InternetButton/InternetButton.h"
#include "math.h"

/* Let me show you how easy it is to put the Button on the Internet.
Useful info, like how to access the data from your browser, can be
found here: http://docs.particle.io/photon/firmware/#spark-function
The code to control the number of illuminated LEDs is here:
https://github.com/spark/InternetButton/blob/master/controlKnob.html
Try naming one of your devices "InternetButton" and running controlKnob in your browser or on your phone!
Note that the Core or Photon *must* be named "InternetButton" because the javascript looks for it.
*/

InternetButton b = InternetButton();
float brightness = 0.2;
int red, green, blue;
int howMany = 6;
int whichColor = 100;
bool changed = false;
bool rainbow_mode = false;
int rainbow = 1;


int prevLed = 0; 
int count = 0; 
int xPrev = 0; 
int yPrev = 0; 
int zPrev = 0; 
//boolean rainbowMode = true; 

bool tiltMode = false; 
bool knobMode = false;

char publishString[40];
char publishString2[40];



void setup() {
    // Use b.begin(1); if you have the original SparkButton, which does not have a buzzer or a plastic enclosure
    // to use, just add a '1' between the parentheses in the code below.
    b.begin();

    //This is all you need to make the function controller() available to the internet
    //The API name and the local name don't need to be the same; just my style
    Particle.function("controller", controller);
  //  Particle.variable("led", &prevLed, INT);
    
  //  Particle.variable("rainbow", &rainbow, INT);

    //This function figures out what combination color, brightness and LEDs to display
    makeColors();

}



void loop(){
    
     // SHAKING -> RAINBOW MODE
//if(!changed) {
    int xValue = b.readX();
    int yValue = b.readY();
    int zValue = b.readZ();

    if(!rainbow_mode){

        if(abs(xValue-xPrev)>100 || abs(yValue-yPrev)>100 || abs(zValue-zPrev)>100) {
            rainbow = 1; 
            sprintf(publishString2,"%u",rainbow);
            Particle.publish("rainbow", publishString2);
            b.rainbow(10);
            rainbow_mode = true; 
        }
    }else {
        rainbow_mode = false; 
        rainbow = 0; 
    }
    
    xPrev = xValue; 
    yPrev = yValue; 
    zPrev = zValue; 

    //END SHAKING RAINBOW MODE
    
    
     //tilt button bool
    if(b.buttonOn(1)) {
        if(tiltMode) {
            tiltMode = false;
        }
        else {
            tiltMode = true; 
        }
    }
    
    
    
    //LIGHT UP LOWEST LED
    int ledPos = -1;
    if(tiltMode)
        ledPos = b.lowestLed(); 

    // Now turn that LED on
   if(prevLed != ledPos && tiltMode) {
        b.allLedsOff();
  
        b.ledOn(ledPos, 0, 30, 30);
        b.ledOn((ledPos+1)%12, 0, 100, 30);
        b.ledOn((ledPos-1)%12, 0, 30, 100);
        
        // turn on lowest led and everything before that until 0. 
        //b.ledOn(ledPos, 0, 30, 30);
        sprintf(publishString,"%u",ledPos);
        
         Particle.publish("led", publishString);
        // for(int i = ledPos; i >= 0; i--) {
        //     b.ledOn((ledPos-i)%12, 0, 30, 30);
        // }
        
    } else {
        //b.allLedsOff();
        prevLed = ledPos;
    }
    
    prevLed = ledPos; 

    // Wait a mo'
    delay(200);
    
    
    
    
    
    //Clicking "up" makes the LEDs brighter
    if(b.buttonOn(1)){
        if(brightness < 1){
            brightness += 0.005;
            changed = true;
        }
    }
    //Clicking "down" makes the LEDs dimmer
    else if (b.buttonOn(3)){
        /*if(brightness > 0){
            brightness -= 0.005;
            if(brightness < 0){
                brightness = 0;
            }
            changed = true;
        }*/
    }
    //Clicking "right" and "left" change the color
   /* else if (b.buttonOn(2)){
        if(whichColor < 255){
            whichColor += 1;
            changed = true;
        }
    }*/
    
   else  if (b.buttonOn(2)) {
        b.rainbow(3);
        // Publish the event "button4" for other services like IFTTT to use
        Spark.publish("button4",NULL, 60, PRIVATE);
    }
    
    else if (b.buttonOn(4)){
        if(whichColor > 0){
            whichColor -= 1;
            changed = true;
        }
    }
    

    //If anything's been altered by clicking or the Spark.function, update the LEDs
    if(changed){
        delay(1);
        makeColors();
        changed = false;
    }
}

//Uses the brightness and the color values to compute what to show
void makeColors(){
  uint8_t x = whichColor;
  if(x < 85) {
    red = brightness * x * 3;
    green = brightness * (255 - x * 3);
    blue = 0;
  } else if(x < 170) {
     x -= 85;
     red = brightness * (255 - x * 3);
     green = 0;
     blue = brightness * x * 3;
  } else {
     x -= 170;
     red = 0;
     green = brightness * x * 3;
     blue = brightness * (255 - x * 3);
  }

  b.allLedsOff();
  for(int i = 1; i <= howMany; i++){
    b.ledOn(i, red, green, blue);
  }
}

/*
controller() is the local function that is executed when the API function "controller" is called.
It changes how many LEDs on the Button are illuminated.
*/
int controller(String command){
    //parse the string into an integer
    int state = atoi(command.c_str());

    //Check that the value it's been given is in the right range
    if (state > 11) {state = 11;}
    else if (state < 0) {state = 0;}

    howMany = state;

    changed = true;

    return 1;
}