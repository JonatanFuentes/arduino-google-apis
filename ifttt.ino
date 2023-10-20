#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 103);
float lectura;
float volt;
EthernetServer server(80);
float value1;
float value2 = 0;
float value3;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
 
  
  Ethernet.begin(mac, ip);

  server.begin();
  
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  lectura = analogRead(A0);
  volt = lectura / 1023.0 * 5.0;
  
   value1 = volt;
   
   sendDataToSheet();
    
    delay(1800000);

  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 1");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.print("Volt: ");
          client.println(volt);
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    
    client.stop();
    
   
    // La fecha y hora ya está definida como valor fijo
    
    
  }
}

void sendDataToSheet() {
  EthernetClient client;
  
  if (client.connect("maker.ifttt.com", 80)) {
    String url = "/trigger/pozo2/with/key/bHNZ2E5bRfePNBIHQkCWr-RLkbnr72gVg-y1X337tKp";
    url += "?value1=" + String(value1) + "&value2=" + String(value2) + "&value3=" + String(value3);
    
    client.print("GET " + url + " HTTP/1.1\r\n");
    client.print("Host: maker.ifttt.com\r\n");
    client.print("Connection: close\r\n\r\n");
    
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    
    client.stop();
  } else {
    Serial.println("Connection to server failed");
  }
}
