# IOC20 3 Mabed Peugnet : Pilotage d'un écran LCD en mode utilisateur et par un driver

## 1. Configuration des GPIO pour le pilotage de l'écran 

### Questions

### Q1. 
Il faut configurer les GPIOs selon le mapping suivant: RS(7), E(27), D4(22), D5(23), D6(24), D7(25), tout en étant à l'état write.

### Q2. 
L'écriture des valeurs vers le LCD se fait en deux étapes: d'abord les 4 bits de poids fort, puis les 4 bits de poids faible.
Le LCD est en mode 4 bits, mais comme les commandes et les données sont transmmises sur 8 bits, on doit procéder ainsi.

### Q3.
Pour réaliser l'initialisation, on doit envoyer vers l'afficheur les valeurs  suivantes:

lcd_command(0b00110011);
lcd_command(0b00110010);
Au démarrage, l'afficheur est dans un mode non défini (8 bits ou 4 bits).
Ces deux prmières instructions permettent de forcer le passage en mode 4 bits.

### Q4. 
On demande l'affichage d'un caractère en mettant RS à '1' et E à '0', puis on appelle la fonction "lcd_message()".

### Q5.
Pour réaliser l'effacement de l'écran, on appelle la fonction "lcd_clear() qui contient les commandes suivantes :

-`lcd_command(LCD_CLEARDISPLAY)` et `lcd_command(LCD_RETURNHOME)`.

-Pour le déplacement du curseur on envoie la commande `LCD_SETDDRAMADDR`.


## 1. Question du fichier `lcd_user.c`

### Q1.

Le mot clé `volatile` permet de toujour reprendre la valeur d'une variable en memoire plutôt que de garder celle présente dans le cache. Dans notre cas les valeurs peuvent changer à tout moment on ne veut donc pas utiliser le cache.

### Q2.

Dans open, on utilise les flags:
- `O_RDWR` parce qu'on veut pouvoir à la fois lire et écrire sur les broches du GPIO.
- `O_SYNC` pour indiquer au système que l'on souhaite ne pas utiliser le cache et donc envoyer les écritures directement.

Dans mmap, on utilise les flags:
- `PROT_READ` pour pouvoir lire les données.
- `PROT_WRITE` pour pouvoir écrire des données.

### Q3.

L'appel de `munmap` permet de libérer proprement la memoire mappée précedemment.

### Q4.

- `LCD_FUNCTIONSET` permet de choisir la command function set, c'est elle qui permet d'initialiser le lcd.
- `LCD_FS_4BITMODE` indique qu'on est en mode 4bits.
- `LCD_FS_2LINE` on envoie `1` dans le bit qui definit le nombre de lignes (N), ce qui permet de passer en mode 2 lignes.
- `LCD_FS_5x8DOTS` permet de choisir la font en envoyant `0` dans le bit correspondant (F): ici 8 dots
- `LCD_DISPLAYCONTROL` choisis la commande display on/off control.
- `LCD_DC_DISPLAYON` envoie `1` dans (D) ce qui permet d'allumer le lcd.
- `LCD_DC_CURSOROFF` envoie `0` dans (C).
- `LCD_ENTRYMODESET` permet de choisir la commande Entry mode set.
- `LCD_EM_RIGHT` envoie `0` dans (I/D) ce qui indique qu'on déplace le curseur vers la droite.
- `LCD_EM_DISPLAYNOSHIFT` envoie `0` dans (S) ce qui désactive le shift.

### Q5.

- `a` est un tableau contenant les valeurs du debut de chaque ligne.
- `len` est le nombre de caractères par ligne.
- `i` est l'index du caratère danss le message à afficher.
- `l` est le numéro de la ligne sur laquelle on écrit.

le premier for permet de changer de ligne à chaque fois qu'on dépasse `len` et ainsi choisir le bon début de ligne.
le second for permet  d'envoyer pour une ligne chaque caractère du message l'un après l'autre.


