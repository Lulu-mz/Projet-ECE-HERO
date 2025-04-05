#include <Arduino.h>

#define NOTE_DO 262
#define NOTE_RE 294
#define NOTE_MI 328
#define NOTE_FA 352
#define NOTE_SOL 390
#define NOTE_LA 437
#define NOTE_SI 498

volatile unsigned long previousTime = 0;
volatile int freqDetected = 0;
volatile bool buttonPressed = false;
bool programmeActif = true;
bool musiqueEnCours = false;

int auClairDeLaLune[11]= {
  NOTE_DO, NOTE_DO, NOTE_DO,
  NOTE_RE, NOTE_MI, NOTE_RE,
  NOTE_DO, NOTE_MI, NOTE_RE,
  NOTE_RE, NOTE_DO
};

void detectSound() {
  unsigned long currentTime = micros();
  if (currentTime - previousTime > 1000) {
    unsigned long interval = currentTime - previousTime;
    previousTime = currentTime;

    if (interval > 0) {
      buttonPressed = true;
      freqDetected = 1000000 / interval;
    }
  }
}

int detectFreq() {
  if (buttonPressed) {
    noInterrupts();
    int freq = freqDetected;
    buttonPressed = false;
    interrupts();

    if (freq > 0) {
      Serial.print("Frequence detectee : ");
      Serial.print(freq);
      Serial.print(" Hz - Note : ");
      if (freq >= 240 && freq <= 267) {
        Serial.println("Do");
        return NOTE_DO;
      } else if (freq >= 275 && freq <= 299) {
        Serial.println("Re");
        return NOTE_RE;
      } else if (freq >= 300 && freq <= 333) {
        Serial.println("Mi");
        return NOTE_MI;
      } else if (freq >= 334 && freq <= 357) {
        Serial.println("Fa");
        return NOTE_FA;
      } else if (freq >= 370 && freq <= 395) {
        Serial.println("Sol");
        return NOTE_SOL;
      } else if (freq >= 420 && freq <= 442) {
        Serial.println("La");
        return NOTE_LA;
      } else if (freq >= 480 && freq <= 503) {
        Serial.println("Si");
        return NOTE_SI;
      } else {
        Serial.println("Inconnu");
        return -1;
      }
    }
  }
  return -1;
}

bool bonneNote(int freqJouee, int noteAttendue) {
  return abs(freqJouee - noteAttendue) <= 5;
}

void lancerMenu() {
  Serial.println("=== MENU ===");
  Serial.println("1 - Au clair de la lune");
  Serial.println("Choisissez une chanson :");

  while (Serial.available() == 0);
  int choix = Serial.read() - '0';
  Serial.read(); // Vider le buffer

  if (choix == 1) {
    Serial.println("Vous avez choisi : Au clair de la lune");
    int taille = 11;
    musiqueEnCours = true;

    for (int i = 0; i < taille && programmeActif; ) {
      Serial.print("Note attendue ");
      Serial.print(i + 1);
      Serial.print(" : ");
      Serial.println(auClairDeLaLune[i]);

      int freq = -1;
      while (freq == -1 && programmeActif) {
        freq = detectFreq();
        delay(200);
      }

      if (freq == -1) continue;

      if (bonneNote(freq, auClairDeLaLune[i])) {
        Serial.println("Bonne note !");
        i++;
      } else {
        Serial.println("Mauvaise note !");
      }
    }

    musiqueEnCours = false;
    Serial.println("Lecture terminee !");
  } else {
    Serial.println("Choix invalide.");
  }
}

void loop() {
  if (!programmeActif) return;

  Serial.println("Bienvenue !");
  Serial.println("Jouez la note DO pour lancer le menu.");
  Serial.println("Jouez la note RE pour quitter.");

  while (programmeActif) {
    int note = detectFreq();
    if (note == -1) {
      delay(300);
      continue;
    }

    if (!musiqueEnCours) {
      if (note == NOTE_DO) {
        lancerMenu();
        Serial.println("Retour au menu principal.");
      } else if (note == NOTE_RE) {
        programmeActif = false;
        Serial.println("Programme arrete.");
      }
    } else {
      // En cas d'appui accidentel hors de la boucle principale pendant une musique
      // Ignorer ici, la détection est gérée dans lancerMenu()
    }

    delay(300);
  }
}

void setup() {
  attachInterrupt(digitalPinToInterrupt(2), detectSound, RISING);
  Serial.begin(9600);
}
