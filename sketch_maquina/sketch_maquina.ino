// Protótipos

void (*MaquinaStep)(void);

void StepInicial(void);
void StepUm(void);

// Funções

void StepInicial(void)
{
  MaquinaStep = StepUm;
}

void StepUm(void)
{
  MaquinaStep = StepInicial;
}

void setup()
{
  MaquinaStep = StepInicial;
}

void loop()
{
  (*MaquinaStep)();
}
