/*
 * KaraClass A basic user environment for arduino touch screen 
 * Copyright: Jean-Pierre Cocatrix jp@cocatrix.fr http://www.karawin.fr
 * Code: https://github.com/karawin/karaClass
 * 
 * based on and require
 * ILI9341_due.h - Arduino Due library for interfacing with ILI9341-based TFTs
 * Code: https://github.com/marekburiak/ILI9341_due
 * Documentation: http://marekburiak.github.io/ILI9341_due/
 * 
 * Require: 
 *  UTouch.h - Arduino/chipKit library support for Color TFT LCD Touch screens 
 *  Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
 * You can find the latest version of the library at 
 * http://www.RinkyDinkElectronics.com/
 * 
 * It is a partial class:
 * TScreen must be defined with your needs ouside.
 * See examples folder
 *  No needs to modify this .h
 */
#ifdef __arm__
#ifndef karaScreen_h
#define karaScreen_h

#if defined(ARDUINO_ARCH_AVR)
    #include <avr/pgmspace.h>
#elif defined(ARDUINO_SAM_DUE)
    #define PROGMEM
#endif
// some trace if DEBUG = 1 else 0
#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define dbgprint Serial.print
#define dbgprintln Serial.println
#else
#define dbgprint(...) /**/  
#define dbgprintln(...) /**/  
#endif

#include <ILI9341_due.h>
#include "fonts\allFonts.h"

#define KARACLASS_VERSION 1.0

// size of the screen
#define WIDTH 320
#define HEIGHT 240
// space between group of buttons
#define PAD 5
// Height of a group of buttons
#define BHEIGHT 68
// Default keyboard to Majuscule, minuscule or numerique
#define KBMAJ 1
#define KBMIN 3
#define KBNUM 2
//Height of a keyboard
#define KHEIGHT 160
//Height of the Keyboard Banner Caption
#define KCHEIGHT 35
// nb Key in a raw of keyboard
#define NRKEY 10
// nb of raw in keyboard
#define NRKBOARD 4
// Position of buttons panel: POSBTS1: bottom  POSBTS2: middle  POSBTS3: top
#define POSBTS1 HEIGHT-BHEIGHT
#define POSBTS2 HEIGHT-(2*BHEIGHT)
#define POSBTS3 HEIGHT-(3*BHEIGHT)
// height of a status panel
#define SHEIGHT 22
// max size of the screen logo
#define LHEIGHT 32
#define LWIDTH  32
// max nuber of TButtons on TPanels
#define MAXBTS 5
// max number of buttons in a Tbuttons group
#define MAXBT 10
// default font for the status bar
#define TSTFONT Arial14 
// default space between line on TPANEL
#define LSPACE 3
// Nb Max of lines on TPANEL
#define MAXLINE 12 
// Nb max of status in a status bar
#define MAXSTATUS 4

//-------------------------------------------
//An event class to execute some action asked from interrupt.
// An interrupt must be as short as possible so one arms the event to do some action in the main loop.
// See TScreen in the example.
//-------------------------------------------
class TEventScreen
{
protected:
bool trigger;
public:
  uint16_t xt;
  uint16_t yt;
  void TrigOn(uint16_t x, uint16_t y){xt = x;yt = y; trigger = true;}
  void TrigOff(void) {trigger = false;}   
  TEventScreen(){;}
  bool isArmed(){return trigger;}
};
//
//An image for the logo or button
class TLogo
{
  public:
  ILI9341_due* Parent;
  uint16_t Width = 0;
  uint16_t Height = 0;
  uint16_t *Image ;
  TLogo( ILI9341_due* parent, uint16_t *image) { Parent = parent, Image = image;}
  TLogo( ILI9341_due* parent, const  uint16_t *image){Parent = parent; Image = (uint16_t*)image; }
  void Background(uint16_t color); //if not in eprom, change the white color of the logo to color, for button use
  void BackgroundWhite(uint16_t color); //revert to white the previously changed logo
  void Draw( uint16_t x, uint16_t y);  // Draw it at x,y
};
//-------------------------------------------
// Base class of  objects to be displayed
// base data and method.
class TBase
{
protected:
  ILI9341_due* Parent; // the TScreen itself
  bool active = false; // is or can be displayed
  bool displayed = false; // is on screen. changed to true in Draw
public:
  virtual void Touch(uint16_t xt, uint16_t yt) {;}
  virtual void unTouch(uint16_t xt, uint16_t yt){;}
  virtual void Draw(void) = 0;
  bool isActive(void){ return active;} 
  bool isDisplayed (void){ return displayed;} 
  bool unDisplay(void){ displayed = false;}
  void Show(void){ active = true;}
  void Hide(void){ active = false;displayed = false;}       
};
//
//-------------------------------------------
// a Button to be contained in a TButtons panel
class TButton: public TBase
{
public:
  TButton(ILI9341_due* parent,uint16_t left, uint16_t top, uint16_t width, uint16_t height);
  uint16_t Color ; // background color
  String Caption;  // The caption on the button
  TLogo *Logo = NULL;     // a logo if needed
  TLogo *LogoOn = NULL;   // a logo for bistable button in state = true
  bool BiStable = false;  // if true, the button is a bistable
  bool State = false;     // state for bistate
  void Draw(void);
  void (*Action)(void) = NULL; // the external action to be called when pressed.
  void Touch(void);
  void unTouch(void) {Draw();}   
  uint16_t Left; uint16_t Top; uint16_t Width; uint16_t Height;  // position on screen (from constructor)
};


