#include <iostream>

#include <wiringPi.h>

constexpr int ledPin {0};

int main(int argc, char *argv[]){
	wiringPiSetup();	//Initialize wiringPi.
	
	pinMode(ledPin, OUTPUT);//Set the pin mode
    std::cout << "Using pin " << ledPin << std::endl; //Output information on terminal

	while(1){
		digitalWrite(ledPin, HIGH);  //Make GPIO output HIGH level
		std::cout << "led turned on >>>" << std::endl; //Output information on terminal
		delay(1000);						//Wait for 1 second
		digitalWrite(ledPin, LOW);  //Make GPIO output LOW level
		std::cout << "led turned off <<<" << std::endl;		//Output information on terminal
		delay(1000);						//Wait for 1 second
	}

    return 0;
}