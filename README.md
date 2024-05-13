# ZMALA-Project

## Linking Modules

### ESP8266 A9G

    D1        ===>    TX
    D2        ===>    RX
    D4        ===>    Rest Pin

### ESP8266 Buzzer

    D5        ===>    Vcc
    Gnd       ===>    Gnd

## Buzzer Pattenrs with led indications

### Battery Status

- Everey 1sec(-- . --) ===> Low Battery( < 30%) ===> Red
- Everey 0.5sec(-- . --) ===> Very Low Battery( < 20%) ===> Orange
- Good Battery ===> Green

### Module Status And Location Status

- Every 0.5sec(-.-.-.-) ===> Oustside The Allowed Area ===> Yellow
- (0.5sec() .1sec) ===> Connected And ready ===> White
