void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Lights on");
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Lights off");
    delay(500);
}