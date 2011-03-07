#define DELAY 100

#define VOLDOWN	2
#define PREV 	3
#define STOP 	4
#define PLAY 	5
#define NEXT 	6
#define VOLUP 	7

int led = 13;

int button_state = LOW;


void setup()
{
	// initialize the serial communication:
	Serial.begin(9600);
	
	// Initialize pin modes
	for(int i=2; i<=7; i++)
	{
		pinMode(i,INPUT);
	}
	
	pinMode(led,OUTPUT);
	
}

void blink()
{
	digitalWrite(led, HIGH);
	delay(DELAY);
	digitalWrite(led, LOW);
	delay(DELAY);
		
}

void handlePin(int pin)
{
	button_state = digitalRead(pin);
	
	if(button_state == HIGH)
	{
		blink();
		Serial.write(pin);
	}
	else
	{
		digitalWrite(led,LOW);
	}
	
}

void loop()
{
	int i;
	for(i=2; i<=7; i++)
	{
		handlePin(i);
	}
		
}
