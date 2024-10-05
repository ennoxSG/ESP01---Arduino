void setup() {
  // Inicializamos el puerto serie a 9600 bps
  Serial.begin(9600);
  Serial.println("Esperando datos del ESP-01...");
}

void loop() {
  // Verificamos si hay datos disponibles en el puerto serie
  if (Serial.available()) {
    // Leemos los datos y los mostramos en el monitor serial
    String data = Serial.readString();
    Serial.println("Datos recibidos desde ESP-01: " + data);
  }
}
