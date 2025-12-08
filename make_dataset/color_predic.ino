#define RED 6
#define GREEN 4
#define BLUE 5
#define SENSOR 7

void allLedsOff() {
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
}
void allLedsOn() {
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, HIGH);
}
void ledOn(int p) {
  allLedsOff();
  digitalWrite(p, HIGH);
}

int readAvg() {
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(SENSOR);
    delay(2);
  }
  return sum / 10;
}

void setup() {
  Serial.begin(115200);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  allLedsOff();
  delay(2000);
  Serial.println("R,G,B,W,D");  // header
}

void loop() {
  allLedsOff();
  delay(60);
  int d = readAvg();
  allLedsOn();
  delay(60);
  int w = readAvg();
  ledOn(RED);
  delay(60);
  int r = readAvg();
  ledOn(GREEN);
  delay(60);
  int g = readAvg();
  ledOn(BLUE);
  delay(60);
  int b = readAvg();
  allLedsOff();

  Serial.printf("R:%d\nG:%d\nB:%d\nW:%d\nD:%d\n", r, g, b, w, d);
  delay(500);  // cəmi ~1 saniyədə 1 ölçmə → modellə eyni!
}