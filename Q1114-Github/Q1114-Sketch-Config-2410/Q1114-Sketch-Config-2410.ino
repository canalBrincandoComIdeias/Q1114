#include <SoftwareSerial.h>

#define TEMPO_DELAY 30000

// RX do Arduino = 10 (recebe dados do TX do sensor)
// TX do Arduino = 11 (envia dados para o RX do sensor)
SoftwareSerial radarSerial(10, 11);

// Calcula checksum de um pacote (exceto os dois primeiros bytes 0x55 0xAA)
uint8_t calculateChecksum(const uint8_t* data, size_t len) {
  uint16_t sum = 0;
  for (size_t i = 2; i < len - 1; i++) {
    sum += data[i];
  }
  return sum & 0xFF;
}

void enterConfigMode() {
  uint8_t cmd[] = { 0x55, 0xAA, 0x00, 0xFF, 0x00, 0x00, 0x00 };
  cmd[6] = calculateChecksum(cmd, sizeof(cmd));
  radarSerial.write(cmd, sizeof(cmd));
  delay(100);
}

void exitConfigMode() {
  uint8_t cmd[] = { 0x55, 0xAA, 0x00, 0xFE, 0x00, 0x00, 0x00 };
  cmd[6] = calculateChecksum(cmd, sizeof(cmd));
  radarSerial.write(cmd, sizeof(cmd));
  delay(100);
}

// Define o tempo de delay do estado de "ausente" (em milissegundos)
// O valor deve estar entre 0 e 60.000 ms (60s), por limitação do LD2410
void setDelayTime(uint16_t delayMs) {
  if (delayMs > 60000) delayMs = 60000;

  uint8_t delayLow = delayMs & 0xFF;
  uint8_t delayHigh = (delayMs >> 8) & 0xFF;

  uint8_t cmd[] = {
    0x55, 0xAA, 0x00, 0x08, // Header e comando
    0x00, 0x02,             // Payload length = 2 bytes
    delayLow, delayHigh,   // Delay time in little endian
    0x00                   // Checksum (a calcular)
  };
  cmd[8] = calculateChecksum(cmd, sizeof(cmd));
  radarSerial.write(cmd, sizeof(cmd));
  delay(100);
}

void setup() {
  Serial.begin(9600);
  Serial.print("configurando para ");
  Serial.print(TEMPO_DELAY);
  Serial.println("(ms)...");

  radarSerial.begin(256000); // HLK-LD2410 usa 256000 bps
  delay(500);

  enterConfigMode();
  delay(200);

  setDelayTime(TEMPO_DELAY);  // Define em milisegundos de delay

  delay(200);
  exitConfigMode();
  Serial.println("configurado!");
}

void loop() {
  // vazio por enquanto
}
