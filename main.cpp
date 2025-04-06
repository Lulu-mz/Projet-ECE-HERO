#include <Arduino.h>

// Définition des fréquences des notes
#define NOTE_DO 262
#define NOTE_RE 294
#define NOTE_MI 328
#define NOTE_FA 352
#define NOTE_SOL 390
#define NOTE_LA 437
#define NOTE_SI 498

// Variables globales
volatile unsigned long previousTime = 0;
volatile int freqDetected = 0;
volatile bool buttonPressed = false;
bool programmeActif = true;
bool musiqueEnCours = false;

// Mélodies
int auClairDeLaLune[11]= {
    NOTE_DO, NOTE_DO, NOTE_DO,
    NOTE_RE, NOTE_MI, NOTE_RE,
    NOTE_DO, NOTE_MI, NOTE_RE,
    NOTE_RE, NOTE_DO
};

int frereJacques[26]= {
    NOTE_DO, NOTE_RE, NOTE_MI, NOTE_DO,
    NOTE_DO, NOTE_RE, NOTE_MI, NOTE_DO,
    NOTE_MI, NOTE_FA, NOTE_SOL,
    NOTE_MI, NOTE_FA, NOTE_SOL,
    NOTE_LA, NOTE_SOL, NOTE_DO,
    NOTE_LA, NOTE_SOL, NOTE_DO,
    NOTE_DO, NOTE_DO, NOTE_DO,
    NOTE_DO, NOTE_DO, NOTE_DO
};

int viveLeVent[42]={
    NOTE_SOL, NOTE_SOL, NOTE_SOL,
    NOTE_SOL, NOTE_SOL, NOTE_SOL,
    NOTE_SOL, NOTE_LA, NOTE_SOL, NOTE_MI, NOTE_RE,
    NOTE_DO, NOTE_RE, NOTE_MI, NOTE_FA,
    NOTE_MI, NOTE_RE, NOTE_DO, NOTE_LA, NOTE_SOL, NOTE_SOL,
    NOTE_SOL, NOTE_SOL, NOTE_SOL,
    NOTE_SOL, NOTE_SOL, NOTE_SOL,
    NOTE_SOL, NOTE_LA, NOTE_SOL, NOTE_MI, NOTE_RE,
    NOTE_DO, NOTE_RE, NOTE_MI, NOTE_FA,
    NOTE_MI, NOTE_RE, NOTE_DO, NOTE_LA, NOTE_SOL, NOTE_SOL,
};

// Interruption pour détecter le son
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

// Détection de la fréquence jouée
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

// Vérifie si la note jouée est correcte
bool bonneNote(int freqJouee, int noteAttendue) {
  return abs(freqJouee - noteAttendue) <= 5;
}

// Fonction générique pour jouer une chanson
void jouerMusique(int* musique, int taille) {
    musiqueEnCours = true;

    for (int i = 0; i < taille && programmeActif; ) {
      Serial.print("Note attendue ");
      Serial.print(i + 1);
      Serial.print(" : ");
    Serial.println(musique[i]);
      int freq = -1;
      while (freq == -1 && programmeActif) {
        freq = detectFreq();
        delay(200);
      }
      if (freq == -1) continue;
    if (bonneNote(freq, musique[i])) {
        Serial.println("Bonne note !");
        i++;
      } else {
        Serial.println("Mauvaise note !");
      }
    }

    musiqueEnCours = false;
    Serial.println("Lecture terminee !");
}

// Menu principal
void lancerMenu() {
  Serial.println("=== MENU ===");
  Serial.println("1 - Au clair de la lune");
  Serial.println("2 - Frere Jacques");
  Serial.println("3 - Vive le vent");
  Serial.println("Choisissez une chanson :");
  while (Serial.available() == 0);
  int choix = Serial.read() - '0';
  Serial.read(); // Vider le buffer
  if (choix == 1) {
    Serial.println("Vous avez choisi : Au clair de la lune");
    jouerMusique(auClairDeLaLune, 11);
  } else if (choix == 2) {
    Serial.println("Vous avez choisi : Frere Jacques");
    jouerMusique(frereJacques, 26);
 } else if (choix == 3){
    Serial.println("Vous avez choisi : Vive le vent");
    jouerMusique(viveLeVent, 42);
 }
 else {
    Serial.println("Choix invalide.");
  }
}

// Boucle principale
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
    }
    // Si musique en cours, ignorer les notes hors séquence
    delay(300);
    }
}

// Setup
void setup() {
  attachInterrupt(digitalPinToInterrupt(2), detectSound, RISING);
  Serial.begin(9600);
}
