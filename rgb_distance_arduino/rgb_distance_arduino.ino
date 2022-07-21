#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 1

#define DATA_PIN 9

// Define the array of leds
CRGB leds[NUM_LEDS];

#define echoPin1 6
#define trigPin1 7

#define echoPin2 5
#define trigPin2 4

#define echoPin3 2
#define trigPin3 3

int maxDistance = 60; // after this distance values are 0
int minDistance = 5;  // below this values are max

int rDistance = 100;
int gDistance = 100;
int bDistance = 100;

// converts distance to between 0 and 255
int clampDistance(int distance) {
  if (distance < minDistance) {
    return 255;
  }
  if (distance > maxDistance) {
    return 0;
  }

  return 255 - ((((distance - minDistance) * 255) / (maxDistance - minDistance)));
}

void setup() {
  // put your setup code here, to run once:

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical

  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin1, INPUT); // Sets the echoPin as an INPUT
  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin2, INPUT); // Sets the echoPin as an INPUT
  pinMode(trigPin3, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin3, INPUT); // Sets the echoPin as an INPUT

  Serial.begin(115200);



}

int getFilteredDistance(int triggerPin, int echoPin) {


  long startTime = millis();
  long allowedTimeMs = 1500;

  int receivedDistances[7];
  int distancesModeArray[61] = {0};

  int distanceModeCount = -1;

  int distanceModeValue = -1;
  int maxDistanceCount = 0;

  for (int count = 0; count < 6; count++) {

    // Clears the trigPin condition
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    long duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    int distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    // Displays the distance on the Serial Monitor

    receivedDistances[count] = distance;

    if (distance > maxDistance) {
      distance = maxDistance;
      maxDistanceCount++;
    }

    if (maxDistanceCount > 2) {
      return maxDistance;
    }

    distancesModeArray[distance]++;

    delay(5);

    if (millis() - startTime > allowedTimeMs) {
      Serial.println("EXCEEDED allowed time returning\n");
      return maxDistance;
    }

  }

  // get the mode value from the measurements, helps cut out random high values
  for (int count = 0; count < 61; count++) {
    if (distancesModeArray[count] > distanceModeCount) {
      distanceModeCount = distancesModeArray[count];
      distanceModeValue = count;
    }
  }

  return distanceModeValue;
}

void loop() {
  int distance = getFilteredDistance(trigPin1, echoPin1);

  Serial.print("Distance 1: ");
  Serial.print(distance);
  Serial.println(" cm");

  rDistance = clampDistance(distance);

  distance = getFilteredDistance(trigPin2, echoPin2);

  Serial.print("Distance 2: ");
  Serial.print(distance);
  Serial.println(" cm");

  gDistance = clampDistance(distance);

  distance = getFilteredDistance(trigPin3, echoPin3);

  Serial.print("Distance 3: ");
  Serial.print(distance);
  Serial.println(" cm");

  bDistance = clampDistance(distance);

  leds[0].red = rDistance;
  leds[0].green = gDistance;
  leds[0].blue = bDistance;

  FastLED.show();
  delay(5);

}
