# **Les Aventures Farfelues de Nolan / Nolan's Wacky Adventures**

## **Français**

### **Description**

Les aventures farfelues de Nolan, est un jeu d'aventure narratif en 2D créé avec C++ et SFML. Suivez Nolan dans sa matinée chaotique alors qu'il tente d'arrvier à l'heure à l'école, rencontrant divers personnages aussi déglingos les uns que les autres, et parcourant différents endroits saugrenus.
La durée d'une run dure entre 4 et 6 minutes selon votre vitesse de lecture et de découverte.

--------------------------------------------------------------

### **Fonctionnalités**

- Plusieurs Zones: Explorez différents lieux incluant la maison, les rues, une zone de manifestation, la gare, l'intérieur d'un train et l'école

- NPCs Interactifs: Parlez à différents personnages avec un système de dialogues et pensées de Nolan lors de ces dialogues.

- Gameplay Narratif: Vivez le trajet matinal de Nolan à travers des interactions humoristiques.

- Transitions Visuelles: Transitions en fondu fluides entre les zones.

- Menu Pause: Mettez en pause et reprenez ou quittez vers le menu principal.

- Système de Crédits: Générique complet à la fin du jeu.

--------------------------------------------------------------

### **Contrôles**

- Z / Flèche Haut: Se déplacer vers le haut
- S / Flèche Bas: Se déplacer vers le bas
- Q / Flèche Gauche: Se déplacer vers la gauche
- D / Flèche Droite: Se déplacer vers la droite
- E: Interagir avec les NPCs / Avancer dans le dialogue
- ÉCHAP: Pause/Reprendre le jeu
- Entrée: Démarrer le jeu (menu principal)
- Clic Souris: Naviguer dans les menus

--------------------------------------------------------------

### **Prérequis techniques**

- **Dépendances** :
SFML 3.0+ (Graphics, Window, System) et 
C++17 ou supérieur

--------------------------------------------------------------

### **Installation**

//Cloner le dépôt dans visual studio//

https://github.com/ByiFire/Les_Aventures_Farfelues_de_Nolan.git

initialiser SFML si non fait précédemment. Puis compiler et jouer.

project/

├── Game.cpp / Game.hpp          # Logique principale et rendu

├── HUD.cpp / HUD.hpp            # Système HUD (timer, zone, dialogues)

├── NPC.cpp / NPC.hpp            # Système NPC et gestion des dialogues

├── IEntity.hpp                  # Interface abstraite des entités

├── Main.cpp                     # Point d'entrée

└── assets/
    
    └── fonts/
        
        └── arial.ttf            # Fichier de police requis

--------------------------------------------------------------

Problèmes connus :

Le fichier de la police qui doit impérativement être mit dans un dossier assets/fonts comme dit ci-dessus.

--------------------------------------------------------------

## **English**

### **Description**

Nolan's Wacky Adventures is a 2D narrative adventure game created with C++ and SFML. Follow Nolan through his chaotic morning as he tries to get to school on time, encountering various characters and navigating through different areas of the city.

--------------------------------------------------------------

### **Features**

- Multiple Areas: Explore different locations including home, streets, a protest zone, train station, inside a train, and school

- Interactive NPCs: Talk to various characters with multi-line dialogue systems

- Story-Driven Gameplay: Experience Nolan's morning journey through humorous interactions

- Visual Transitions: Smooth fade transitions between areas

- Pause Menu: Pause and resume or quit to main menu

- Credits System: Full credits roll at the end of the game

--------------------------------------------------------------

### **Controls**

- Z / Up Arrow: Move up
- S / Down Arrow: Move down
- Q / Left Arrow: Move left
- D / Right Arrow: Move right
- E: Interact with NPCs / Advance dialogue
- ESC: Pause/Unpause game
- Enter: Start game (main menu)
- Mouse Click: Navigate menus
--------------------------------------------------------------

### **Technical Requirements**
- Dependencies :

- SFML 3.0+ (Graphics, Window, System)
- C++17 or higher

--------------------------------------------------------------

### **Installation**

//Clone the github depository//

https://github.com/ByiFire/Les_Aventures_Farfelues_de_Nolan.git

Initialize SFML if it hasn't been done before and compile. Then play.

project/

├── Game.cpp / Game.hpp          

├── HUD.cpp / HUD.hpp            

├── NPC.cpp / NPC.hpp            

├── IEntity.hpp                  

├── Main.cpp                     

└── assets/
    
    └── fonts/
        
        └── arial.ttf            # Font required

--------------------------------------------------------------

Known bugs : 

The font file must be in a folder assets/font like said right above.

-------------------------------------------------------------  
# Thanks for reading, enjoy the game ! / Merci d'avoir lu, bon jeu !