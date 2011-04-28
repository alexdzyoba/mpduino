#define DELAY 100

#define VOLDOWN	2
#define PREV 	3
#define STOP 	4
#define PLAY 	5
#define NEXT 	6
#define VOLUP 	7

// Notification led
int led = 13;

int button_state = LOW;


void setup()
{
	// initialize the serial communication: set baud rate
	Serial.begin(9600);
	
	// Initialize buttons pin modes
	for(int i=2; i<=7; i++)
	{
		pinMode(i,INPUT);
	}
	
	// Initialize led pin mode
	pinMode(led,OUTPUT);
	
}

// Blink routine
void blink()
{
	digitalWrite(led, HIGH);
	delay(DELAY);
	digitalWrite(led, LOW);
	delay(DELAY);
		
}

void handlePin(int pin)
{
	// Read pin state
	button_state = digitalRead(pin);
	
	// If there were high voltage, i.e. button was pressed
	if(button_state == HIGH)
	{
		// blink led
		blink();
		
		// Send throug serial port pin number
		Serial.write(pin);
	}
	else
	{
		digitalWrite(led,LOW);
	}
	
}

// Main cycle
void loop()
{
	int i;
	// Ask each pin
	for(i=2; i<=7; i++)
	{
		handlePin(i);
	}
		
}
