#define LED_OUT 35
#define LED_IN 39

void setup() {
    Serial.begin(9600);
    pinMode(LED_OUT,OUTPUT);
    pinMode(LED_IN,OUTPUT);
    digitalWrite(LED_IN,LOW);
}

void loop() {
  int x;
  String buf;
  if (Serial.available() > 0){
      x = Serial.parseInt();
      buf = "Blinking "+((String)x)+" times";
      Serial.write("Blinking");
      blink(x);
  }

}

void blink(int n){
  if (!n) {
    delay(2000);
    return;
  } else {
    digitalWrite(LED_OUT,HIGH);
    delay(1000);
    digitalWrite(LED_OUT,LOW);
    delay(1000);
    blink(n-1);
    return;
  }
}
