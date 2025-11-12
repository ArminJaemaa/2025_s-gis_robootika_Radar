/* ============================================================
   ULTRASONIC RADAR DASHBOARD (Processing)
   ------------------------------------------------------------
   Reads data from Arduino in the format "angle,distance"
   Example input:  45,123
   Then visualizes it as a circular radar sweep.
   ============================================================ */

import processing.serial.*;  // library to read serial data
Serial myPort;               // the serial port object

// ---------- Visualization parameters ----------
float sweepAngle = 0;                // current radar sweep angle
ArrayList<PVector> points;           // stores radar dots (angle, distance)
int radarRadius = 300;               // radius of radar area in pixels
int maxDistance = 150;               // max sensor range in cm (match Arduino)
PFont font;                          // for text display

// ---------- SETUP runs once ----------
void setup() {
  size(800, 800);          // window size
  background(0);            // black background
  smooth();

  points = new ArrayList<PVector>();

  // Print available serial ports to console so you can pick the right one
  println(Serial.list());

  // Replace [0] with your actual port number if needed.
  // Example: Serial.list()[1] if your Arduino is the second one listed.
  myPort = new Serial(this, Serial.list()[0], 9600);
  myPort.bufferUntil('\n');   // process one line at a time

  font = createFont("Consolas", 16);
  textFont(font);
}

// ---------- DRAW runs repeatedly (~60 times/second) ----------
void draw() {
  background(0);           // clear screen every frame

  // Draw radar background (circles + grid)
  drawRadarGrid();

  // Draw detected objects
  drawPoints();

  // Draw sweep line (rotating)
  drawSweep();
}

// ---------- Handle serial input from Arduino ----------
void serialEvent(Serial p) {
  // Read one line until newline character
  String line = trim(p.readStringUntil('\n'));

  // Make sure we actually got data
  if (line == null || line.length() < 3) return;

  // Expect "angle,distance"
  String[] parts = split(line, ',');
  if (parts.length != 2) return;

  try {
    float angle = float(parts[0]) - 90;
    float dist  = float(parts[1]);

    if (dist < 0 || dist > maxDistance) return; // ignore invalid

    sweepAngle = angle; // current servo position
    
    for (int i = 0; i<points.size()-1; i++){
      if (points.get(i).x == sweepAngle){
        points.remove(i);
      }
    }
    // Convert cm to pixels for plotting
    float radius = map(dist, 0, maxDistance, 0, radarRadius);

    // Store new reading (angle, radius)
    points.add(new PVector(angle, radius));

    // Keep the list short (oldest points fade out)
    //if (points.size() > 300) points.remove(0);
  } catch (Exception e) {
    println("Parsing error: " + e);
  }
}

// ---------- Draw concentric circles and radial lines ----------
void drawRadarGrid() {
  pushMatrix();
  translate(width/2, height/2); // move origin to center
  stroke(0, 255, 0, 50);        // faint green
  noFill();

  // Distance rings every 50px
  for (int r = 50; r <= radarRadius; r += 50) {
    //circle(0, 0, r * 2);
    arc(0, 0, r*2, r*2, -170*PI/180, -10*PI/180);
  }

  // Angle lines every 30 degrees
  for (int a = -80; a < 90; a += 20) {
    float rad = radians(a - 90);
    line(0, 0, radarRadius * cos(rad), radarRadius * sin(rad));
  }

  popMatrix();
}

// ---------- Draw the sweeping radar line ----------
void drawSweep() {
  pushMatrix();
  translate(width/2, height/2);
  float rad = radians(sweepAngle - 90);
  stroke(0, 255, 0);          // bright green
  line(0, 0, radarRadius * cos(rad), radarRadius * sin(rad));
  popMatrix();
}

// ---------- Draw the detected dots ----------
void drawPoints() {
  pushMatrix();
  translate(width/2, height/2);
  strokeWeight(4);

  // Draw older points first, so new ones appear brighter
  for (int i = 0; i < points.size(); i++) {
    PVector p = points.get(i);
    float ageFactor = map(i, 0, points.size(), 50, 255); // fading trail
    stroke(0, 255, 0, ageFactor);
    float rad = radians(p.x - 90);
    float r = p.y;
    point(r * cos(rad), r * sin(rad));
  }

  popMatrix();
}
