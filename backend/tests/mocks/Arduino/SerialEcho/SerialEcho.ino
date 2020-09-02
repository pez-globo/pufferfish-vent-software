/*
  SerialPassthrough sketch
  Modified to be a simple loop back
*/

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
  char in;
  if (Serial.available()) {
    in = Serial.read();
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.write(in);
    if (in == '!') delay(1000);
  }
}
