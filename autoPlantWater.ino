// we have calibrated the sensor taking analogRead 300 as wettest soil and 1000 as dryest soil

#include <LiquidCrystal_I2C.h>
#define sensorPin A0 // analog pin of the soil sensor is initialised to A0
#define trigPin 6
#define echoPin 7
#define relayPin 10
#define buzzerPin 11
LiquidCrystal_I2C lcd(0x27, 16, 2);
int moistureLevel; // the analog resistor output representing the moisture level
int moisturePercent;
int reservoirWaterLevel;
int reservoirWaterPercent;
void setup()
{
    pinMode(sensorPin, INPUT); // A0 of arduino is initialised as output
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(relayPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(relayPin, LOW);
    Serial.begin(9600);
    welcomeLCD();
}
void loop()
{
    moistureLevel = moistureLevelFinder(sensorPin);
    moisturePercent = moisturePercentFinder(moistureLevel);
    humidityLCD(moisturePercent);
    pumpWaterControl(moisturePercent);
    wateringStatus(moisturePercent);
    reservoirWaterLevel = reservoirLevel();
    /* NOTE temporary code to find the hieght of the reservoir.
    lcd.clear();
    lcd.print(reservoirWaterLevel);
    delay(3000);
    */

    reservoirWaterPercent = reservoirLevelPercent(reservoirWaterLevel);
    reservoirPercentLCD(reservoirWaterPercent);
    reservoirStatusLCD(reservoirWaterPercent);
    buzzerControl(reservoirWaterPercent);

    delay(2000);
}

// NOTE below we implement the individual function required for the program

//function for calculating the humidity level of the soil
int moistureLevelFinder(int analogPin)
{ // input pin having analog input
    moistureLevel = analogRead(analogPin);
    return moistureLevel;
}
/* function for calculating the humidity percentage by taking 300 as the lower mark for wetness and
 taking 1000 as the upper mark for the dryness.
 */
int moisturePercentFinder(int moistureLevel)
{
    int moisturePercent = (1000 - moistureLevel) * (0.14); //  0.14 come by dividing 100 by (1000-300)
    if (moisturePercent > 100)
    {
        moisturePercent = 100;
    }
    else if (moisturePercent < 0)
    {
        moisturePercent = 0;
    }
    return moisturePercent;
}

void welcomeLCD()
{
    lcd.init();
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("WELCOME TO AUTO ");
    lcd.setCursor(0, 1);
    lcd.print("PLANT WATERING !");
    delay(3000);
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("MADE BY");
    lcd.setCursor(1, 1);
    lcd.print("VIBHOR AGARWAL");

    delay(4000);
}
void humidityLCD(int moisturePercent)
{
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("SOIL HUMIDITY");
    lcd.setCursor(5, 1);
    lcd.print("IS ");
    lcd.print(moisturePercent);
    lcd.print("%");
    delay(3000);
}

/* NOTE if moisturePercent > 80 - soil is too wet and no need to water
if moisturePercent<40 - soil is too dry, turning the pump on
if 40 < moisturePercent < 80 then soil humidity OK no need to water.
*/

void wateringStatus(int moisturePercent)
{
    if (moisturePercent <= 30)
    {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("SOIL TOO DRY");
        lcd.setCursor(0, 1);
        lcd.print("PUMP SWITCHED ON");
    }
    else if (moisturePercent > 30 & moisturePercent < 80)
    {
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("SOIL GOOD");
        lcd.setCursor(1, 1);
        lcd.print("PUMP NO CHANGE");
    }
    else
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SOIL TOO WET");
        lcd.setCursor(0, 1);
        lcd.print("PUMP SWITCHED OF");
    }
    delay(3000);
}
// calculate the reservoir water height to find the reservoir water percentage
float reservoirLevel()
{
    long bounceTime;
    float distanceCM;
    float distanceInch;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(20);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(20);
    digitalWrite(trigPin, LOW);
    bounceTime = pulseIn(echoPin, HIGH);
    distanceCM = bounceTime / 58.2;
    return distanceCM;
}
// NOTE  taking 15 CM as level when bucket full and 45 cm when bucket empty
// FIXME there might be some bugs here.
int reservoirLevelPercent(int reservoirLevel)
{
    int reservoirPercent;
    if (reservoirLevel <= 15)
    {
        reservoirPercent = 100;
    }
    else if (reservoirLevel >= 45)
    {
        reservoirPercent = 0;
    }
    else
    {
        reservoirPercent = (45 - reservoirLevel) * 3.3;
    }
    return reservoirPercent;
}

void reservoirPercentLCD(int reservoirLevelPercent)
{
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("BUCKET WATER");
    lcd.setCursor(2, 1);
    lcd.print("LEVEL IS ");
    lcd.print(reservoirLevelPercent);
    lcd.print(" %");
    delay(3000);
}

// TODO implement a solution for auto reservoir filling. Currently Manual
void reservoirStatusLCD(int reservoirLevelPercent)
{
    if (reservoirLevelPercent <= 30)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("BUCKET WATER LOW");
        lcd.setCursor(2, 1);
        lcd.print("KINDLY REFILL");
    }
    else if (reservoirLevelPercent > 30 & reservoirLevelPercent < 80)
    {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("BUCKET WATER OK");
        lcd.setCursor(0, 1);
        lcd.print("REFILL NO CHANGE");
    }
    else
    {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("BUCKET FULL");
        lcd.setCursor(2, 1);
        lcd.print("STOP REFILL");
    }
    delay(3000);
}

/* NOTE below function turns on the pump when the moisture percent falls below 30%
and it turns off the pump once the moisture level reaches 80%. */

void pumpWaterControl(int moisturePercent)
{
    if (moisturePercent <= 30)
    {
        digitalWrite(relayPin, HIGH);
    }
    else if (moisturePercent >= 80)
    {
        digitalWrite(relayPin, LOW);
    }
}
void buzzerControl(int reservoirLevelPercent)
{
    if (reservoirLevelPercent <= 30)
    {
        digitalWrite(buzzerPin, HIGH);
    }
    else if (reservoirLevelPercent > 30 & reservoirLevelPercent < 80)
    {
        digitalWrite(buzzerPin, LOW);
    }
    delay(1000);
}
