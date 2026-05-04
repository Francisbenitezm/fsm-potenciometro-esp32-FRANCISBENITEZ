#define LED_PIN 2
#define BOTON_PIN 4
#define POT_PIN 34

volatile int estado = 0;
volatile bool tickTimer = false;

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

int ultimoEstadoBoton = HIGH;

int contadorCambios = 0;
bool estadoLED = LOW;

void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
  tickTimer = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup()
{
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BOTON_PIN, INPUT_PULLUP);
  pinMode(POT_PIN, INPUT);

  analogWrite(LED_PIN, 0);

  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 1000000, true, 0);

  Serial.println("Estado 0: Standby");
}

void loop()
{
  int estadoBoton = digitalRead(BOTON_PIN);

  if (estado != 2)
  {
    if (ultimoEstadoBoton == HIGH && estadoBoton == LOW)
    {
      estado++;

      if (estado > 2)
      {
        estado = 0;
      }

      analogWrite(LED_PIN, 0);
      contadorCambios = 0;
      estadoLED = LOW;
      tickTimer = false;

      Serial.print("Cambio a Estado ");
      Serial.println(estado);

      if (estado == 2)
      {
        Serial.println("Inicia secuencia de parpadeo");
      }
    }
  }

  ultimoEstadoBoton = estadoBoton;

  if (estado == 0)
  {
    analogWrite(LED_PIN, 0);
  }

  else if (estado == 1)
  {
    int valorADC = analogRead(POT_PIN);
    int valorPWM = map(valorADC, 0, 4095, 0, 255);

    analogWrite(LED_PIN, valorPWM);

    Serial.print("ADC: ");
    Serial.print(valorADC);
    Serial.print(" | PWM: ");
    Serial.println(valorPWM);
  }

  else if (estado == 2)
  {
    if (tickTimer == true)
    {
      tickTimer = false;

      estadoLED = !estadoLED;

      if (estadoLED == HIGH)
      {
        analogWrite(LED_PIN, 255);
      }
      else
      {
        analogWrite(LED_PIN, 0);
      }

      contadorCambios++;

      Serial.print("Cambio LED: ");
      Serial.println(contadorCambios);

      if (contadorCambios >= 10)
      {
        analogWrite(LED_PIN, 0);
        estadoLED = LOW;
        contadorCambios = 0;
        estado = 0;

        Serial.println("Secuencia terminada. Regresa a Estado 0");
      }
    }
  }
}
