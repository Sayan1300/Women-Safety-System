#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

SoftwareSerial Gsm(3, 2);       // GSM RX, TX
SoftwareSerial gpsSerial(8, 9); // GPS TX, RX

TinyGPSPlus gps;

char phone_no[] = "+91xxxxxxxxxx";

int soundSensor = 4;
int sensorState;

bool isSMSsent = false;

void setup()
{
  Serial.begin(9600);

  gpsSerial.begin(9600);
  Gsm.begin(9600);

  Serial.println("System Started...");

  pinMode(soundSensor, INPUT_PULLUP);

  // GSM SMS mode
  Gsm.listen();
  Gsm.println("AT+CMGF=1");
  delay(1000);
}

void loop()
{
  // ---------------- GPS READING ----------------
  gpsSerial.listen();

  while (gpsSerial.available())
  {
    gps.encode(gpsSerial.read());
  }

  // Debug GPS
  if (gps.location.isValid())
  {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);

    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);

    Serial.print("Satellites: ");
    Serial.println(gps.satellites.value());

    Serial.println("--------------------");
  }
  else
  {
    Serial.println("Searching satellites...");
  }

  // ---------------- SOUND SENSOR ----------------
  sensorState = digitalRead(soundSensor);

  if (sensorState == LOW)
  {
    if (!isSMSsent)
    {
      // CHECK GPS VALIDITY
      if (gps.location.isValid())
      {
        double flat = gps.location.lat();
        double flon = gps.location.lng();

        // SWITCH TO GSM
        Gsm.listen();

        Gsm.println("AT+CMGF=1");
        delay(1000);

        Gsm.print("AT+CMGS=\"");
        Gsm.print(phone_no);
        Gsm.println("\"");

        delay(1000);

        Gsm.println("ALERT! I Need Help!");
        Gsm.print("Location: ");

        Gsm.print("http://maps.google.com/maps?q=loc:");
        Gsm.print(flat, 6);
        Gsm.print(",");
        Gsm.print(flon, 6);

        delay(500);

        Gsm.write(26); // CTRL+Z to send SMS

        delay(5000);

        Serial.println("SMS Sent Successfully");

        isSMSsent = true;
      }
      else
      {
        Serial.println("GPS location not valid yet!");
      }
    }
  }
  else
  {
    isSMSsent = false;
  }

  delay(1000);
}