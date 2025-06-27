//Pinos
#define pinSensor 8     //ARDUINO=8  ESP=4
#define pinRele 7       //ARDUINO=7  ESP=21

#define NIVEL_RELE_LIGADO HIGH

//Variáveis Globais
bool estado = false;
bool estadoAnt = false;

void setup() {
  pinMode(pinSensor, INPUT);
  pinMode(pinRele, OUTPUT);

  digitalWrite(pinRele, !NIVEL_RELE_LIGADO);

  Serial.begin(9600);
  delay(2000);  //Espera para iniciar o serial e o módulo HLK-LD2420, que também precisa de um tempo para iniciar.
  Serial.println("Sketch Iniciado...");
}

void loop() {
 
  estado = digitalRead(pinSensor);

  if (estado != estadoAnt) {
    if (estado) {
      Serial.println("Pessoa Detectada!");
      digitalWrite(pinRele, NIVEL_RELE_LIGADO);
    } else {
      Serial.println("ausente");
      digitalWrite(pinRele, !NIVEL_RELE_LIGADO);
    }
  }

  estadoAnt = estado;
  delay(20);
}
