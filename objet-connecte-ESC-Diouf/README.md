Projet de Session — Coffre du Laboratoire C-200

1. Contexte du projet

Ce prototype a été conçu dans le cadre du cours Objets connectés.
L’objectif est de créer une énigme interactive (prototype) pouvant s’intégrer à une salle d’escape game.
Le système, basé sur un microcontrôleur ESP32, combine plusieurs capteurs et actionneurs afin d’offrir une expérience immersive : détection de présence, saisie d’un code secret, rétroaction lumineuse et sonore.


2. Concept narratif
   
Les joueurs explorent le laboratoire abandonné du Dr Verdan, un chercheur obsédé par les ondes lumineuses et la conversion de la couleur en énergie.Avant sa disparition, il aurait créé une expérience chromatique capable de concentrer la lumière pure.

En entrant dans la pièce, un dispositif ancien s’allume soudainement.Le capteur réagit à leur présence et un message s’affiche :« Lorsque l’onde initiale chute, l’équilibre apparaît, la résonance persiste, la lumière s’ouvre. »

Autour d’eux, quatre boutons colorés : rouge, vert, bleu et jaune.En observant les indices du carnet du chercheur, les joueurs comprennent que chaque phrase correspond à une étape du spectre lumineux.

Ils doivent alors entrer la bonne séquence.

S’ils réussissent, le dispositif s’illumine, une mélodie résonne, et le coffre virtuel du Dr Verdan s’ouvre enfin, révélant sa découverte.



3. Éléments matériels
   
Élément	Type	Rôle

ESP32 DevKitC	    Microcontrôleur	Unité principale de traitement
PIR (GPIO 23)	    Capteur de présence	Active le système quand un joueur approche
Boutons (12 – 15)	Capteurs d’entrée	Permettent de saisir la combinaison
LED verte (GPIO 2)	Actionneur visuel	Indique la réussite
LED rouge (GPIO 4)	Actionneur visuel	Indique l’échec
Buzzer (GPIO 5)	Actionneur sonore	Produit les bips et la mélodie
Résistances, fils, breadboard	—	Connexions et protection des composants


4. Fonctionnement

- Détection de présence : le capteur PIR détecte la chaleur du joueur et envoie un signal à l’ESP32.
Le système s’active ; la LED verte clignote brièvement et un bip indique que le dispositif est prêt.

- Saisie du code : le joueur appuie sur les quatre boutons correspondant aux couleurs rouge, vert, bleu et jaune.

- Validation : la séquence entrée est comparée au code secret programmé dans le microcontrôleur.

- Si la combinaison est correcte : la LED verte s’allume et une mélodie de victoire est jouée.

- Si la combinaison est erronée : la LED rouge s’allume et un bip long retentit.

- Réinitialisation : après un temps donné, le système revient à l’état initial et attend une nouvelle détection PIR.


5. Schéma électronique

ESP32 relié à :

- Capteur PIR sur GPIO 23

- Boutons poussoirs sur GPIO 12, 13, 14 et 15

- LED verte sur GPIO 2

- LED rouge sur GPIO 4

- Buzzer (PWM LEDC) sur GPIO 5

- Chaque bouton (integrés pour les boutons) et LED possède une résistance de protection.
Un schéma complet accompagne le rapport pour illustrer les connexions.



6. Code et structure

Le projet est divisé en modules :

- main.c : logique principale (boucle de jeu, validation de la séquence).

- buzzer.c / buzzer.h : gestion du son (bip et mélodie).

- pir_sensor.c / pir_sensor.h : détection de présence.

- push_button.c / push_button.h : lecture des boutons.

- led.c / led.h : contrôle des voyants lumineux.




7. Améliorations prévues (Remise 2)

Pour la seconde phase du projet, un module de communication IoT sera ajouté.
L’ESP32 enverra un message HTTP vers un tableau de bord (ThingSpeak ou IFTTT) pour consigner les réussites.

Des mesures simples de sécurité seront intégrées, telles qu’une clé API et la validation du message.


8. Vidéo démonstrative
    
Une courte vidéo (6 minutes) montre le fonctionnement du prototype :

Détection de présence

Entrée d’une mauvaise séquence (LED rouge)

Entrée de la bonne séquence (LED verte + mélodie)

Réinitialisation automatique


9.   Auteur

Projet réalisé par Mouhammad Wagane Diouf