//-------------------------------------------
//A mini keyboard panel of NRKEY*NRKBOARD keys , a banner and a String to show the typing
class TKeyboard: public TBase
{
private:
  uint16_t topa ; // top for all panel
  uint16_t top;   // top of the keyboard key
  bool state = false;
  uint16_t color = ILI9341_BLUE;
public:
  TKeyboard(ILI9341_due* parent);
  String Caption = ""; 
  String Banner = "";
  TButton* Key[NRKEY][NRKBOARD];
  uint16_t btColor = ILI9341_GOLD;  // default color
  void setTop(uint16_t Top);  // may be for a wider screen ;-)
  uint16_t getTop(){return topa;}
  void Touch(uint16_t xt, uint16_t yt);
  void unTouch(uint16_t xt, uint16_t yt);
  void Draw(void);
  void Start(String banner, uint8_t set ); // Init the keyboard with banner and the set if any 
  bool isAvailable(){return state;}  // A  String is ready
  void SetMn();
  void SetBase(); 
  void Set1();   
  void Set2();   
  void Set3();   
};
//-------------------------------------------
//A group of  nb button in a raw  (a panel) at the bottom by default
// else call setTop
class TButtons: public TBase
{
private:
  uint16_t   nb;
  uint16_t top ;
  uint16_t color = ILI9341_BLUE;
  bool grouped = false;
public:
  TButtons(ILI9341_due* parent,  uint16_t nbr); // nbr = number of buttons in the panel. MAXBT maxi
  TButton* Button[MAXBT];
  uint16_t btColor = ILI9341_GOLD;  // default button  background color 
//  String Caption;  // not used for now
  void setTop(uint16_t Top);
  uint16_t getTop(){return top;}
  void Touch(uint16_t xt, uint16_t yt);
  void unTouch(uint16_t xt, uint16_t yt); 
  void Draw(void);
  void Grouped(bool state);  // if true, all buttons are bistable but only one is in state trus
};
//-------------------------------------------
// Main Panel  of the screen
class TPanel: public TBase
{
private:
  short index = 0; // current index 
  uint16_t until = HEIGHT-LHEIGHT;
  uint16_t from =  LHEIGHT;  
public:
  TPanel(ILI9341_due* parent ){Parent = parent;Show();} // show is the default
  String Line[MAXLINE+2]; // array of String (text lines)
  uint16_t Color = ILI9341_DARKBLUE;
  void clearPanelLines();
  void clearPanel();
  void Println(char* str); // with scrolling
  void Println(String str);// with scrolling
  void Print(char* str); // with scrolling
  void Print(String str);// with scrolling
  void Draw(void);
  void Draw(uint16_t from,uint16_t until);
  void Until(short unt){until = unt; }  // to display until the top of buttons panel if any
  void From(short fro){from= fro;}      // to display from the bottom of  buttons panel if any
  void Touch(uint16_t xt, uint16_t yt){if (Action != 0) Action();} 
  void unTouch(uint16_t xt, uint16_t yt){;} 
  void (*Action)(void) = NULL; // external action on touch
};

//-------------------------------------------
// A panel and Buttons container
// 
class TPanels: public TBase
{
public: 
  TPanels(ILI9341_due* parent);
  bool Active = true;
  TButtons* Bts[MAXBTS]; // the set of buttons panels
  TKeyboard* Keyboard = NULL;  // created if needed
  TPanel* Panel = 0; 
  void Touch(uint16_t xt, uint16_t yt);
  void unTouch(uint16_t xt, uint16_t yt);
  void Draw();
  void StartKeyboard(String banner, uint8_t set );// create and show keyboard 
  String GetKeyboard(); // return caption and destroy keyboard
  bool isKeyboard(void){return Keyboard->isAvailable();}// a caption ready  
};
//-------------------------------------------
// a status label to be drawn in a status bar
class TStatus
{
  private:
  String label="";  // printed_t as "label: caption" at x left pixel
  String caption="";
  uint16_t at;  // position
  uint16_t color = ILI9341_LIGHTSTEELBLUE;
 ILI9341_due* Parent; 
public:
  TStatus(ILI9341_due* parent,String lab,String capt, uint16_t x)  {Parent = parent; label = lab; caption = capt; at = x; }
  bool State = false;  // if true, the color is changed in green else it is red
  void Caption(char* cap){caption=cap;Modified = true;}
  void Label(char* cap){label=cap; Modified = true;}
  void Color(uint16_t col){ color = col;}
  void Draw();
  uint16_t At(){ return at;}
  void At(uint16_t here){ at = here;}
  uint16_t Width(); 
  bool Modified = false; // true if label or caption is changed externally (interrupt...)
};

//-------------------------------------------
//The status bar at the top of the screen
// contains up to MAXSTATUS status
// show sd status, wifi status, time etc...
// always visible
class TStBar
{
private:
  uint16_t color = ILI9341_BLACK;
  uint16_t forecolor;
  void reDraw(uint16_t index);
  ILI9341_due* Parent;  
public:
  TStBar(ILI9341_due* parent); 
  TStatus *Status[MAXSTATUS];
  // some status to display
  void Draw();
  void Refresh(); // some display to update if modified?
};



#endif
#else
  #error Oops! Trying to include  on another device?
#endif