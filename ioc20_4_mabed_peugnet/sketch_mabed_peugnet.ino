

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

#define OLED_RESET     4                // Reset pin 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define MAX_WAIT_FOR_TIMER 5
unsigned int waitFor(int timer, unsigned long period){
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];  // il y a autant de timers que de tâches périodiques
  unsigned long newTime = micros() / period;              // numéro de la période modulo 2^32 
  long delta = newTime - waitForTimer[timer];              // delta entre la période courante et celle enregistrée
  if ( delta < 0 ) delta = 1 + newTime;                   // en cas de dépassement du nombre de périodes possibles sur 2^32 
  if ( delta ) waitForTimer[timer] = newTime;             // enregistrement du nouveau numéro de période
  return delta;
}

//----------------------------------Boîtes à lettres-------------------------------------//

enum {EMPTY, FULL};

struct mailbox0 {                                          
  int state;                                              // valeur indiquant l'état de la boite
  int val;                                                // valeur pouvant être lue ou écrite par deux tâches indépendantes l'une de l'autre.                                           
} mb0 = {.state = EMPTY};


struct mailbox2 {                                          
  int state;                                              // valeur indiquant l'état de la boite
  int val;                                                // valeur pouvant être lue ou écrite par deux tâches indépendantes l'une de l'autre.                                           
} mb2 = {.state = EMPTY};

struct mailbox1{
  int state;
  int val;
}mb1 = {.state = EMPTY};

//----------------------------------Définition de la tâche Led-------------------------//

struct Led_st {
  int timer;                                              // numéro du timer pour cette tâche utilisée par WaitFor
  unsigned long period;                                   // periode de clignotement
  int pin;                                                // numéro de la broche sur laquelle est la LED
  int etat;                                               // etat interne de la led
}; 

void setup_Led( struct Led_st * ctx, int timer, unsigned long period, byte pin) {
  ctx->timer = timer;
  ctx->period = period;
  ctx->pin = pin;
  ctx->etat = 0;
  pinMode(pin,OUTPUT);
  digitalWrite(pin, ctx->etat);
}

void loop_Led(struct Led_st * ctx, struct mailbox0 *mb, struct mailbox1 *mb1) {
  if (!waitFor(ctx->timer, ctx->period)) return;                                 // sort s'il y a moins d'une période écoulée
  //digitalWrite(ctx->pin,ctx->etat);                                            // écriture
  //ctx->etat = 1 - ctx->etat;                                                   // changement d'état
  
  if (mb->state != FULL) return;                          // attend que la mailbox soit pleine 
  int valeur=map(mb->val,0,1023,100,750);                 //On lit la valeur relevée par la photorésistance et l'adopte à notre besoin de faire clignoter la Led avec map
  digitalWrite(ctx->pin,HIGH);                            // la Led clignote de façon inversement proportionnel à la lumière reçue.
  delay(valeur);                                          //Plus la lumière est intense plus le delais de clignotement de la led est long et donc clignotera lentement.
  digitalWrite(ctx->pin,LOW);                             
  delay(valeur);
  
  if (mb1->state != FULL) return;
  //if (mb1->val != HIGH){
    for(int i=0;i<10000;i++)
    digitalWrite(ctx->pin,mb1->val);
  
}


//-------------------------------Définition de la tâche Mess-------------------------------//

struct Mess_st {
  int timer;                                              // numéro de timer utilisé par WaitFor
  unsigned long period;                                   // periode d'affichage
  char mess[20];
} Mess_t ; 

void setup_Mess(struct Mess_st * ctx, int timer, unsigned long period, const char * mess) {
  ctx->timer = timer;
  ctx->period = period;
  strcpy(ctx->mess, mess);
  Serial.begin(9600);                                     // initialisation du débit de la liaison série
}

void loop_Mess(struct Mess_st *ctx) {
  if (!(waitFor(ctx->timer,ctx->period))) return;         // sort s'il y a moins d'une période écoulée
  Serial.println(ctx->mess);                              // affichage du message
}

//-------------------------------Définition de la tache Oled---------------------------------//

struct Oled_st {
  int timer;                                              // numéro du timer pour cette tâche utilisée par WaitFor
  unsigned long period;                                                                                
} Oled_t; 

