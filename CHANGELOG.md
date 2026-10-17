# Changelog

Tutte le modifiche importanti al progetto `MultiFTPServer` vengono documentate in questo file.

## [3.0.3] - 2026-10-17
### Correzioni di bug, conformità RFC e miglioramenti prestazionali (con `+0 byte` di overhead SRAM su AVR):

- **Supporto RFC 2428 `EPSV` (Extended Passive Mode)** ([SimpleFTPServer#99](https://github.com/xreef/SimpleFTPServer/pull/99)):
  - Aggiunto il comando `EPSV` in `processCommand()` e l'annuncio `EPSV` nella risposta `FEAT`.
  - Aggiunta la guardia `if (dataServer.status() == CLOSED)` per `NETWORK_WiFiNINA` su `PASV`/`EPSV` per evitare l'esaurimento dei socket hardware sul modulo NINA-W102.
- **Gestione scritture TCP parziali in `doRetrieve()`** ([SimpleFTPServer#100](https://github.com/xreef/SimpleFTPServer/pull/100)):
  - Riposizionamento del puntatore file (`file.seek(bytesTransfered + written)`) quando `data.write(buf, nb)` invia meno byte di quelli letti dallo storage in condizioni di backpressure TCP.
- **Fix callback `FTP_TRANSFER_STOP` e protezione overflow in `readChar()`** ([SimpleFTPServer#98](https://github.com/xreef/SimpleFTPServer/pull/98)):
  - Catturato il nome del file prima di `file.close()` in `closeTransfer()` affinché la callback `FTP_TRANSFER_STOP` riceva sempre il nome corretto del file.
  - Corretto il controllo dei limiti in `readChar()` (`iCL < FTP_CMD_SIZE - 1`) per impedire che il terminatore null `'\0'` sovrascriva il primo byte di `cwdName[FTP_CWD_SIZE]`.
- **Supporto apostrofi nei nomi di file** ([SimpleFTPServer#96](https://github.com/xreef/SimpleFTPServer/issues/96)):
  - Corretto `readChar()` sostituendo `if (c == '\'')` con `if (c == '\\')`, normalizzando solo i backslash Windows in `/` e preservando gli apostrofi nei nomi di file e directory.
- **Rimozione warning `WiFiClient::flush()` deprecato su ESP32 Core 3.3.7+** ([SimpleFTPServer#94](https://github.com/xreef/SimpleFTPServer/issues/94)):
  - Rimossi i `data.flush()` ridondanti dopo `data.stop()` in `STOR`/`APPE` e protetto `data.flush()` con `#if defined(ESP8266)` in `doRetrieve()` e `closeTransfer()`.
- **Fix `isDir()` e comando `DELE` con client FTP avanzati (SpeedCommander, FileZilla, WinSCP)** ([SimpleFTPServer#92](https://github.com/xreef/SimpleFTPServer/issues/92)):
  - Corretto `FtpServer::isDir()` su tutti i backend (`STORAGE_SD`, `STORAGE_LITTLEFS`, `STORAGE_FFAT`, `STORAGE_SD_MMC`, `STORAGE_SDFAT1`/`2`) affinché verifichi `isDirectory()`/`isDir()` e chiuda sempre l'handle temporaneo senza lasciare aperto `this->file`.
  - Anticipato l'invio di `250 Deleted <file>` prima della callback `FTP_FREE_SPACE_CHANGE` nel comando `DELE`.
- **Fix comando `APPE` (Append) su ESP32 `STORAGE_SD` e `STORAGE_SD_MMC`** ([SimpleFTPServer#90](https://github.com/xreef/SimpleFTPServer/issues/90)):
  - Preservata la modalità `"a"` (`FILE_APPEND`) in `FtpServer::openFile(char path[], const char* readType)` invece di sovrascriverla con `"w"` (`FILE_WRITE`).
- **Fix `getFileModTime()`, `NLST` e conformità RFC 3659 per `MLSD` / `MLST`**:
  - Corretto `getFileModTime(uint16_t*, uint16_t*)` su `STORAGE_SD` e ESP32 VFS per evitare chiamate su `dir` chiuso e garantire il funzionamento di `MLST`.
  - Ripristinato in `doMlsd()` il formato standard RFC 3659 (`Type=file|dir;Modify=...;Size=...; <name>`) su tutti i backend di storage e corretto `NLST` in `doList()` su `SdFat` affinché invii solo il nome del file.
- **Eliminazione dei blocchi (15–70s) su `SdFat` e fix disconnessione in `disconnectClient()` / `QUIT`** ([SimpleFTPServer#93](https://github.com/xreef/SimpleFTPServer/issues/93)):
  - Passato `0, 0` alla callback su `FTP_CONNECT` e `FTP_DISCONNECT` senza interrogare il filesystem.
  - Su `STORAGE_SDFAT1` e `STORAGE_SDFAT2`, evitata la chiamata bloccante a `freeClusterCount()` quando `MAINTAIN_FREE_CLUSTER_COUNT` è disabilitato (`0` di default in `SdFat`).
  - Aggiunto `client.stop()` mancante in `disconnectClient()`, chiusi `client` e `data` prima della callback ed eliminato il doppio invio di `221 Goodbye` su `QUIT`.
- **Supporto completo `Arduino Nano 33 IoT` (`SAMD21 + WiFiNINA`) ed esempi aggiornati** ([SimpleFTPServer#90](https://github.com/xreef/SimpleFTPServer/issues/90), [SimpleFTPServer#91](https://github.com/xreef/SimpleFTPServer/issues/91)):
  - Separati i blocchi `#ifndef DEFAULT_STORAGE_TYPE_*` e `#ifndef DEFAULT_FTP_SERVER_NETWORK_TYPE_*` in `FtpServerKey.h` (e corretto `DEFAULT_FTP_SERVER_NETWORK_TYPE_SAMD`).
  - Aggiunti e aggiornati gli esempi `Arduino_WiFiNINA_SD`, `Arduino_Mega_W5500_SD`, `Arduino_Mega_W5500_SdFat2`, `ESP32_Dynamic_SD_Mount`, `ESP32_Dynamic_SD_Mount_SdFat` e tutti gli sketch di esempio della libreria.

---

## [3.0.2] - 2025-11-28
- Fix `#elif defined` per ESP32 ed ESP8266 in `FtpServer.cpp` ([SimpleFTPServer#88](https://github.com/xreef/SimpleFTPServer/issues/88)).
- Fix su `STORAGE_SPIFM` `getName`.

## [3.0.1] - 2025-11-24
- Fix: Gestione corretta delle modalità `FILE_READ` / `FILE_WRITE` e overload di `openFile` su diverse piattaforme.
- Fix: Protezione `#ifndef ESP32` applicata per gli overload numerici di `openFile`.
- Esempio aggiunto: `ESP32_SD_SdFat2` e `ESP32_SD_WiFi`.

## [3.0.0] - 2025-10-13
- Migliorata la gestione dell'apertura dei file e del posizionamento del puntatore.
- Maggiore robustezza nella connessione dati passiva e supporto `FFat` con fallback POSIX.
- Controllo dello spazio libero configurabile prima di accettare upload e callback FTP.
