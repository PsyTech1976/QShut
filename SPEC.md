# SPEC.md - Specifiche Tecniche e Architetturali di QShut
# Technical and Architectural Specifications of QShut

---

## 🇮🇹 INDICE (Italiano - Lingua Principale)
1. [Obiettivi e Scopo del Progetto](#1-obiettivi-e-scopo-del-progetto)
2. [Requisiti di Sistema e Dipendenze](#2-requisiti-di-sistema-e-dipendenze)
3. [Architettura del Software e Componenti](#3-architettura-del-software-e-componenti)
4. [Specifiche Funzionali e Logica Operativa](#4-specifiche-funzionali-e-logica-operativa)
   - [4.1 Pianificazione a Conto alla Rovescia](#41-pianificazione-a-conto-alla-rovescia)
   - [4.2 Pianificazione a Orario Specifico](#42-pianificazione-a-orario-specifico)
   - [4.3 Esecuzione delle Azioni di Sistema e Gestione Permessi](#43-esecuzione-delle-azioni-di-sistema-e-gestione-permessi)
   - [4.4 Barra di Sistema, Notifiche e Avviso a 60 Secondi](#44-barra-di-sistema-notifiche-e-avviso-a-60-secondi)
   - [4.5 Sistema di Internazionalizzazione (i18n)](#45-sistema-di-internazionalizzazione-i18n)
   - [4.6 Guida Utente HTML Integrata](#46-guida-utente-html-integrata)
   - [4.7 Modalità Simulazione (Test / Dry Run)](#47-modalità-simulazione-test--dry-run)
5. [Specifiche dell'Interfaccia Grafica (UI/UX)](#5-specifiche-dellinterfaccia-grafica-uiux)
6. [Specifiche di Compilazione e Packaging AppImage](#6-specifiche-di-compilazione-e-packaging-appimage)
7. [Specifiche di Sicurezza e Privacy](#7-specifiche-di-sicurezza-e-privacy)

---

## 🇬🇧 TABLE OF CONTENTS (English - In Coda)
1. [Project Goals & Scope](#1-project-goals--scope)
2. [System Requirements & Dependencies](#2-system-requirements--dependencies)
3. [Software Architecture & Components](#3-software-architecture--components)
4. [Functional Specifications & Runtime Logic](#4-functional-specifications--runtime-logic)
5. [User Interface Specifications (UI/UX)](#5-user-interface-specifications-uiux)
6. [Build & AppImage Packaging Specifications](#6-build--appimage-packaging-specifications)
7. [Security & Privacy Specifications](#7-security--privacy-specifications)

---

# 🇮🇹 Sezione in Italiano (Specifiche Tecniche)

## 1. Obiettivi e Scopo del Progetto

**QShut** è un'applicazione grafica desktop per ambienti Linux progettata per pianificare lo spegnimento, il riavvio o la sospensione del sistema tramite un'interfaccia intuitiva e moderna.

L'applicazione soddisfa i seguenti requisiti cardine:
- **Operatività non privilegiata**: Spegnimento del computer senza richiesta di credenziali amministrative (`sudo` o `root`), operando all'interno delle policy della sessione utente standard.
- **Portabilità e distribuzione autonoma**: Rilascio come pacchetto unico **AppImage** compatibile con le principali distribuzioni Linux (Arch, Ubuntu, Debian, Fedora, openSUSE) senza dipendenze esterne.
- **Supporto multi-ambiente**: Piena compatibilità con server grafici **X11** e compositori **Wayland** (KDE Plasma, GNOME, Sway, Hyprland, XFCE).
- **Multilingua nativo**: Interfaccia e documentazione disponibili in 5 lingue (Italiano, Inglese, Francese, Tedesco, Spagnolo).

---

## 2. Requisiti di Sistema e Dipendenze

### 2.1 Piattaforma Target
- **Sistema Operativo**: GNU/Linux (x86_64).
- **Init System**: systemd (richiesto per il backend D-Bus `systemd-logind`).
- **Server Display**: X11 o Wayland (XDG-Shell).

### 2.2 Stack Tecnologico
- **Linguaggio**: C++17 (ISO/IEC 14882:2017).
- **Framework GUI e Core**: Qt 6 (moduli `Core`, `Gui`, `Widgets`, `DBus`).
- **Build System**: CMake (>= 3.16).
- **Packaging**: linuxdeploy, linuxdeploy-plugin-qt, appimagetool.

---

## 3. Architettura del Software e Componenti

Il software segue un'architettura modulare a oggetti basata sul paradigma Signal/Slot di Qt:

```
+-------------------------------------------------------------+
|                         main.cpp                            |
|  - QApplication / Stile Fusion                              |
|  - QCommandLineParser (--test, --help, --version)           |
+------------------------------+------------------------------+
                               |
                               v
+-------------------------------------------------------------+
|                        MainWindow                           |
|  - Gestione interfaccia (QTabWidget, QSpinBox, QTimeEdit)   |
|  - QTimer (1000ms tick) & calcolo countdown                 |
|  - QSystemTrayIcon (menu contestuale e tooltip)             |
|  - Menu popup cambio lingua istantaneo                      |
|  - Dialogo Guida HTML (HelpDialog)                          |
+------------+--------------------+---------------------+-----+
             |                    |                     |
             v                    v                     v
   +-------------------+  +---------------+   +-------------------+
   |  ShutdownManager  |  |LanguageManager|   |    HelpDialog     |
   | - D-Bus logind    |  | - 5 lingue    |   | - QTextBrowser    |
   | - systemctl       |  | - Rileva locale|   | - Selettore lingua|
   | - shutdown cmd    |  | - QSettings   |   | - HTML da risorse |
   | - Dry Run test    |  | - Dizionario  |   +-------------------+
   +-------------------+  +---------------+
```

### Ruoli dei Moduli:
1. **`MainWindow`** (`src/MainWindow.h`, `src/MainWindow.cpp`):
   Finestra principale dell'applicazione. Riceve l'input dell'utente, gestisce i controlli, aggiorna i timer al secondo, controlla l'icona nel vassoio di sistema e smista gli eventi di arresto e annullamento.
2. **`ShutdownManager`** (`src/ShutdownManager.h`, `src/ShutdownManager.cpp`):
   Responsabile dell'esecuzione fisica del comando di spegnimento, riavvio o sospensione. Isola la logica di sistema dall'interfaccia grafica.
3. **`LanguageManager`** (`src/LanguageManager.h`, `src/LanguageManager.cpp`):
   Pattern Singleton che gestisce l'internazionalizzazione a runtime per le 5 lingue supportate, memorizzando la selezione in `QSettings`.
4. **`HelpDialog`** (`src/HelpDialog.h`, `src/HelpDialog.cpp`):
   Finestra di dialogo dedicata alla visualizzazione della guida utente formattata in HTML ricavata dalle risorse Qt (`:/help/guide_<lang>.html`).

---

## 4. Specifiche Funzionali e Logica Operativa

### 4.1 Pianificazione a Conto alla Rovescia
- Intervallo consentito: da 1 a 10080 minuti (fino a 7 giorni).
- Pulsanti rapidi:
  - `+15m`: incrementa il valore di 15 minuti.
  - `+30m`: incrementa il valore di 30 minuti.
  - `+1h`: incrementa il valore di 60 minuti.
  - `+2h`: incrementa il valore di 120 minuti.
- Formula calcolo target temporale:
  $$\text{TargetDateTime} = \text{CurrentDateTime} + (\text{Minuti} \times 60)$$

### 4.2 Pianificazione a Orario Specifico
- Selezione orario: widget `QTimeEdit` con formato 24 ore `HH:mm`.
- Logica di calcolo del giorno (Oggi / Domani):
  - Sia $T_{\text{adesso}}$ l'orario corrente e $T_{\text{scelto}}$ l'orario impostato dall'utente.
  - Se $T_{\text{scelto}} > T_{\text{adesso}}$: la data programmata è la data corrente (**oggi**).
  - Se $T_{\text{scelto}} \le T_{\text{adesso}}$: la data programmata è la data di domani (**domani**).
- Il tempo totale residuo in secondi è calcolato come:
  $$\text{TotalSeconds} = \text{CurrentDateTime.secsTo}(\text{TargetDateTime})$$

### 4.3 Esecuzione delle Azioni di Sistema e Gestione Permessi
Per garantire il funzionamento senza password, `ShutdownManager` implementa una catena di fallback a 3 livelli:

1. **Livello 1: D-Bus `systemd-logind` (Predefinito)**:
   - **Bus**: `QDBusConnection::systemBus()`
   - **Destinazione**: `org.freedesktop.login1`
   - **Path**: `/org/freedesktop/login1`
   - **Interfaccia**: `org.freedesktop.login1.Manager`
   - **Metodi invocati**:
     - Spegnimento: `PowerOff(bool interactive = true)`
     - Riavvio: `Reboot(bool interactive = true)`
     - Sospensione: `Suspend(bool interactive = true)`
   - Poiché le policy Polkit predefinite consentono agli utenti con sessione desktop attiva locale di gestire l'alimentazione, l'operazione ha esito positivo immediato senza richiesta di password di root.

2. **Livello 2: Fallback su `systemctl`**:
   - Invocazione asincrona disaccoppiata (`QProcess::startDetached`) di:
     - `systemctl poweroff`
     - `systemctl reboot`
     - `systemctl suspend`

3. **Livello 3: Fallback su comando `shutdown`**:
   - Invocazione di `shutdown -h now` (per spegnimento) o `shutdown -r now` (per riavvio).

### 4.4 Barra di Sistema, Notifiche e Avviso a 60 Secondi
- **Minimizzazione**: Se abilitata la casella *"Riduci a icona nella barra di sistema"*, la finestra principale viene nascosta (`hide()`) al click su *"Avvia Timer"*.
- **Icona Tray**: L'icona mostra in tempo reale nel tooltip il conteggio residuo `HH:MM:SS`.
- **Avviso di sicurezza a 60 secondi**:
  - Quando $\text{RemainingSeconds} \le 60$ e l'avviso non è stato ancora emesso:
    1. Viene inviata una notifica desktop di sistema ad alta priorità via tray icon.
    2. La finestra principale viene forzata in primo piano (`showNormal()` e `activateWindow()`).
    3. L'utente ha 60 secondi per cliccare sul pulsante **"Annulla"**.

### 4.5 Sistema di Internazionalizzazione (i18n)
- **Lingue supportate**:
  | Codice | Lingua | Icona / Flag |
  |---|---|---|
  | `en` | English | 🇬🇧 |
  | `it` | Italiano | 🇮🇹 |
  | `fr` | Français | 🇫🇷 |
  | `de` | Deutsch | 🇩🇪 |
  | `es` | Español | 🇪🇸 |
- **Rilevamento all'avvio**:
  1. Verifica se esiste una preferenza memorizzata in `QSettings("QShutProject", "qshutdown")`.
  2. Se assente, legge i primi due caratteri di `QLocale::system().name().toLower()`.
  3. Se coincide con `it`, `fr`, `de` o `es`, imposta tale lingua.
  4. Altrimenti, applica il fallback universale **`en`**.
- **Aggiornamento dinamico**: Ciascun cambio di lingua emette il segnale `languageChanged(code)`, scatenando il metodo `retranslateUi()` che riscrive labels, bottoni, tooltip, voci di menu e guide in tempo reale.

### 4.6 Guida Utente HTML Integrata
- Dialogo indipendente non modale `HelpDialog`.
- Carica documenti HTML nativi formattati con CSS responsive (`:/help/guide_<lang>.html`).
- Contiene un selettore di lingua integrato sincronizzato bidirezionalmente con `LanguageManager`.

### 4.7 Modalità Simulazione (Test / Dry Run)
- Flag `--test` o `-t` da riga di comando oppure casella nella GUI.
- Quando attiva, il timer esegue tutto il flusso (countdown, barra, notifiche, avviso a 60 secondi), ma allo scadere del tempo visualizza un messaggio di successo informativo senza inviare comandi di spegnimento alla macchina.

---

## 5. Specifiche dell'Interfaccia Grafica (UI/UX)

- **Dimensioni Finestra**: Minima 480x520 px, iniziale raccomandata 500x540 px.
- **Stile Applicato**: Qt `Fusion` style con palette adattiva (rispetta la combinazione scura/chiara del desktop dell'utente).
- **Tipografia**:
  - Display Countdown: 28pt grassetto monospazio per massima leggibilità a distanza.
  - Titoli: 15pt grassetto.
  - Pulsanti principali: 11pt grassetto con altezza minima 42px per facilità di click.
- **Palette Colori Pulsanti**:
  - Tasto Avvio: Verde accent (`#2b8a3e` normale, `#2f9e44` hover).
  - Tasto Annulla: Rosso warning (`#c92a2a` normale, `#e03131` hover).
- **Prevenzione chiusure accidentali**: Nel metodo `closeEvent()`, se un timer è in esecuzione, l'app intercetta la chiusura proponendo una finestra di dialogo per scegliere se minimizzare nella tray o annullare il timer.

---

## 6. Specifiche di Compilazione e Packaging AppImage

### 6.1 Struttura Cartelle di Build
```
build/
├── AppDir/
│   ├── AppRun                  (Script di bootstrap e setup env)
│   ├── qshutdown.desktop       (Metadati XDG)
│   ├── qshutdown.png           (Icona 256x256)
│   ├── qshutdown.svg           (Icona vettoriale)
│   └── usr/
│       ├── bin/qshutdown       (Binario ELF compilato)
│       ├── bin/qt.conf         (Configurazione percorsi Qt)
│       ├── lib/                (Librerie Qt e dipendenze condivise)
│       └── plugins/            (Plugin Qt platforms, imageformats, wayland)
└── QShut-x86_64.AppImage       (File finale distribuibile)
```

### 6.2 Plugin di Piattaforma Inclusi
- **X11**: `platforms/libqxcb.so`, `xcbglintegrations/libqxcb-glx-integration.so`, `libqxcb-egl-integration.so`.
- **Wayland**: `platforms/libqwayland.so`, `wayland-shell-integration/`, `wayland-graphics-integration-client/`.

### 6.3 Automazione `build_appimage.sh`
1. Compilazione Release via CMake (`-DCMAKE_BUILD_TYPE=Release`).
2. Isolamento dei plugin Qt per escludere moduli di terze parti non necessari.
3. Esecuzione di `linuxdeploy` con `NO_STRIP=true` (per compatibilità con sezioni ELF DT_RELR recenti).
4. Esecuzione del plugin `linuxdeploy-plugin-qt` per impacchettare i moduli core e wayland.
5. Creazione archivio squashfs e generazione del pacchetto `.AppImage` tramite `appimagetool`.

---

## 7. Specifiche di Sicurezza e Privacy

1. **Privilegi Utente**: L'applicazione viene eseguita interamente nello spazio utente non privilegiato (`UID > 1000`). Nessun file binario ha flag `setuid` né richiede elevazione di privilegi.
2. **Nessun dato personale memorizzato**:
   - I file di configurazione (`~/.config/QShutProject/qshutdown.conf`) contengono unicamente la chiave `language=<codice>`.
   - Nessun percorso personale o credenziale viene salvato o trasmesso.
3. **Sicurezza di Rete**: L'applicazione non esegue chiamate di rete esterne, non invia telemetria e opera al 100% offline.

---
---

# 🇬🇧 English Section (Technical Specifications)

## 1. Project Goals & Scope

**QShut** is a modern desktop utility for Linux environments designed to schedule computer shutdown, reboot, or suspend operations through an intuitive graphical interface.

Core requirements:
- **Unprivileged execution**: Triggers system power-off without administrative credentials (`sudo` or `root`), operating strictly within standard desktop session permissions.
- **Autonomous distribution**: Packaged as a self-contained **AppImage** compatible across major Linux distributions (Arch, Ubuntu, Debian, Fedora, openSUSE) with no external library dependencies.
- **Display Server Compatibility**: Native support for both **X11** and **Wayland** compositors (KDE Plasma, GNOME, Sway, Hyprland, XFCE).
- **Native Localization**: UI and user guides available in 5 languages (English, Italian, French, German, Spanish).

---

## 2. System Requirements & Dependencies

### 2.1 Target Platform
- **OS**: GNU/Linux (x86_64).
- **Init System**: systemd (required for `systemd-logind` D-Bus backend).
- **Display Server**: X11 or Wayland (XDG-Shell).

### 2.2 Technology Stack
- **Language**: C++17 (ISO/IEC 14882:2017).
- **GUI & Core Framework**: Qt 6 (`Core`, `Gui`, `Widgets`, `DBus`).
- **Build System**: CMake (>= 3.16).
- **Packaging Tools**: linuxdeploy, linuxdeploy-plugin-qt, appimagetool.

---

## 3. Software Architecture & Components

The software is structured around modular C++ classes leveraging Qt's Signal/Slot mechanism:

- **`MainWindow`**: Central controller managing the GUI, countdown timer ticks (1000ms), tray icon status, language selection popup, and guide window.
- **`ShutdownManager`**: Backend subsystem abstracting operating system interaction, D-Bus invocations, fallback mechanisms, and dry-run execution.
- **`LanguageManager`**: Singleton service handling string dictionaries across 5 languages, detecting operating system locale, and persisting user preferences via `QSettings`.
- **`HelpDialog`**: Independent dialog presenting styled HTML documentation loaded from Qt internal resources (`:/help/guide_<lang>.html`).

---

## 4. Functional Specifications & Runtime Logic

### 4.1 Countdown Scheduling
- Range: 1 to 10080 minutes (up to 7 days).
- Increment shortcuts: `+15m`, `+30m`, `+1h`, `+2h`.
- Calculation: $\text{TargetDateTime} = \text{CurrentDateTime} + (\text{Minutes} \times 60)$.

### 4.2 Specific Time Scheduling
- 24-hour time selector widget (`HH:mm`).
- Logic:
  - If user time is later than current time: schedule for **today**.
  - If user time is earlier than or equal to current time: schedule for **tomorrow**.

### 4.3 Action Execution & Authorization
1. **Tier 1 - D-Bus `systemd-logind` (Default)**:
   - Interface: `org.freedesktop.login1.Manager` on `/org/freedesktop/login1`.
   - Calls `PowerOff(true)`, `Reboot(true)`, or `Suspend(true)`.
   - Authorized automatically by system polkit rules for active local desktop sessions.
2. **Tier 2 - Fallback to `systemctl`**: Detached execution of `systemctl poweroff|reboot|suspend`.
3. **Tier 3 - Fallback to `shutdown`**: Detached execution of `shutdown -h now` or `shutdown -r now`.

### 4.4 System Tray & 60-Second Warning
- When minimized to tray, live remaining time is reflected in the tray tooltip.
- At $\le 60$ seconds remaining:
  1. A high-priority desktop notification is triggered.
  2. The window is restored and brought to the foreground.
  3. The user can abort the process by clicking **"Cancel"**.

### 4.5 Localization Architecture
- Supports `en`, `it`, `fr`, `de`, `es`.
- Auto-detects system locale via `QLocale::system()`; defaults to English if unsupported.
- Dynamic runtime switching without restarting the process.

### 4.6 Simulation Mode (Dry Run)
- Triggered by `--test` flag or UI checkbox.
- Simulates complete countdown and notifications without firing actual system power commands.

---

## 5. User Interface Specifications (UI/UX)

- Window geometry: Minimum 480x520 px, default 500x540 px.
- Visual style: Qt `Fusion` with adaptive system palette.
- High-contrast 28pt bold countdown label and progress bar.
- Close-interception safeguards against accidental termination during active countdowns.

---

## 6. Build & AppImage Packaging Specifications

- Builds with CMake in Release configuration.
- Bundles Qt 6 Core, Gui, Widgets, DBus, SVG, and X11/Wayland platform plugins.
- Built AppImage contains an embedded `AppRun` script managing dynamic linker paths and plugin environment variables.

---

## 7. Security & Privacy Specifications

- Operates entirely with unprivileged user permissions (`UID > 1000`).
- No hardcoded paths, user credentials, or telemetry.
- 100% offline functionality.
