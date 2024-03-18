#include "arduinoFFT.h"

#define SAMPLES 128            //Must be a power of 2
#define SAMPLING_FREQUENCY 8000 //Hz, must be less than 10000 due to ADC

#define AIN1 8
#define AIN2 9
#define PWMA 3
#define BIN1 10 
#define BIN2 11
#define PWMB 5
#define STBY 12

arduinoFFT FFT = arduinoFFT();

unsigned int sampling_period_us;
unsigned long microseconds;

double vReal[SAMPLES];
double vImag[SAMPLES];


void setup() {
   Serial.begin(115200);

   sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));

   pinMode( AIN1, OUTPUT );
   pinMode( AIN2, OUTPUT );
   pinMode( BIN1, OUTPUT );
   pinMode( BIN2, OUTPUT );
   //pinMode( STBY, OUTPUT );
}

void loop() {

   /*SAMPLING*/
   for(int i=0; i<SAMPLES; i++)
   {
       microseconds = micros();    //Overflows after around 70 minutes!

       vReal[i] = analogRead(A0);  //ESP8266の場合は「A0」。普通のArduinoは「0」。
       vImag[i] = 0;

       while(micros() < (microseconds + sampling_period_us)){
       }
   }

   /*FFT*/
   FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
   FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
   FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
   double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);

    if(peak<2880 && peak>2800){//2850 ミ
      digitalWrite( AIN1, HIGH );//左折　RIGHT  OK B=RIGHT , A=LEFT A is STRONG
      digitalWrite( BIN1, HIGH );
      digitalWrite( AIN2, LOW );
      digitalWrite( BIN2, LOW );
      //delay(100);
      analogWrite( PWMA, 255*0.5 );
      analogWrite( PWMB, 255*0.4 );
    }

    else if(peak<3300 && peak>3100){//2950
      digitalWrite( BIN1, HIGH );//右折　LEFT
      digitalWrite( AIN1, HIGH );
      digitalWrite( AIN2, LOW );
      digitalWrite( BIN2, LOW );
      //delay(100);
      analogWrite( PWMA, 255*0.35 );
      analogWrite( PWMB, 255*0.65 );
    }
    
    else if(peak<3100 && peak>2950){//3030
      digitalWrite( AIN1, HIGH );//STRAIGHT
      digitalWrite( BIN1, HIGH );
      digitalWrite( AIN2, LOW );
      digitalWrite( BIN2, LOW );
      analogWrite( PWMA, 255*0.825 );//X 0.9 0.8
      analogWrite( PWMB, 255 );
      //delay(100);
    }
    
    else if(peak<3400 && peak>3300){//3100
      digitalWrite( AIN1, LOW );//BACK
      digitalWrite( BIN1, LOW );
      digitalWrite( AIN2, HIGH );//STRAIGHT BACK
      digitalWrite( BIN2, HIGH );
      analogWrite( PWMA, 255*0.925 *0.5);
      analogWrite( PWMB, 255 *0.5);
      //delay(100);
    }
    else{
      digitalWrite( AIN1, LOW );
      digitalWrite( BIN1, LOW );
      digitalWrite( AIN2, LOW );
      digitalWrite( BIN2, LOW );
    }
    
   /*PRINT RESULTS*/
   Serial.println(peak);     //Print out what frequency is the most dominant.

   for(int i=2; i<(SAMPLES/2); i++)
   {
       /*View all these three lines in serial terminal to see which frequencies has which amplitudes*/

       //Serial.print((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 1);
       //Serial.print(" ");
       //Serial.println(vReal[i], 1);    //View only this line in serial plotter to visualize the bins
   }

   delay(1);  //Repeat the process every second OR:
   //while(1);       //ESPだと無限ループがエラーになるので注意

}
