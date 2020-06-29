/*
 Name:		AccelTest.ino
 Created:	6/28/2020 10:17:55 PM
 Author:	Jirka
*/

// the setup function runs once when you press reset or power the board
void setup() {

	Serial.begin(9600);

	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);

}

// the loop function runs over and over again until power down or reset
void loop() {
	
	int x = analogRead(A0);
	delayMicroseconds(100);
	int y = analogRead(A1);
	delayMicroseconds(100);
	int z = analogRead(A2);
	delayMicroseconds(100);
	
	/*
	Serial.print(" | X: "); Serial.print(x);
	Serial.print(" \t | \t Y: "); Serial.print(y);
	Serial.print(" \t | \t Z: "); Serial.print(z);
	Serial.print(" \t | ");
	Serial.println();
	
	delay(100);
	*/
	
}
