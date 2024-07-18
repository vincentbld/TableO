#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Configuration Wi-Fi
const char* ssid = "Ideas-Lab Invités";
const char* password = "autonomie";

// Configuration des LEDs
#define PIN1           14
#define PIN2           12
#define NUMPIXELS1     21
#define NUMPIXELS2     24

Adafruit_NeoPixel pixels1(NUMPIXELS1, PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2(NUMPIXELS2, PIN2, NEO_GRB + NEO_KHZ800);

// Configuration du NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200, 60000); // UTC+2 pour Paris (heure d'été)

// Définir les couleurs pour chaque jour
uint32_t colors[7] = {
  pixels1.Color(102, 255, 150), // Lundi - Vert clair
  pixels1.Color(102, 178, 255), // Mardi - Bleu clair
  pixels1.Color(255, 102, 178), // Mercredi - Rose
  pixels1.Color(255, 255, 102), // Jeudi - Jaune
  pixels1.Color(51, 153, 255),  // Vendredi - Bleu
  pixels1.Color(255, 153, 51),  // Samedi - Orange
  pixels1.Color(178, 102, 255)  // Dimanche - Mauve
};

// Couleurs pour chaque segment de temps sur le ruban 2
uint32_t morningColor = pixels2.Color(255, 140, 0); // Matin - Orange foncé
uint32_t noonColor = pixels2.Color(255, 255, 0);    // Midi - Jaune vif
uint32_t afternoonColor = pixels2.Color(173, 216, 230); // Après-midi - Bleu clair
uint32_t eveningColor = pixels2.Color(0, 0, 139);  // Soir - Bleu foncé

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connexion au Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Initialisation des LEDs
  pixels1.begin();
  pixels2.begin();
  pixels1.show(); // Initialise toutes les LEDs à 'off'
  pixels2.show(); // Initialise toutes les LEDs à 'off'

  // Initialisation du client NTP
  timeClient.begin();
}

void loop() {
  timeClient.update();
  
  // Obtenir le jour de la semaine (0 = dimanche, 1 = lundi, ..., 6 = samedi)
  int dayOfWeek = (timeClient.getDay() + 6) % 7; // pour aligner avec le ruban LED (commencer à 0 pour lundi)

  // Effacer toutes les LEDs
  pixels1.clear();
  pixels2.clear();
  
  // Allumer 3 LEDs sur le ruban 1 selon le jour de la semaine
  for (int i = 0; i < 3; i++) {
    int ledIndex = (dayOfWeek * 3 + i) % NUMPIXELS1;
    pixels1.setPixelColor(ledIndex, colors[dayOfWeek]);
  }

  // Obtenir l'heure actuelle en secondes depuis minuit
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  int currentTimeInSeconds = currentHour * 3600 + currentMinute * 60 + currentSecond;

  // Définir les plages horaires en secondes depuis minuit
  int morningStart = 5 * 3600;
  int morningEnd = 12 * 3600;
  int noonStart = 12 * 3600 + 1 * 60;
  int noonEnd = 13 * 3600 + 30 * 60;
  int afternoonStart = 13 * 3600 + 31 * 60;
  int afternoonEnd = 17 * 3600 + 30 * 60;
  int eveningStart = 17 * 3600 + 31 * 60;
  int eveningEnd = 23 * 3600;
  int nightStart = 23 * 3600 + 1 * 60;

  // Allumer les LEDs sur le ruban 2 en fonction de la plage horaire
  if (currentTimeInSeconds >= morningStart && currentTimeInSeconds <= morningEnd) {
    // Matin : 6 LEDs
    for (int i = 0; i < 6; i++) {
      pixels2.setPixelColor(i, morningColor);
    }
  } else if (currentTimeInSeconds >= noonStart && currentTimeInSeconds <= noonEnd) {
    // Midi : 4 LEDs
    for (int i = 6; i < 10; i++) {
      pixels2.setPixelColor(i, noonColor);
    }
  } else if (currentTimeInSeconds >= afternoonStart && currentTimeInSeconds <= afternoonEnd) {
    // Après-midi : 6 LEDs
    for (int i = 10; i < 16; i++) {
      pixels2.setPixelColor(i, afternoonColor);
    }
  } else if (currentTimeInSeconds >= eveningStart && currentTimeInSeconds <= eveningEnd) {
    // Soir : 8 LEDs
    for (int i = 16; i < 24; i++) {
      pixels2.setPixelColor(i, eveningColor);
    }
  } else if (currentTimeInSeconds >= nightStart || currentTimeInSeconds < morningStart) {
    // Eteindre toutes les LEDs la nuit
    pixels2.clear();
  }

  // Afficher les changements
  pixels1.show();
  pixels2.show();
  
  // Attendre avant de rafraîchir (ajuster si nécessaire)
  delay(1000);
}