void setup_Oled(struct Oled_st * ctx, int timer, unsigned long period) {
  ctx->timer = timer;
  ctx->period = period;
  
//Initialisation de l'écran Oled  
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3C for 128x32
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    
  // the library initializes this with an Adafruit splash screen.
  display.display();

  display.setTextSize(1);                                 // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);                    // Draw white text
  display.setCursor(0,0);                                 // Start at top-left corner
  display.cp437(true);                                    // Use full 256 char 'Code Page 437' font
  display.clearDisplay();   
}


int cpt =0;                                               //Initialisation de notre compteur

void loop_Oled(struct Oled_st * ctx, struct mailbox0 *mb) {
  if (!(waitFor(ctx->timer,ctx->period))) return;         // sort s'il y a moins d'une période écoulée
  display.clearDisplay();
  display.setTextSize(2); 
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if (mb->state != FULL) return;                          //On attend que la boîte soit pleine
  int valeur=map(mb->val, 50, 900, 0, 100);               //On lit la valeur relevée par la photorésistance et l'adopte à notre besoin de l'afficher en pourcentage avec map 
  display.print(valeur);                                  //Affichage du pourcentage de luminosité reçu par la photorésistance
  display.println("%"); 
  mb->state = EMPTY;
  display.display();
  
  /*display.println(F("Compteur:"));                      //Affichage du compteur
  display.display();
  cpt++;                                                  //Incrémentation du compteur
  display.print(cpt);
  display.display(); */ 
}



//-----------------------------Définition de la tâche Lum---------------------------------//

struct Lum_st {
  int timer;                                              // numéro du timer pour cette tâche utilisé par WaitFor
  unsigned long period;                                                                                
}; 

void setup_Lum(struct Lum_st * ctx, int timer, unsigned long period) {
  ctx->timer = timer;
  ctx->period = period;
}

void loop_Lum(struct Lum_st * ctx, struct mailbox0 *mb0, struct mailbox2 *mb2) {
  if (!(waitFor(ctx->timer,ctx->period))) return;
  int valeur = analogRead(A1);                                        //lit le le port analogique A1
  
  if (mb0->state != EMPTY) return;                                     // attend que la mailbox0 soit vide
  mb0->val = valeur;                                                   // On enregistre dans l'argument val, de la boîte aux lettres, la valeur lue par la photorésistance; comprise entre 0 et 1023.
  mb0->state = FULL;                                                   // On signal que la boîte est remplie avec une nouvelle valeur pour val

  if (mb2->state != EMPTY) return; 
  mb2->val = valeur;
  mb2->state = FULL;
} 

//------------------------------Déclaration de la tâche ISR---------------------------//

struct ISR_st {
  int timer;                                              // numéro du timer pour cette tâche utilisé par WaitFor
  unsigned long period; 
};

struct ISR_st ISR1;

void setup_ISR(struct ISR_st * ctx, int timer, unsigned long period) {
  ctx->timer = timer;
  ctx->period = period;
  Serial.begin(9600);
}


void loop_ISR(struct ISR_st * ctx, struct mailbox1 * mb1) {
  if (!(waitFor(ctx->timer,ctx->period))) return;
  if(mb1->state != EMPTY)return;
  mb1->val = LOW;
  mb1->state = FULL;
}


void serialEvent() {
    char input = (char)Serial.read();
    if(input == 's'){
      loop_ISR(&ISR1,&mb1);}
   
}



//------------------------------Déclaration des tâches----------------------------------//

struct Led_st Led1;
struct Mess_st Mess1, Mess2;
struct Oled_st Oled1;
struct Lum_st Lum1;

//------------------------------Setup et Loop-------------------------------------------//

void setup() {
  setup_Led(&Led1, 0, 100000, 13);                    // Led1 est exécutée toutes les 100ms 
  setup_Mess(&Mess1, 1, 1000000, "bonjour");          // Mess1 est exécutée toutes les secondes 
  setup_Mess(&Mess2, 2, 1500000, "Salut");            // Mess2 est exécutée toutes les 1.5 secondes 
  setup_Oled(&Oled1, 3,1000000);                      // Oled1 afficher un compteur qui s'incrémente toutes les secondes sur l'écran OLED
  setup_Lum(&Lum1,4,500000);                          // Lum1 lit toutes les 0,5 seconde le port analogique A1 
  setup_ISR(&ISR1,5,10000);
}


void loop() {
  loop_Led(&Led1,&mb0,&mb1);                                        
  loop_Mess(&Mess1); 
  loop_Mess(&Mess2);
  loop_Oled(&Oled1,&mb0);
  loop_Lum(&Lum1,&mb0,&mb2); 
  loop_ISR(&ISR1,&mb1); 
}













  
