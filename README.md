# 🛴 PS4 Trottinette – Embedded Energy Transfer System (2024–2025)

Willkommen im offiziellen GitHub-Repository unseres Projekts im Rahmen des PS4-Moduls an der HEIA-FR.  
Unser Ziel ist es, eine **mechanisch-menschlich angetriebene Trottinette** mit einem **STM32-gesteuerten Energiespeicher- und Hebesystem** zu realisieren.

## 🔧 Projektziel

Im Rahmen des interdisziplinären Wettbewerbs GM-GE entwickeln wir:
- eine **selbstgebaute Trottinette**, die mechanisch angetrieben wird und elektrische Energie in Superkondensatoren speichert
- ein **autonomes Hebesystem**, das mithilfe dieser Energie eine **10 kg Masse auf bis zu 1.75 m** heben kann

Die gesamte Steuerungselektronik basiert auf einem **STM32-Mikrocontroller** (STM32CubeIDE), inklusive:
- ADC-Messung von Spannung
- Energieübertragung
- DC-Motorsteuerung
- Benutzerinteraktion per Tasten
- Safety-Handling und Automatisierung

## 📦 Projektstruktur

```bash
├── Core/                # Main application code (STM32)
├── Drivers/             # HAL drivers
├── PS4_Trottinette.ioc  # STM32CubeMX config file
├── .gitignore
├── README.md
└── ...


⚙️ Technische Komponenten
Modul	Beschreibung
🧠 Mikrocontroller	STM32 (z. B. WB55)
⚡ Energiespeicher	4x Superkondensatoren à 140 F, max 2.5 V
🔋 Energiequelle	Menschlich erzeugt (über DC-Motor als Generator)
🎯 Aktuator	DC-Motor zum Heben der Masse
🔌 Verbindung	Manuell beim Stop im Standbereich
⬆️ Levage-Automatik	Einmal gestartet → läuft ohne Eingriff
📏 Sensorik	Spannungserfassung via ADC, optional Position über Timer/Encoder
🎮 Interaktion	3 Tasten: Start Hebung, Absenken, Not-Stopp

🚀 Wettbewerb – Drei Phasen
Efficience
→ Höhe + Genauigkeit der Schätzung (nach 2 Runden)

Répétabilité
→ Zielhöhe zwischen 25–75 cm, möglichst genau treffen

Finale
→ Möglichst viele Hebungen auf ≥1.25–1.70 m in 5 Minuten

📚 Anforderungen laut Reglement
Kein gekaufter Trottinette-Rahmen

Energie nur menschlich erzeugt

Kein Gegen- oder Rückholgewicht

Hebung rein über elektrische Energie aus der Trottinette

Elektronik darf eigene 9 V Batterie nutzen

Hebung vollautomatisch nach Start, keine manuelle Hilfe

💡 Hinweise zur Entwicklung
STM32CubeIDE & STM32CubeMX zur Konfiguration

DMA empfohlen für Spannungserfassung

Code modular halten (z. B. motor_control.c, energy_transfer.c, ui_input.c)

Sicherheit (z. B. Über-/Unterspannung) beachten

🛠️ Build & Flash
Öffne das Projekt mit STM32CubeIDE

Stelle sicher, dass .ioc und Core/, Drivers/ korrekt generiert sind

Baue und flashe über ST-Link oder eingebauten Debugger

📄 Lizenz / Nutzung
Dieses Projekt wurde im Rahmen des PS4-Moduls der HEIA-FR (2024–2025) erstellt und dient ausschließlich der akademischen Arbeit und Bewertung.
