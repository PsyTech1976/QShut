# QShut - Linux Shutdown & Timer GUI

<p align="center">
  <img src="resources/qshutdown.svg" width="128" height="128" alt="QShut Logo" />
</p>

<p align="center">
  <strong>Applicazione grafica moderna per Linux in C++ e Qt 6 per lo spegnimento, riavvio e sospensione programmata del sistema.</strong><br>
  <em>Modern Linux GUI application in C++ and Qt 6 for scheduled system shutdown, restart, and suspend.</em>
</p>

---

## 🇮🇹 INDICE (Italiano - Lingua Principale)
1. [Panoramica](#panoramica)
2. [Funzionalità Principali](#funzionalità-principali)
3. [Guida all'Uso per l'Utente](#guida-alluso-per-lutente)
   - [Pianificazione a Conto alla Rovescia](#1-pianificazione-a-conto-alla-rovescia)
   - [Pianificazione a Orario Specifico](#2-pianificazione-a-orario-specifico)
   - [Scelta dell'Azione](#3-scelta-dellazione)
   - [Barra di Sistema (System Tray) e Notifiche](#4-barra-di-sistema-system-tray-e-notifiche)
   - [Avviso di Sicurezza a 60 Secondi](#5-avviso-di-sicurezza-a-60-secondi)
   - [Selezione della Lingua](#6-selezione-della-lingua)
   - [Guida Utente Integrata](#7-guida-utente-integrata)
   - [Modalità Simulazione (Test)](#8-modalità-simulazione-test)
4. [Opzioni da Riga di Comando](#opzioni-da-riga-di-comando)
5. [Come Usare il Pacchetto AppImage](#come-usare-il-pacchetto-appimage)
6. [Compilazione da Sorgente](#compilazione-da-sorgente)
7. [Rigenerazione dell'AppImage](#rigenerazione-dellappimage)

---

## 🇬🇧 TABLE OF CONTENTS (English - In Coda)
1. [Overview](#overview)
2. [Key Features](#key-features)
3. [User-Level Guide](#user-level-guide)
4. [Command-Line Options](#command-line-options)
5. [Using the AppImage](#using-the-appimage)
6. [Building from Source](#building-from-source)
7. [Rebuilding the AppImage](#rebuilding-the-appimage)

---

# 🇮🇹 Sezione in Italiano

## Panoramica

**QShut** è un'utilità desktop open-source per distribuzioni Linux (compatibile sia con sessioni **X11** che **Wayland**) progettata per consentire la pianificazione immediata dello spegnimento, riavvio o messa in standby del computer attraverso una GUI intuitiva.

A differenza di molti strumenti tradizionali da terminale, QShut:
- **Non richiede password di amministratore (`sudo` o `root`)**: sfrutta l'interfaccia standard D-Bus `systemd-logind` (`org.freedesktop.login1`), autorizzata dalla sessione utente locale.
- **Supporta 5 lingue complete**: Italiano, Inglese, Francese, Tedesco e Spagnolo.
- **È distribuibile come AppImage standalone**: nessun bisogno di installare librerie esterne.

---

## Funzionalità Principali

- **Supporto Multilingua Dinamico**: Rileva la lingua del sistema operativo al primo avvio (default Inglese in assenza o lingua non supportata). La lingua può essere commutata istantaneamente tramite menu a popup senza riavviare.
- **Guida Utente HTML Separata**: Finestra dedicata richiamabile dal pulsante *Guida* con documentazione completa e selettore di lingua integrato.
- **Due Modalità Temporali**:
  - *Conto alla rovescia*: minuti personalizzabili e pulsanti rapidi (+15m, +30m, +1h, +2h).
  - *Orario preciso*: orario nel formato `HH:mm`, con calcolo automatico per oggi o per domani.
- **Azioni Disponibili**: Spegnimento (*Power Off*), Riavvio (*Reboot*) e Sospensione (*Sleep / Suspend*).
- **Monitoraggio in Tempo Reale**: Display con conto alla rovescia in formato `HH:MM:SS` ad alta visibilità e barra di avanzamento percentuale.
- **Integrazione Vassoio di Sistema**: Riduzione nella tray icon con tooltip del tempo residuo.
- **Avviso di Sicurezza a 60 Secondi**: Notifica desktop e finestra riportata in primo piano per consentire l'annullamento rapido in caso di ripensamento.
- **Modalità Simulazione (Test)**: Esecuzione del timer senza inviare alcun comando di spegnimento effettivo alla macchina.

---

## Guida all'Uso per l'Utente

### 1. Pianificazione a Conto alla Rovescia
1. Seleziona la scheda **"Conto alla Rovescia"**.
2. Imposta i minuti desiderati tramite il selettore numerico oppure clicca sui pulsanti veloci:
   - `+15m` : aggiunge 15 minuti
   - `+30m` : aggiunge 30 minuti
   - `+1h` : aggiunge 60 minuti
   - `+2h` : aggiunge 120 minuti
3. Premi **"Avvia Timer"**.

### 2. Pianificazione a Orario Specifico
1. Seleziona la scheda **"Orario Specifico"**.
2. Imposta l'orario di destinazione (es. `23:45`).
3. Il programma indicherà chiaramente sotto l'orario se l'evento è programmato per **oggi** o per **domani** e quante ore/minuti mancano.
4. Premi **"Avvia Timer"**.

### 3. Scelta dell'Azione
Tramite il menu a tendina "Azione" puoi scegliere l'operazione desiderata:
- **Spegni il computer** (predefinito)
- **Riavvia il computer**
- **Sospendi (Sleep)**

### 4. Barra di Sistema (System Tray) e Notifiche
- Con la casella *"Riduci a icona nella barra di sistema"* attiva, la finestra si nasconderà non appena viene avviato il timer.
- Posizionando il cursore sull'icona di QShut nel vassoio di sistema puoi visualizzare il tempo rimanente aggiornato al secondo.
- Facendo clic sull'icona o cliccando con il tasto destro -> *"Mostra Finestra"* puoi riaprire la finestra principale.

### 5. Avviso di Sicurezza a 60 Secondi
Quando il timer raggiunge gli ultimi **60 secondi**:
- Viene inviata una notifica desktop di avviso.
- La finestra principale ritorna automaticamente visibile in primo piano.
- Puoi cliccare su **"Annulla"** in qualunque istante per interrompere la procedura.

### 6. Selezione della Lingua
Nella parte superiore della finestra è presente il pulsante con la bandiera della lingua attuale:
1. Clicca sul pulsante per aprire il menu a popup.
2. Scegli tra:
   - 🇬🇧 **English**
   - 🇮🇹 **Italiano**
   - 🇫🇷 **Français**
   - 🇩🇪 **Deutsch**
   - 🇪🇸 **Español**
3. L'interfaccia si aggiornerà immediatamente. La scelta viene salvata per i successivi avvii.

### 7. Guida Utente Integrata
Cliccando sul pulsante **"Guida"** nella testata si apre una finestra separata con la guida HTML completa. È possibile cambiare lingua della guida in qualsiasi momento tramite il menu dedicato in alto a destra nella finestra di aiuto.

### 8. Modalità Simulazione (Test)
Selezionando la casella *"Modalità simulazione (Test)"*, puoi verificare il corretto funzionamento del conto alla rovescia e delle notifiche senza rischiare di arrestare o riavviare il computer.

---

## Opzioni da Riga di Comando

L'eseguibile e l'AppImage accettano parametri da terminale:

```bash
# Avvia in modalità simulazione / test
./QShut-x86_64.AppImage --test

# Mostra le opzioni disponibili
./QShut-x86_64.AppImage --help

# Mostra la versione installata
./QShut-x86_64.AppImage --version
```

---

## Come Usare il Pacchetto AppImage

Il pacchetto AppImage include tutte le dipendenze Qt6 e i moduli per X11 e Wayland.

```bash
# Assegna i permessi di esecuzione
chmod +x QShut-x86_64.AppImage

# Avvia l'applicazione
./QShut-x86_64.AppImage
```

---

## Compilazione da Sorgente

### Requisiti di sistema
- Compilatore C++17 (`g++` o `clang++`)
- `cmake` (>= 3.16)
- Librerie Qt 6: `Core`, `Gui`, `Widgets`, `DBus`

### Istruzioni di compilazione:
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)

# Esecuzione del binario compilato:
./qshutdown
```

---

## Rigenerazione dell'AppImage

È incluso lo script `build_appimage.sh` pronto all'uso:

```bash
./build_appimage.sh
```

Lo script compila il progetto in modalità Release, prepara la struttura `AppDir`, include i plugin Qt necessari e produce il file `QShut-x86_64.AppImage`.

---
---

# 🇬🇧 English Section

## Overview

**QShut** is a modern, lightweight open-source desktop application for Linux systems (fully compatible with both **X11** and **Wayland** sessions) designed to easily schedule computer shutdown, restart, or sleep via an intuitive graphical user interface.

Key advantages over standard command-line tools:
- **No administrator password required (`sudo` or `root`)**: integrates directly with the standard `systemd-logind` D-Bus interface (`org.freedesktop.login1`).
- **Full multilingual support**: English, Italian, French, German, and Spanish.
- **Standalone AppImage package**: runs on any modern Linux distribution without extra dependencies.

---

## Key Features

- **Dynamic Multilingual Support**: Automatically detects the system's language on startup (defaults to English if unsupported). Switch languages on-the-fly via a top popup menu without restarting.
- **Separate HTML User Guide**: Dedicated help window reachable via the *Guide* button with complete documentation and built-in language selector.
- **Dual Timing Modes**:
  - *Countdown*: custom minutes selector with quick increment buttons (+15m, +30m, +1h, +2h).
  - *Specific Time*: 24-hour time picker (`HH:mm`), automatically calculating whether it falls today or tomorrow.
- **System Actions**: Shut down (*Power Off*), Restart (*Reboot*), and Sleep (*Suspend*).
- **Live Visual Feedback**: High-visibility `HH:MM:SS` timer display and percentage progress bar.
- **System Tray Integration**: Automatically minimizes to the tray with real-time remaining countdown tooltip.
- **60-Second Safety Alert**: Desktop notification and automatic window foregrounding 60 seconds before execution, allowing quick cancellation.
- **Simulation Mode (Test)**: Run the countdown and alerts without actually performing any power action.

---

## User-Level Guide

### 1. Countdown Mode
1. Click the **"Countdown"** tab.
2. Enter the target minutes or click the shortcut buttons:
   - `+15m` : adds 15 minutes
   - `+30m` : adds 30 minutes
   - `+1h` : adds 60 minutes
   - `+2h` : adds 120 minutes
3. Click **"Start Timer"**.

### 2. Specific Time Mode
1. Click the **"Specific Time"** tab.
2. Select the desired time (e.g., `23:45`).
3. The info label below indicates whether the schedule applies to **today** or **tomorrow**, along with remaining hours and minutes.
4. Click **"Start Timer"**.

### 3. Choosing the Action
Use the "Action" drop-down menu to select:
- **Shut down computer** (default)
- **Restart computer**
- **Suspend (Sleep)**

### 4. System Tray & Notifications
- When *"Minimize to system tray"* is enabled, the main window hides as soon as the timer begins.
- Hover over the tray icon to check the live countdown.
- Left-click the icon or right-click -> *"Show Window"* to restore the interface.

### 5. 60-Second Safety Warning
When the remaining timer reaches **60 seconds**:
- A desktop notification alert appears.
- The window is brought back to the foreground.
- Click **"Cancel"** at any time to abort the operation.

### 6. Language Selection
Click the language flag button in the upper right header:
1. Select from:
   - 🇬🇧 **English**
   - 🇮🇹 **Italiano**
   - 🇫🇷 **Français**
   - 🇩🇪 **Deutsch**
   - 🇪🇸 **Español**
2. The UI switches immediately and saves your choice across sessions.

### 7. Integrated User Guide
Click the **"Guide"** button in the header to view the illustrated HTML guide in a separate window. You can change the guide language anytime via the top-right combo box.

### 8. Simulation Mode (Test)
Check *"Simulation mode (Test)"* to test the timer and notifications safely without turning off your machine.

---

## Command-Line Options

```bash
# Launch with test/simulation mode pre-enabled
./QShut-x86_64.AppImage --test

# Display available options
./QShut-x86_64.AppImage --help

# Show version
./QShut-x86_64.AppImage --version
```

---

## Using the AppImage

```bash
# Make the file executable
chmod +x QShut-x86_64.AppImage

# Run the application
./QShut-x86_64.AppImage
```

---

## Building from Source

### Prerequisites
- C++17 compiler (`g++` or `clang++`)
- `cmake` (>= 3.16)
- Qt 6 libraries: `Core`, `Gui`, `Widgets`, `DBus`

### Build steps:
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)

# Run binary:
./qshutdown
```

---

## Rebuilding the AppImage

```bash
./build_appimage.sh
```

---

## License

Released under the **MIT License**. Free to use, modify, and distribute.
