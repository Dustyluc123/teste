# Simulador de Sessão de Recarga — Sprint 1

Programa em C que simula uma sessão completa de recarga de veículo elétrico,
calculando o custo com base no horário e no perfil do usuário.

---

## Compilação e execução

**Pré-requisito:** gcc instalado.

```bash
# Compilar
make

# Executar
./recarga

# Remover binário
make clean
```

Compilação manual (sem Make):

```bash
gcc -Wall -Wextra -pedantic -std=c99 -o recarga recarga.c
./recarga
```

---

## Lógica do programa

### 1. Coleta e validação de dados (`do-while`)

O programa solicita ao usuário:

| Campo            | Restrição                |
|------------------|--------------------------|
| Nome             | Não pode ser vazio       |
| Tipo de usuário  | 1 = Padrão, 2 = Premium  |
| Hora de início   | 0 a 23                   |
| Duração          | 1 a 480 minutos          |

Cada campo é lido em um laço `do-while` que só avança quando a entrada é válida.
O `switch` avalia a opção de tipo de usuário e rejeita valores fora de {1, 2}.

### 2. Simulação da recarga (`for`)

Um laço `for` itera minuto a minuto durante toda a duração solicitada.
A cada iteração:
- Calcula a hora atual (início + minutos decorridos) para determinar a tarifa vigente.
- Acumula energia (kWh) e custo parcial (R$).
- Exibe uma barra de progresso a cada ~10%.

### 3. Tarifação diferenciada (`if/else`)

| Faixa horária          | Tarifa (R$/kWh) |
|------------------------|-----------------|
| Pico      18h – 21h59  | R$ 1,20         |
| Noturno   22h – 05h59  | R$ 0,45         |
| Padrão    06h – 17h59  | R$ 0,75         |

Usuários **Premium** recebem 15 % de desconto sobre o custo total.

### 4. Relatório final

Ao término da sessão, o programa exibe:
- Nome e perfil do usuário
- Horário de início e duração
- Faixa tarifária inicial
- Energia carregada (kWh)
- Custo bruto, desconto (se aplicável) e **total a pagar**
- Mensagem de encerramento contextual

---

## Exemplo de saída

```
====================================================
   SIMULADOR DE SESSÃO DE RECARGA — ELETROCHARGE
====================================================

Nome do usuário: João Silva

Tipo de usuário:
  [1] Padrão
  [2] Premium (15% de desconto)
Escolha (1 ou 2): 2

Hora de início da recarga (0–23): 20
Duração desejada em minutos (1–480): 90

----------------------------------------------------
  Iniciando sessão de recarga…
----------------------------------------------------

  Progresso da recarga:
  [####################] 100%  9.00 kWh

====================================================
              RELATÓRIO DA SESSÃO
====================================================
  Usuário      : João Silva
  Perfil       : Premium
----------------------------------------------------
  Início       : 20:00
  Duração      : 90 min  (término aprox. 21:30)
  Faixa tarifária inicial : Pico (R$ 1,20/kWh)
----------------------------------------------------
  Energia carregada : 9.000 kWh
  Custo bruto       : R$ 10.80
  Desconto Premium  : R$ 1.62  (15%)
  TOTAL A PAGAR     : R$ 9.18
====================================================
  Sessão econômica! Bom proveito.
====================================================
```

---

## Estruturas utilizadas

| Requisito                          | Implementação                                    |
|------------------------------------|--------------------------------------------------|
| `if / else`                        | `obter_tarifa()`, `nome_tarifa()`, relatório      |
| `switch`                           | Validação do tipo de usuário em `ler_tipo_usuario()` |
| `for`                              | Simulação minuto a minuto em `simular_recarga()`  |
| `while`                            | Descarte do buffer de leitura (`while (getchar() != '\n')`) |
| `do-while`                         | Laços de validação em `ler_nome()`, `ler_inteiro()`, `ler_tipo_usuario()` |