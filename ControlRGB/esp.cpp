#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Configurar el servidor en el puerto 80
ESP8266WebServer server(80);

// Pin para el LED
const int ledPin = 2;  // GPIO2 del ESP-01 (D4)

// Variables de estado del LED
bool ledState = false;  // Apagado inicialmente

// Función para manejar la solicitud de encender el LED
void handleLedOn() {
  digitalWrite(ledPin, HIGH);  // Enciende el LED
  ledState = true;
  Serial.println("LED encendido");
  server.send(200, "text/html", "<html><body><h1>LED Encendido</h1><a href='/'>Regresar</a></body></html>");
}

// Función para manejar la solicitud de apagar el LED
void handleLedOff() {
  digitalWrite(ledPin, LOW);  // Apaga el LED
  ledState = false;
  Serial.println("LED apagado");
  server.send(200, "text/html", "<html><body><h1>LED Apagado</h1><a href='/'>Regresar</a></body></html>");
}

// Función para manejar la selección del color RGB
void handleRgbColor() {
  if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
    String r = server.arg("r");
    String g = server.arg("g");
    String b = server.arg("b");

    // Enviar valores RGB por Serial
    String rgbCommand = "RGB(" + r + "," + g + "," + b + ")";
    Serial.println(rgbCommand);

    String htmlResponse = "<html><body><h1>Color enviado: " + rgbCommand + "</h1><a href='/'>Regresar</a></body></html>";
    server.send(200, "text/html", htmlResponse);
  } else {
    server.send(400, "text/plain", "Parametros RGB no encontrados");
  }
}

// Página principal con la rueda de color, previsualización y botones
void handleRoot() {
  String htmlPage = "<html><head>"
                    "<title>Control de LED y Color RGB</title>"
                    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                    "<style>"
                    "body { font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 0; background-color: #f4f4f4; }"
                    "h1 { color: #333; margin-top: 20px; }"
                    ".container { display: flex; flex-direction: column; align-items: center; justify-content: center; padding: 20px; }"
                    "input[type='range'] { -webkit-appearance: none; width: 80%; height: 15px; background: #ddd; border-radius: 5px; margin: 10px 0; }"
                    "input[type='range']::-webkit-slider-thumb { -webkit-appearance: none; width: 25px; height: 25px; background: #888; border-radius: 50%; cursor: pointer; }"
                    "button { width: 80%; padding: 15px; margin: 10px 0; background-color: #28a745; color: white; border: none; border-radius: 5px; font-size: 18px; cursor: pointer; }"
                    "button:hover { background-color: #218838; }"
                    ".led-buttons { display: flex; justify-content: space-around; width: 80%; }"
                    ".led-buttons a { width: 45%; }"
                    "#colorBox { width: 150px; height: 150px; margin-top: 20px; background-color: rgb(128, 128, 128); }"
                    "</style>"
                    "<script>"
                    "function updateColor() {"
                    "  var r = document.getElementById('r').value;"
                    "  var g = document.getElementById('g').value;"
                    "  var b = document.getElementById('b').value;"
                    "  var colorBox = document.getElementById('colorBox');"
                    "  colorBox.style.backgroundColor = 'rgb(' + r + ',' + g + ',' + b + ')';"
                    "}"
                    "function sendRGB() {"
                    "  var r = document.getElementById('r').value;"
                    "  var g = document.getElementById('g').value;"
                    "  var b = document.getElementById('b').value;"
                    "  var url = '/rgb?r=' + r + '&g=' + g + '&b=' + b;"
                    "  window.location.href = url;"
                    "}"
                    "</script>"
                    "</head><body>"
                    "<h1>Control de LED y Color RGB</h1>"
                    "<div class='container'>"
                    "<label for='r'>Rojo</label>"
                    "<input type='range' id='r' name='r' min='0' max='255' value='128' style='background: linear-gradient(to right, #000, red);' oninput='updateColor()'><br>"
                    "<label for='g'>Verde</label>"
                    "<input type='range' id='g' name='g' min='0' max='255' value='128' style='background: linear-gradient(to right, #000, green);' oninput='updateColor()'><br>"
                    "<label for='b'>Azul</label>"
                    "<input type='range' id='b' name='b' min='0' max='255' value='128' style='background: linear-gradient(to right, #000, blue);' oninput='updateColor()'><br><br>"
                    "<div id='colorBox'></div><br><br>"
                    "<button onclick='sendRGB()'>Enviar Color RGB</button><br><br>"
                    "<div class='led-buttons'>"
                    "<a href='/led/on'><button style='background-color: #007bff;'>Encender LED</button></a>"
                    "<a href='/led/off'><button style='background-color: #dc3545;'>Apagar LED</button></a>"
                    "</div>"
                    "</div>"
                    "</body></html>";

  server.send(200, "text/html", htmlPage);
}

void setup() {
  // Configurar el pin del LED como salida
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Aseguramos que el LED esté apagado al inicio

  // Iniciar la comunicación serial
  Serial.begin(9600);

  // Configurar el ESP-01 como punto de acceso (Access Point)
  WiFi.softAP("ESP-RGB-Control", "12345678");  // Nombre y contraseña de la red

  // Imprimir la IP del punto de acceso
  Serial.println("Punto de acceso iniciado.");
  Serial.print("IP del punto de acceso: ");
  Serial.println(WiFi.softAPIP());

  // Configurar rutas para el servidor web
  server.on("/", handleRoot);
  server.on("/led/on", handleLedOn);
  server.on("/led/off", handleLedOff);
  server.on("/rgb", handleRgbColor);

  // Iniciar servidor
  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  // Manejar las solicitudes del servidor
  server.handleClient();
}
