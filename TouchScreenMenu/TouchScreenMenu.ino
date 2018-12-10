



//Compile on Arduino 1.0.1




#include <TouchScreenMenu.h>
#include <TouchScreen.h>
#include <TFT.h>
#include <cstddef.h>

double gasLevel = 0;
double oilPressure1 = 0;
double oilPressure2 = 0;
double tempurature = 0;
double amps = 0;
double speed = 0;
double tach = 0;
double o2Level = 0;	// 0.9vdc when rich		0.45vdc when good mix				0.1vdc when lean	produces voltage when around 600F
double mafs = 0;	// 0.4-0.8vdc at idle	4.5-5.0vdc at wide open throttle	Need to make it so the wire is cleaned after the car turns off
double brakeFluidLevel = 0;

double cabTemp = 0;
double outsideTemp = 0;

bool highBeam = false;
bool charge = false;
bool parkingBreak = false;

double intermitent = 0;	// Need to find a good default
double lowFanSpeed = 0;	// Need to figure out how the fan speeds work and pick a speed for low

// create the array of items for the main menu
TouchScreenMenuItem mainMenuItems[] = {
	TouchScreenMenuItem("-> Sub Menus"),
	TouchScreenMenuItem("Wiper Menu"),
	TouchScreenMenuItem("Dashboard"),
	TouchScreenMenuItem("ENDOFMENU")
};

// create the array of items for the first sub menu
TouchScreenMenuItem subMenuItems[] = {
	TouchScreenMenuItem("<- Main Menu"),
	TouchScreenMenuItem("Light Control"),
	TouchScreenMenuItem("Timer"),
	TouchScreenMenuItem("Test Screen"),
	TouchScreenMenuItem("Test Screen 2"),
	TouchScreenMenuItem("ENDOFMENU")
};

// Create the stuff for the timer screen
TouchScreenMenuItem timerItems[] = {
	TouchScreenMenuItem("<- Main Menu"),
	TouchScreenMenuItem("ENDOFMENU")
};

// Create an empty menu to be filled with dash data
TouchScreenMenuItem mainDashItems[] = {
	TouchScreenMenuItem("ENDOFMENU")
};

// Create menu-less screen
TouchScreenMenuItem emptyItems[] = {
	TouchScreenMenuItem("ENDOFMENU")
};

unsigned int black = TSC.createColor(0, 0, 0);
unsigned int white = TSC.createColor(255, 255, 255);
unsigned int orange = TSC.createColor(206, 114, 33);

// Create the various menus setting the (items, font size, spacing, padding, justification, titles)
TouchScreenMenu subMenu = TouchScreenMenu(subMenuItems, 2, 10, 5, CENTERJ, "Sub Menu");
TouchScreenMenu mainMenu = TouchScreenMenu(mainMenuItems, 2, 10, 10, CENTERJ, "Main Menu");
TouchScreenMenu timerScreen = TouchScreenMenu(timerItems, 2, 10, 10, CENTERJ, "Timer Screen");
TouchScreenMenu dimmerScreen = TouchScreenMenu(emptyItems, 2, 10, 10, CENTERJ, "Dimmer Screen");
TouchScreenMenu wipperScreen = TouchScreenMenu(emptyItems, 2, 10, 10, CENTERJ, "Wiper Menu");
TouchScreenMenu TestScreen = TouchScreenMenu(emptyItems, 2, 10, 10, CENTERJ);

// Keep track of which menu is the currently active one
TouchScreenMenu *curMenu = &mainMenu;

TouchScreenArea goBack = TouchScreenButton("<- Back", white, black, 50, TSC.getScreenHeight() - 50, 2, 10);

// These are for the wiper menu
TouchScreenArrowButton courseDown = TouchScreenArrowButton("courseDown", black, orange, 10, TSC.getScreenHeight() - 180 , 30, 30, LEFT);
TouchScreenArrowButton courseUp = TouchScreenArrowButton("courseUp", black, orange, 10, TSC.getScreenHeight() - 180 , 30, 30, RIGHT);
TouchScreenArrowButton fineDown = TouchScreenArrowButton("fineDown", black, orange, 10, TSC.getScreenHeight() - 180 , 30, 30, LEFT);
TouchScreenArrowButton fineUp = TouchScreenArrowButton("fineUp", black, orange, 10, TSC.getScreenHeight() - 180 , 30, 30, RIGHT);

// TouchScreenSlider(text, foreColor, backColor, x, y, w, h, Layout layout);
TouchScreenSlider lightBright = TouchScreenSlider("lightBright",TSC.createColor(200, 200, 200), TSC.createColor(50, 50, 255), 5, TSC.getScreenHeight() - 110, 150, 40, HORIZONTAL);

float counter = 0;

// Resolution is 240 x 320
// Setup all variables that need setting after the arduino starts up
void setup(void) {
	Serial.begin(9600);
	Serial.println("Starting...");
	TSC.setBackColor(TSC.createColor(255, 255, 0)); // change the default background color
	TSC.init(); // make sure everything gets initialized
	Serial.println("TSC Initialized!");
	
	curMenu->draw(); // put up the main menu
	Serial.println("Setup Complete!");
}

