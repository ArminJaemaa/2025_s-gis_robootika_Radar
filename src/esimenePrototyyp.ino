#include <Servo.h> //võtame kasutusele Servo teegi
#define signal 5 //juhime servot selle viigu kaudu
#define	sensor_output 2  //potentsiomeetrid loeme selle viigu kaudu
#define	sensor_input 3  //potentsiomeetrid loeme selle viigu kaudu
#define helikiirus 0.0343 
#define max_distance 300 //cm
//cm mikrosekundis

long pikkus;
float kaugus;

Servo mootor; //defineerime objekti mootor, mis tähistab meie servot

void setup()
{
  pinMode(signal, OUTPUT);
  pinMode(sensor_output, OUTPUT);
  pinMode(sensor_input, INPUT);

  Serial.begin(9600);
  mootor.attach(signal); //ütleme objektile moootor, et ta vahetab andmeid selle viigu kaudu
}

void loop()
{
  for(int i=10; i<=170; i+=5){
    mootor.write(i);
    // trig viigu peal signaal madalaks
    digitalWrite(sensor_output, LOW);
    delayMicroseconds(2);
    // seame trig viigu peal signaal kõrgeks 10-ks mikrosekundiks
    // selle peale saadetakse välja ultraheli signaal ja
    // echo viigu väärtus seatakse HIGH olekusse, kuni sensor tuvastab
    // ultrahelisignaali tagasipeegeldumise või kuni möödud 38 millisekundit.
    // Mõlemal juhul läheb echo viigu väärtus LOW olekusse.
    digitalWrite(sensor_output, HIGH);
    delayMicroseconds(10);
    digitalWrite(sensor_output, LOW);
    // pulseIn() funktsioon mõõdab mitu mikrosekundit on echo viik olekus HIGH
    pikkus = pulseIn(sensor_input, HIGH);
    //arvutame kui pika maa pidi ultrahelisignaal läbima saatmisest
    //kuni kinnipüüdmiseni peale tagasipeegeldumist. 
    kaugus = pikkus * helikiirus;
    //nüüd on meil ultrahelisignaali poolt läbitud teekond sentimeetrites, 
    //aga see tuleb jagada kahega, sest signaal peegeldus tagasi e.
    //läbis teekonna takistuseni kaks korda.
    kaugus = kaugus/2;
    //Kirjutame saadud tulemuse välja
    if(kaugus<max_distance){
      Serial.print("Kaugus cm: ");
      Serial.println(kaugus);
    }
  }
  for(int i=170; i>=10; i-=5){
    mootor.write(i);
    // trig viigu peal signaal madalaks
    digitalWrite(sensor_output, LOW);
    delayMicroseconds(2);
    // seame trig viigu peal signaal kõrgeks 10-ks mikrosekundiks
    // selle peale saadetakse välja ultraheli signaal ja
    // echo viigu väärtus seatakse HIGH olekusse, kuni sensor tuvastab
    // ultrahelisignaali tagasipeegeldumise või kuni möödud 38 millisekundit.
    // Mõlemal juhul läheb echo viigu väärtus LOW olekusse.
    digitalWrite(sensor_output, HIGH);
    delayMicroseconds(10);
    digitalWrite(sensor_output, LOW);
    // pulseIn() funktsioon mõõdab mitu mikrosekundit on echo viik olekus HIGH
    pikkus = pulseIn(sensor_input, HIGH);
    //arvutame kui pika maa pidi ultrahelisignaal läbima saatmisest
    //kuni kinnipüüdmiseni peale tagasipeegeldumist. 
    kaugus = pikkus * helikiirus;
    //nüüd on meil ultrahelisignaali poolt läbitud teekond sentimeetrites, 
    //aga see tuleb jagada kahega, sest signaal peegeldus tagasi e.
    //läbis teekonna takistuseni kaks korda.
    kaugus = kaugus/2;
    //Kirjutame saadud tulemuse välja
    if(kaugus<max_distance){
      Serial.print("Kaugus cm: ");
      Serial.println(kaugus);
    }
  }
}