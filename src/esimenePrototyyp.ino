#include <Servo.h>

#define SIGNAL_PIN 5
#define TRIG_PIN 2
#define ECHO_PIN 3
#define HELIKIIRUS 0.0343  // cm per microsecond
#define MAX_DISTANCE 300   // cm
#define MOOTMISTE_ARV 10   // number of samples per position

Servo mootor;

long pikkus;
float kaugused[MOOTMISTE_ARV];
float kaugus;

// --- helper to get one distance reading ---
float loeKaugus() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  pikkus = pulseIn(ECHO_PIN, HIGH, 30000UL); // timeout ~ 5 m
  if (pikkus == 0) return -1; // no echo

  kaugus = (pikkus * HELIKIIRUS) / 2.0;
  if (kaugus > MAX_DISTANCE || kaugus < 2) return -1;
  return kaugus;
}

// --- median filter ---
float medianFilter(float arr[], int n) {
  // simple bubble sort (ok for n=10)
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        float temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
  if (n % 2 == 0)
    return (arr[n/2 - 1] + arr[n/2]) / 2.0;
  else
    return arr[n/2];
}

void setup() {
  pinMode(SIGNAL_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  mootor.attach(SIGNAL_PIN);
  Serial.begin(9600);
}

// --- measure with filtering at angle i ---
void mooda(int nurk) {
  mootor.write(nurk);
  delay(100); // give servo time to move
  
  int valid = 0;
  for (int n = 0; n < MOOTMISTE_ARV; n++) {
    float k = loeKaugus();
    if (k > 0) {
      kaugused[valid++] = k;
    }
    delay(30);
  }

  if (valid > 2) {
    float med = medianFilter(kaugused, valid);
    // Optional: compute trimmed mean for extra smoothing
    float sum = 0;
    int count = 0;
    for (int j = 0; j < valid; j++) {
      if (abs(kaugused[j] - med) < 0.2 * med) { // keep within ±20% of median
        sum += kaugused[j];
        count++;
      }
    }
    float avg = (count > 0) ? sum / count : med;

    // --- Send clean serial output for dashboard ---
    Serial.print(nurk);
    Serial.print(",");
    Serial.println(avg);
  } else {
    Serial.print(nurk);
    Serial.print(",");
    Serial.println(0);
  }
}

void loop() {
  // Sweep right
  for (int i = 10; i <= 170; i += 5) {
    mooda(i);
  }
  // Sweep left
  for (int i = 170; i >= 10; i -= 5) {
    mooda(i);
  }
}