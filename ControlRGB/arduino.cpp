// Pines del LED RGB
const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;

// Variables para almacenar los valores RGB
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

// Variables para el estado del LED
bool ledState = false;

void setup() {
  // Configurar los pines del LED RGB como salidas
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Iniciar la comunicación serial
  Serial.begin(9600);

  // Asegurarse de que el LED RGB esté apagado al inicio
  setColor(0, 0, 0);
}

void loop() {
  // Verificar si hay datos disponibles en la comunicación serial
  if (Serial.available()) {
    // Leer el comando completo
    String command = Serial.readStringUntil('\n');
    command.trim();  // Quitar espacios en blanco y saltos de línea

    // Verificar si el comando es para encender o apagar el LED
    if (command == "LED encendido") {
      ledState = true;
      applyColor();  // Aplicar el color actual al LED
      Serial.println("LED encendido");
    } else if (command == "LED apagado") {
      ledState = false;
      setColor(0, 0, 0);  // Apagar el LED
      Serial.println("LED apagado");
    } 
    // Verificar si el comando es un color RGB
    else if (command.startsWith("RGB")) {
      // Extraer los valores RGB del comando
      int rStart = command.indexOf('(') + 1;
      int rEnd = command.indexOf(',');
      int gEnd = command.lastIndexOf(',');
      int bEnd = command.indexOf(')');

      redValue = command.substring(rStart, rEnd).toInt();
      greenValue = command.substring(rEnd + 1, gEnd).toInt();
      blueValue = command.substring(gEnd + 1, bEnd).toInt();

      Serial.print("Color recibido: R=");
      Serial.print(redValue);
      Serial.print(", G=");
      Serial.print(greenValue);
      Serial.print(", B=");
      Serial.println(blueValue);

      // Si el LED está encendido, aplicar el nuevo color
      if (ledState) {
        applyColor();
      }
    }
  }
}

// Función para aplicar el color RGB al LED
void applyColor() {
  setColor(redValue, greenValue, blueValue);
}

// Función para establecer los valores RGB en los pines del LED
void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}
