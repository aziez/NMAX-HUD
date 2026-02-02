#include "NetworkManager.h"

// ==========================================
static OTAData* _otaDataPtr = nullptr;

// HTML & CSS & JS (Tetap Sama seperti sebelumnya)
const char* style = 
"<style>"
"body{font-family:sans-serif;background:#3498db;color:#555;text-align:center;padding-top:50px}"
"form{background:#fff;max-width:300px;margin:0 auto;padding:30px;border-radius:8px;box-shadow:0 4px 10px rgba(0,0,0,0.1)}"
"input{width:100%;box-sizing:border-box;height:40px;margin:10px 0;border:1px solid #ddd;border-radius:4px;padding:0 10px}"
".btn{background:#2ecc71;color:white;border:none;cursor:pointer;font-weight:bold;transition:0.3s}"
".btn:hover{background:#27ae60}"
"#bar_container{background:#eee;border-radius:5px;height:20px;margin-top:20px;overflow:hidden;display:none}"
"#bar{background:#3498db;width:0%;height:100%;transition:width 0.2s}"
"#status{margin-top:10px;font-size:14px}"
"</style>";

const char* loginIndex = 
"<form name=loginForm>"
"<h2>NMAX HUD</h2>"
"<input name=userid placeholder='User ID'> "
"<input name=pwd placeholder=Password type=Password> "
"<input type=submit onclick=check(this.form) class=btn value='LOGIN'></form>"
"<script>"
"function check(form) {"
"if(form.userid.value=='admin' && form.pwd.value=='admin')"
"{window.location.href = '/serverIndex'}"
"else {alert('Wrong Password!')}"
"return false;"
"}"
"</script>";

const char* serverIndex = 
"<form id='upload_form' enctype='multipart/form-data'>"
"<h2>Firmware Update</h2>"
"<input type='file' name='update' id='file' accept='.bin' required>"
"<input type='submit' class=btn value='UPDATE FIRMWARE'>"
"<div id='bar_container'><div id='bar'></div></div>"
"<div id='status'></div>"
"</form>"
"<script>"
"var form = document.getElementById('upload_form');"
"form.onsubmit = function(event) {"
"  event.preventDefault();"
"  var fileInput = document.getElementById('file');"
"  if(fileInput.files.length === 0){ alert('Select a file!'); return; }"
"  var data = new FormData();"
"  data.append('update', fileInput.files[0]);"
"  var xhr = new XMLHttpRequest();"
"  xhr.upload.addEventListener('progress', function(e) {"
"    var percent = Math.round((e.loaded / e.total) * 100);"
"    document.getElementById('bar_container').style.display = 'block';"
"    document.getElementById('bar').style.width = percent + '%';"
"    document.getElementById('status').innerText = 'Uploading: ' + percent + '%';"
"  });"
"  xhr.onreadystatechange = function() {"
"    if (xhr.readyState == 4) {"
"      if(xhr.status == 200) {"
"        document.getElementById('status').innerText = 'Success! Rebooting...';"
"        document.getElementById('bar').style.backgroundColor = '#2ecc71';"
"      } else {"
"        document.getElementById('status').innerText = 'Failed! Check Serial Monitor.';"
"        document.getElementById('bar').style.backgroundColor = '#e74c3c';"
"      }"
"    }"
"  };"
"  xhr.open('POST', '/update');"
"  xhr.send(data);"
"};"
"</script>";

// ==========================================
// IMPLEMENTATION
// ==========================================

NetworkManager::NetworkManager() : server(80) {
    _isActive = false;
    _progressCallback = nullptr; // Init
}

void NetworkManager::setProgressCallback(OtaCallback cb) {
    _progressCallback = cb;
}

void NetworkManager::begin() {
    server.on("/", HTTP_GET, [this]() {
        server.send(200, "text/html", String(style) + String(loginIndex));
    });

    server.on("/serverIndex", HTTP_GET, [this]() {
        server.send(200, "text/html", String(style) + String(serverIndex));
    });

    server.on("/update", HTTP_POST, [this]() {
        bool success = !Update.hasError();
        server.send(200, "text/plain", success ? "OK" : "FAIL");
        if (success) { delay(500); ESP.restart(); }
    }, [this]() {
        HTTPUpload& upload = server.upload();
        
        if (upload.status == UPLOAD_FILE_START) {
            Serial.printf("Update Start: %s\n", upload.filename.c_str());
            
            // Panggil Callback
            if (_progressCallback) _progressCallback(0, "Preparing...");

            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { 
                Update.printError(Serial);
            }
        } 
        else if (upload.status == UPLOAD_FILE_WRITE) {
            if (!Update.hasError()) {
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                    Update.printError(Serial);
                }
            }
            
            // --- FIX RUMUS PROGRESS ---
            int progress = 0;
            if (upload.totalSize > 0) {
                // Gunakan Update.progress() (Bytes written) dan upload.totalSize
                progress = (Update.progress() * 100) / upload.totalSize;
            } else {
                static int fake = 0; fake = (fake + 2) % 100;
                progress = fake;
            }

            // Update Struct Global
            if (_otaDataPtr) {
                _otaDataPtr->progress = progress;
                _otaDataPtr->status = "Uploading...";
                _otaDataPtr->isUpdating = true;
            }

            // Panggil Callback
            if (_progressCallback) {
                _progressCallback(progress, "Writing Flash...");
            }
            
            yield(); 
        } 
        else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) { 
                if (_progressCallback) _progressCallback(100, "Success! Rebooting...");
            } else {
                if (_progressCallback) _progressCallback(0, "Error Occurred!");
            }
        }
    });
}
void NetworkManager::startAP() {
    if (_isActive) return;
    WiFi.mode(WIFI_AP);
    String mac = WiFi.macAddress();
    String ssid = "NMAX_HUD_" + mac.substring(12, 14) + mac.substring(15, 17);
    WiFi.softAP(ssid.c_str(), "12345678"); 
    if (MDNS.begin("nmax")) Serial.println("MDNS started");
    server.begin();
    _isActive = true;
    bootTime = millis();
}

void NetworkManager::stopAP() {
    if (!_isActive) return;
    server.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    _isActive = false;
}

void NetworkManager::handle() {
    if (_isActive) server.handleClient();
}

void NetworkManager::updateUI(OTAData* data) {
    if (!_isActive) return;
    _otaDataPtr = data;
    if (data->ssid == "") {
        String mac = WiFi.macAddress();
        data->ssid = "NMAX_HUD_" + mac.substring(12, 14) + mac.substring(15, 17);
        data->ip = WiFi.softAPIP().toString();
    }
    if (!data->isUpdating && millis() - bootTime > 300000) stopAP();
}

bool NetworkManager::isActive() { return _isActive; }