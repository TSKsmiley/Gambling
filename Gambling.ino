#include <TouchScreen.h> //touch bibliotek
#include <LCDWIKI_GUI.h> //selve grafik biblioteket
#include <LCDWIKI_KBV.h> //Hardware sepcifikt bibliotek



//   Her definere vi nogle aliases til de pins skærmen sidder forbundet

#define YP A3
#define XM A2
#define YM 9
#define XP 8

//her bruges det til at opprette en instance af den klasse der skal bruge for at tilgå skærmen
LCDWIKI_KBV my_lcd = LCDWIKI_KBV(ILI9341,A3,A2,A1,A0,A4);



// Definitioner til max og min tryk på skærmen

#define TS_MINX 911
#define TS_MAXX 117

#define TS_MINY 87 
#define TS_MAXY 906

#define MINPRESSURE 10
#define MAXPRESSURE 1000

//her bruges værdierne til at oprette en instance 
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Her definere vi aliaser til 16 bit farve hex-koder så vi bare kan skrive navnet på farven frem for hex koden
#define BLACK   0x0000
#define GREY    0x8C71
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//her sætter vi vores start værdier
int balance = 100;
int betBal = 100;

void show_string(String str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
  // denne metode skriver en string ud på skærmen i den størrelse og farve man definere
  my_lcd.Set_Text_Mode(mode);      //denne mode reffere til om der skal være tekst baggrund eller ej
  my_lcd.Set_Text_Size(csize);     //Størrelsen på texten
  my_lcd.Set_Text_colour(fc);      //selve text farven
  my_lcd.Set_Text_Back_colour(bc); //Text baggrundsfarve hvis mode er false
  my_lcd.Print_String(str,x,y);    //her skriver den selve texten
}

//Her lavers alt det grafiske til setup
void show_UI(void)
{
  my_lcd.Set_Draw_color(GREEN);
  
  // FORMAT TIL RECTANGLER  x_start, y_start, x_slut, y_slut
  // her tegner vi vores plus og minus knapper
  //TOP
  my_lcd.Fill_Round_Rectangle(5, 5, 42,39, 5);
  my_lcd.Fill_Round_Rectangle(192, 5, 234,39, 5);
  show_string("-",19,15,2,WHITE, BLACK,1);
  show_string("+",209,15,2,WHITE, BLACK,1);
  //BUND
  my_lcd.Fill_Round_Rectangle(5,my_lcd.Get_Display_Height() - 44, 42, my_lcd.Get_Display_Height() - 5, 5);
  my_lcd.Fill_Round_Rectangle(192,my_lcd.Get_Display_Height() - 44, 234, my_lcd.Get_Display_Height() - 5, 5);
  show_string("-",LEFT + 20,289,2,WHITE, BLACK,1);
  show_string("+",209,289,2,WHITE, BLACK,1);
  
  updateBalance(0);
  
  my_lcd.Set_Draw_color(GREEN);
  my_lcd.Fill_Round_Rectangle(45,my_lcd.Get_Display_Height() - 44, my_lcd.Get_Display_Width() - 52, my_lcd.Get_Display_Height() - 5, 5);
  show_string("Place bet", CENTER, 250, 2, WHITE, BLACK, 1);
  
  updateBet(0);
}

void updateBalance(int deltaBal) { //TODO COMMENT BAL
  balance += deltaBal;
  
  String temp = String("$" + String(balance));
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Fill_Rectangle(42, 0, 192, 39);
  show_string(temp,CENTER,15,2,WHITE, BLACK,1);
}

void updateBet(int deltaBet) { //TODO COMMENT BET
  if ((betBal += deltaBet) <= 0) {
    delay(100);
    return;
  }
  
  betBal += deltaBet;
  
  my_lcd.Set_Draw_color(GREEN);
  my_lcd.Fill_Round_Rectangle(45,my_lcd.Get_Display_Height() - 44, my_lcd.Get_Display_Width() - 52, my_lcd.Get_Display_Height() - 5, 5);
  String temp = String("$" + String(betBal));
  show_string(temp, CENTER, my_lcd.Get_Display_Height() - 30, 2, WHITE, BLACK, 1);
}