// Loop through this method until the end of time, this is the engine
void loop(void) {
	// handle the current menu
	if(curMenu == &timerScreen){
		// TouchScreenLabel(char *text, unsigned int foreColor, unsigned int backColor, unsigned int x, unsigned int y, unsigned int fontSize, unsigned int padding, bool drawBackground);
		// void drawString(char *string,unsigned int poX, unsigned int poY,unsigned int size,unsigned int color);
		char buffer[7];
		dtostrf(counter, 7, 0, buffer);
		Serial.println(buffer);
		TSC.drawString(buffer, 0, 300, 3, TSC.createColor(255, 255, 0));
		counter = counter + 1;
		TSC.drawString(buffer, 0, 300, 3, TSC.createColor(0, 0, 0));
	}
	else if(curMenu != NULL){
		// process the current menu
		TouchScreenMenuItem *item = curMenu->process(false);
		// check to see which, if any, menu item was pressed
		checkMenuSelection(item);
	}
	checkButtons();
}

// check various buttons and perform actions if any was pressed
void checkButtons(){
	if((curMenu != &mainMenu && curMenu != &subMenu) && goBack.process()){ // return from the graphics function screen
		Serial.println("goBack");
        curMenu = &mainMenu;
        TSC.clearScreen();
        curMenu->draw();
	}
	else if(curMenu == &dimmerScreen && lightBright.process()){ // return from the graphics function screen
		// Serial.println("lightBright");
		Serial.print("Brightnes slider Value");
		float brightness = lightBright.getValue();
		Serial.print(brightness);
		Serial.println("");
		Serial.println(brightness*255);
		analogWrite(A8, brightness*255);
		
	}
	// else if(backFromBtns.process()){ // return from the buttons screen
		// Serial.println("backFromBtns");
        // curMenu = &mainMenu;
        // TSC.clearScreen();
        // curMenu->draw();
  // }
  // else if(backFromLbls.process()){ // return from the labels screen
		// Serial.println("backFromLbls");
        // curMenu = &mainMenu;
        // TSC.clearScreen();
        // curMenu->draw();
  // }
}

// Check to see if any menu item was pressed and do something
void checkMenuSelection(TouchScreenMenuItem *item) {
	boolean handled = false;
	if(item != NULL){
		Serial.print("Item Clicked: ");
		Serial.println(item->getText());
		

		if(!strcmp(item->getText(),"<- Main Menu")){
			curMenu = &mainMenu;
			TSC.clearScreen();
			curMenu->draw();
			handled = true;
		}
		else if(!strcmp(item->getText(),"Wiper Menu")){
			curMenu = &wipperScreen;
			TSC.clearScreen();
			
			courseDown.draw();
			courseUp.draw();
			fineDown.draw();
			fineUp.draw();
			goBack.draw();
			
			curMenu->draw();
			handled = true;
		}
		else if(!strcmp(item->getText(),"-> Sub Menus")){
			curMenu = &subMenu;
			TSC.clearScreen();
			curMenu->draw();
			handled = true;
		}
		else if(!strcmp(item->getText(),"Test Screen")){
			curMenu = &TestScreen;
			TSC.clearScreen();
			// 	drawRectangle(poX, poY, width, height, color, boolean fill);
			for( int y = 0; y < 320; y = y + 20 ) {
				for( int x = 0; x < 240; x = x + 20 ) {
					TSC.drawRectangle(x, y, 10, 10, white, true);
					TSC.drawRectangle(x, y + 10, 10, 10, black, true);
					TSC.drawRectangle(x + 10, y + 10, 10, 10, white, true);
					TSC.drawRectangle(x + 10, y, 10, 10, black, true);
				}
			}
			handled = true;
		}
		else if(!strcmp(item->getText(),"Test Screen 2")){
			unsigned long processTime;
			TSC.clearScreen();
			// 	drawRectangle(poX, poY, width, height, color, boolean fill);
			processTime = millis();
			TSC.drawRectangle(0, 0, 240, 320, white, true);
			Serial.print("It took ");
			Serial.print(millis() - processTime);
			Serial.print(" milliseconds to fill with white.");
			processTime = millis();
			Serial.println();
			
			TSC.drawRectangle(0, 0, 240, 320, black, true);
			Serial.print("It took ");
			Serial.print(millis() - processTime);
			Serial.print(" milliseconds to fill with black.\n");
			
			TSC.clearScreen();
			curMenu->draw();
			handled = true;
		}
		else if(!strcmp(item->getText(),"Timer")){
			curMenu = &timerScreen;
			TSC.clearScreen();
			curMenu->draw();
			handled = true;
		}
		else if(!strcmp(item->getText(),"Light Control")){
			lightBright.setValue(.5); // change the value on one of the sliders
			lightBright.setPadding(10); // change the padding on one of the sliders
			// btn8.setValue(.75); // change the value on one of the sliders
			// btn8.setPadding(5); // change the padding on one of the sliders
			curMenu = &dimmerScreen;
			TSC.clearScreen();
			curMenu->draw();
			goBack.draw();
			lightBright.draw();
			handled = true;
		}
		// if the menu item didn't get handled redraw it unpressed
		if(handled==false){
			curMenu->drawItem(item,false);
		}
	}
}

