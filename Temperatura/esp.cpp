#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>

const char* ssid = "CONF";
const char* password = "12345678";

float latitude = 14.6407; // Valor por defecto Guatemala 
float longitude = -90.5133; // Valor por defecto Guatemala

ESP8266WebServer server(80);

void setup() {
  Serial.begin(9600);
  
  // Conexión a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
  Serial.print("IP del ESP: ");
  Serial.println(WiFi.localIP());

  // Definimos las rutas del servidor
  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
  server.on("/data", handleData);
  
  // Inicia el servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}

// Maneja las solicitudes a la raíz
void handleRoot() {
  String html = "<html><head><title>Datos del Tiempo</title>"
                "<style>"
                "body { font-family: Arial, sans-serif; text-align: center; background-color: #f2f2f2; }"
                "input[type='text'] { padding: 10px; width: 200px; margin: 10px; }"
                "button { padding: 10px; background-color: #4CAF50; color: white; border: none; }"
                "button:hover { background-color: #45a049; }"
                "</style>"
                "</head><body>"
                "<h1>Datos del Tiempo</h1>"
                "<h2>Temperatura: <span id='temp'>Cargando...</span> °C</h2>"
                "<h2>Velocidad del Viento: <span id='wind'>Cargando...</span> m/s</h2>"
                "<input type='text' id='lat' placeholder='Latitud' value='" + String(latitude) + "'>"
                "<input type='text' id='lon' placeholder='Longitud' value='" + String(longitude) + "'>"
                "<button onclick='updateLocation()'>Actualizar</button>"
                "<script>"
                "function updateLocation() {"
                "  var lat = document.getElementById('lat').value;"
                "  var lon = document.getElementById('lon').value;"
                "  fetch('/update?lat=' + lat + '&lon=' + lon);"
                "}"
                "setInterval(function() {"
                "  fetch('/data').then(response => response.json()).then(data => {"
                "    document.getElementById('temp').innerText = data.temperature;"
                "    document.getElementById('wind').innerText = data.windspeed;"
                "  });"
                "}, 10000);" // Actualiza cada 10 segundos
                "</script>"
                "</body></html>";
  
  server.send(200, "text/html", html);
}

// Manejo de la actualización de ubicación
void handleUpdate() {
  if (server.hasArg("lat") && server.hasArg("lon")) {
    latitude = server.arg("lat").toFloat(); // Actualiza la latitud
    longitude = server.arg("lon").toFloat(); // Actualiza la longitud
    Serial.print("Nueva latitud: ");
    Serial.println(latitude);
    Serial.print("Nueva longitud: ");
    Serial.println(longitude);
    server.send(200, "text/plain", "Ubicación actualizada");
  } else {
    server.send(400, "text/plain", "Error: Parámetros faltantes");
  }
}

// Manejo de la obtención de datos
void handleData() {
  WiFiClientSecure client;  
  client.setInsecure(); // Omite la verificación del certificado

  String apiUrl = "my.meteoblue.com";
  String path = "/packages/current?apikey=&lat=" + String(latitude) + "&lon=" + String(longitude) + "&asl=1508&format=json";

  Serial.println("Intentando conectar a la API...");
  if (!client.connect(apiUrl, 443)) {
    Serial.println("Error al conectar con la API");
    server.send(500, "text/plain", "Error al conectar con la API");
    return;
  }
  Serial.println("Conectado a la API");

  // Realiza la solicitud GET
  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + apiUrl + "\r\n" +
               "Connection: close\r\n\r\n");

  // Espera la respuesta
  String json;
  while (client.connected() || client.available()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      // Solo necesitamos procesar la línea de respuesta del JSON
      if (line.startsWith("{")) {
        json = line; // Almacena el JSON completo
        break; // Salir del bucle una vez procesada la respuesta
      }
    }
  }
  client.stop();

  // Enviar la respuesta JSON al cliente
  if (json.length() > 0) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, json);

    if (!error) {
      float temperature = doc["data_current"]["temperature"];
      float windspeed = doc["data_current"]["windspeed"];
      
      // Crear una respuesta JSON
      String response = "{\"temperature\":" + String(temperature) + ", \"windspeed\":" + String(windspeed) + "}";
      server.send(200, "application/json", response);
    } else {
      Serial.println("Error al analizar el JSON");
      server.send(500, "text/plain", "Error al analizar el JSON");
    }
  } else {
    server.send(500, "text/plain", "No se recibió datos");
  }
}
