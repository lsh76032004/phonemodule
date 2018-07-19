int fd = 0;
String str;

// the setup routine runs once when you press reset:
void setup()
{
    // initialize the digital pin as an output.
    Serial.begin(19200);
}

// the loop routine runs over and over again forever:
void loop()
{

    switch (fd)
    {
    case 0:
        fd = 1;
        str = String('0') + String(analogRead(A0), DEC);
        break;
    case 1:
        fd = 2;
        str = String('1') + String(analogRead(A1), DEC);
        break;
    case 2:
        fd = 3;
        str = String('2') + String(analogRead(A2), DEC);
        break;
    case 3:
        fd = 4;
        str = String('3') + String(analogRead(A3), DEC);
        break;
    case 4:
        fd = 0;
        str = String('4') + String(analogRead(A4), DEC);
        break;
    default:
        fd = 0;
        break;
    }
    Serial.print(str);

    Serial.flush();
    delay(100);
}
