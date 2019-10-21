
int i = 0;

void setup()
{
    //Serial.begin(9600);
    Serial.println("setup() called");
}

void loop()
{
    Serial.print("loop() called ");
    Serial.println(i, DEC);
    i++;
    delay(1000);
}