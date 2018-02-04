#include <Arduino.h>
#include "network.h"
#include "biorb.h"

String hostname("your_AP_SSID");
const char* ap_default_psk = "your_AP_passphrase"; ///< Default PSK.
ESP8266WiFiMulti wifiMulti;

// TCP server at port 80 will respond to HTTP requests
ESP8266WebServer  httpServer(80);

//holds the current upload
File fsUploadFile;

//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

String getContentType(String filename) {
  if (httpServer.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.htm";

  if ( httpServer.args() > 0 )
  {
    if ( httpServer.hasArg("send") )  {
      String strValue = httpServer.arg("send");
      uint32_t uValue = strtoul((const char *)strValue.c_str(), 0, 0);
      Serial.print("sending Biorb-Code: " + strValue );
      SendCode(uValue);
    } else
    {
      int i;
      Serial.println("HTTP args:");
      for (i = 0; i < httpServer.args(); i++) Serial.println("\t" + httpServer.argName(i) + "=" + httpServer.arg(i) );
    }
  }
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = httpServer.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload() {
  if (httpServer.uri() != "/edit") return;
  HTTPUpload& upload = httpServer.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //Serial.print("handleFileUpload Data: "); Serial.println(upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
    Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
  }
}

void handleFileDelete() {
  if (httpServer.args() == 0) return httpServer.send(500, "text/plain", "BAD ARGS");
  String path = httpServer.arg(0);
  Serial.println("handleFileDelete: " + path);
  if (path == "/")
    return httpServer.send(500, "text/plain", "BAD PATH");
  if (!SPIFFS.exists(path))
    return httpServer.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  httpServer.send(200, "text/plain", "");
  path = String();
}

void handleFileCreate() {
  if (httpServer.args() == 0)
    return httpServer.send(500, "text/plain", "BAD ARGS");
  String path = httpServer.arg(0);
  Serial.println("handleFileCreate: " + path);
  if (path == "/")
    return httpServer.send(500, "text/plain", "BAD PATH");
  if (SPIFFS.exists(path))
    return httpServer.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if (file)
    file.close();
  else
    return httpServer.send(500, "text/plain", "CREATE FAILED");
  httpServer.send(200, "text/plain", "");
  path = String();
}

void handleFileList() {
  if (!httpServer.hasArg("dir")) {
    httpServer.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = httpServer.arg("dir");
  Serial.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";
  httpServer.send(200, "text/json", output);
}

void networkInit(void)
{
  /*
      Initialize network
  */
  WiFi.hostname(hostname);

  if (WiFi.getMode() != WIFI_STA)
  {
    WiFi.mode(WIFI_STA);
    delay(10);
  }

  wifiMulti.addAP("SSID_1", "PASSPHRASE_1");
  wifiMulti.addAP("SSID_2", "PASSPHRASE_2");
  wifiMulti.addAP("SSID_3", "PASSPHRASE_3");

  // don't wait, observe time changing when ntp timestamp is received
  Serial.println("Wait for WiFi connection.");

  // ... Give ESP 20 seconds to connect to station.
  unsigned long startTime = millis();
  uint8_t i = 0;

  while (wifiMulti.run() != WL_CONNECTED && millis() - startTime < 20000)
  {
    Serial.write('.');
    if ( (i++) % 2 )  SendCode(BIORB_BRIGHTNESS_40);  else SendCode(BIORB_BRIGHTNESS_100);
    //Serial.print(WiFi.status());
    delay(500);
  }
  Serial.println();

  // Check connection
  if (wifiMulti.run() == WL_CONNECTED)
  {
    Serial.print("connected to: ");
    Serial.println(WiFi.SSID());

    String theSSID = WiFi.SSID();

    if ( theSSID.equals("SSID_1") ) { IPAddress ip(192,168,6,40); IPAddress mask(255,255,255,0); IPAddress gw(0,0,0,0); WiFi.config(ip, gw, mask); }
    if ( theSSID.equals("SSID_2") ) { IPAddress ip(192,168,25,40); IPAddress mask(255,255,255,0); IPAddress gw(0,0,0,0); WiFi.config(ip, gw, mask); }

    // ... print IP Address
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    SendCode(BIORB_GREEN);
  }
  else
  {
    Serial.println("Can not connect to WiFi station. Go into AP mode.");

    // Go into software AP mode.
    WiFi.mode(WIFI_AP);

    delay(10);

    WiFi.softAP((const char *)hostname.c_str(), ap_default_psk);

    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());
    SendCode(BIORB_RED);
  }

  /*
      Initialize file system
  */
  SPIFFS.begin();                           // Start the SPI Flash Files System
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }

  /*
      Initialize HTTP Service
  */

  if (!MDNS.begin((const char *)hostname.c_str(), WiFi.localIP())) {
    Serial.println("Error setting up MDNS responder!");
    return;
  }
  Serial.println("mDNS responder started");

  //SERVER INIT
  //list directory
  httpServer.on("/list", HTTP_GET, handleFileList);
  //load editor
  httpServer.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) httpServer.send(404, "text/plain", "FileNotFound");
  });
  //create file
  httpServer.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  httpServer.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  httpServer.on("/edit", HTTP_POST, []() {
    httpServer.send(200, "text/plain", "");
  }, handleFileUpload);

  //called when the url is not defined here
  //use it to load content from SPIFFS
  httpServer.onNotFound([]() {
    if (!handleFileRead(httpServer.uri()))
      httpServer.send(404, "text/plain", "FileNotFound");
  });

  httpServer.begin();
  Serial.println("HTTP server started");
}

void HTTPHandleRequest(void)
{
  httpServer.handleClient();
}