void dices(int dealerDice, int playerDice) { //TODO COMMENT DICES
  String dealer = String(String(dealerDice));
  String player = String(String(playerDice));
  
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Fill_Rectangle(0,40,my_lcd.Get_Display_Width(), my_lcd.Get_Display_Height() - 90);
  show_string("Dealer - " + dealer, CENTER, my_lcd.Get_Display_Height() / 2 - 75, 2, GREY, BLACK, 1);
  show_string("Player - " + player, CENTER, my_lcd.Get_Display_Height() / 2 - 50, 2, WHITE, BLACK, 1);
  if (dealerDice > playerDice) {
    show_string("You lose $" + String(betBal), CENTER, my_lcd.Get_Display_Height() / 2 + 45, 2, WHITE, BLACK, 1);
  }
  else if (dealerDice == playerDice) {
    show_string("You tied", CENTER, my_lcd.Get_Display_Height() / 2 + 15, 2, WHITE, BLACK, 1);
    show_string("and lost $" + String(betBal / 2), CENTER, my_lcd.Get_Display_Height() / 2 + 45, 2, WHITE, BLACK, 1);
  }
  else {
    show_string("You win $" + String(betBal), CENTER, my_lcd.Get_Display_Height() / 2 + 45, 2, GREEN, BLACK, 1);
  }
}

void setup(void) //dette køre før main loopet
{
  // her starter vi LCD skærmen og overskriver den med vores baggrundsfarve som er sort
  my_lcd.Init_LCD();
  my_lcd.Fill_Screen(BLACK);

  // her tegner vi alt det der kun skal tegnes til opstart
  show_UI();
  pinMode(13, OUTPUT);
}



void loop()
{
  // her sætter vi pin 13 til høj læse Touch Point og sætter pin 13 ned igen
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  //Her sættes de to delte pins til output fordi det mangler i Touch Point librariet 
  pinMode(XM, OUTPUT); // Syntaktisk udfordret kode
  pinMode(YP, OUTPUT); // Syntaktisk udfordret kode
  
  //hvis ikke trykket opfylder max og min kriteriene springes denne itteration over
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
  {
    
    //her sætter vi p.x og p.y
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, my_lcd.Get_Display_Width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0,my_lcd.Get_Display_Height());
    
    

    /*
      ALT HERUNDER ET TIL HVAD DER SKAL SKE BASERET PÅ HVOR HENNE PÅ SKÆRMEN MAN TRYKKER
      ALTSÅ ER DET LOGIKEN TIL KANPPERNE
    */

    if(((p.x>5)&&(p.x < 42)&&(p.y < 39))) // MINUS KNAP I TOPPEN
    {
      if (balance > 50) 
      {
        updateBalance(-100);
      }
    }
    else if(((p.x>192)&&(p.x < 234)&&(p.y < 39))) // PLUS KNAP I TOPPEN
    {
      if (balance <= 32600) 
      {
        updateBalance(100);
      }
    }
    else if((p.y > my_lcd.Get_Display_Height() - 39)) // ALLE 3 KNAPPER I BUNDEN AF SKÆRMEN
    {
      if (p.x < 42) // --------------------------------------------------- MINUS KNAP
      {
        if (betBal > 150) 
        {
          updateBet(-100);
        }
      }
      else if (p.x > my_lcd.Get_Display_Width() - 41) { // --------------- PLUS KNAP
        if (betBal < 10000)
        {
          updateBet(100);
        }
      }
      else if (((balance + betBal) >= 0)&&((balance - betBal) >= 0)) // -- BET KNAP
      {
        int dealerDice = random(1,7);
        int playerDice = random(1,7);
      
        if (dealerDice > playerDice) {      // HVIS spilleren taber
          dices(dealerDice, playerDice);
          updateBalance(-betBal);
        }
        else if (playerDice > dealerDice) { // HVIS spilleren vinder
          dices(dealerDice, playerDice);
          updateBalance(betBal);
        }
        else {                              // HVIS spilleren får det samme som huset
          dices(dealerDice, playerDice);
          updateBalance(-betBal / 2);
        }
      }
    }
  }
  random(1,7); // her trækkes der et tilfældigt tal for hver gang loopet køre for at gøre random mere random
}
