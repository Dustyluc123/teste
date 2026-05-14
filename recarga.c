/*
 * recarga.c — Simulador de Sessão de Recarga de Veículo Elétrico
 *
 * Lógica geral:
 *   1. Coleta dados do usuário (nome, tipo de usuário, horário de início, duração desejada).
 *   2. Valida cada entrada com laços do-while, rejeitando valores inválidos.
 *   3. Simula a passagem do tempo de recarga com um laço for, acumulando
 *      energia consumida por minuto (taxa variável conforme horário de pico).
 *   4. Calcula o custo total aplicando tarifas diferenciadas:
 *        • Horário de pico  (18h–22h): tarifa mais alta
 *        • Horário noturno  (22h–06h): tarifa reduzida
 *        • Demais horários             : tarifa padrão
 *        • Desconto para usuários Premium.
 *   5. Exibe relatório formatado ao final da sessão.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ── Constantes de tarifa (R$/kWh) ───────────────────────────────────────── */
#define TARIFA_PADRAO  0.75
#define TARIFA_PICO    1.20
#define TARIFA_NOTURNA 0.45

/* Taxa de carga simulada (kWh por minuto) — valor fictício para demonstração */
#define KWH_POR_MINUTO 0.10

/* Desconto concedido a usuários Premium (percentual) */
#define DESCONTO_PREMIUM 15.0

/* Largura da linha decorativa no relatório */
#define LARGURA 52

/* ── Tipos de usuário ─────────────────────────────────────────────────────── */
typedef enum {
    USUARIO_PADRAO  = 1,
    USUARIO_PREMIUM = 2
} TipoUsuario;

/* ── Estrutura que armazena os dados de uma sessão ───────────────────────── */
typedef struct {
    char       nome[64];
    TipoUsuario tipo;
    int        hora_inicio;   /* 0–23 */
    int        duracao_min;   /* minutos de recarga */
    double     energia_kwh;   /* energia total consumida */
    double     custo_bruto;   /* custo antes do desconto */
    double     desconto;      /* valor do desconto (R$) */
    double     custo_final;   /* custo a pagar */
} Sessao;

/* ── Protótipos ───────────────────────────────────────────────────────────── */
static void   imprimir_linha(char c);
static void   ler_nome(char *dest, int tamanho);
static int    ler_inteiro(const char *prompt, int minimo, int maximo);
static TipoUsuario ler_tipo_usuario(void);
static double obter_tarifa(int hora);
static void   simular_recarga(Sessao *s);
static void   calcular_custo(Sessao *s);
static void   exibir_relatorio(const Sessao *s);

/* ═══════════════════════════════════════════════════════════════════════════ */
int main(void)
{
    Sessao sessao;

    /* Cabeçalho */
    imprimir_linha('=');
    printf("   SIMULADOR DE SESSÃO DE RECARGA — ELETROCHARGE\n");
    imprimir_linha('=');
    printf("\n");

    /* 1. Coleta de dados -------------------------------------------------- */
    ler_nome(sessao.nome, sizeof(sessao.nome));
    sessao.tipo        = ler_tipo_usuario();
    sessao.hora_inicio = ler_inteiro("Hora de início da recarga (0–23): ", 0, 23);
    sessao.duracao_min = ler_inteiro("Duração desejada em minutos (1–480): ", 1, 480);

    printf("\n");
    imprimir_linha('-');
    printf("  Iniciando sessão de recarga…\n");
    imprimir_linha('-');

    /* 2. Simulação e cálculo ---------------------------------------------- */
    simular_recarga(&sessao);
    calcular_custo(&sessao);

    /* 3. Relatório -------------------------------------------------------- */
    exibir_relatorio(&sessao);

    return 0;
}

/* ── Imprime uma linha decorativa de LARGURA caracteres ─────────────────── */
static void imprimir_linha(char c)
{
    int i;
    for (i = 0; i < LARGURA; i++) {
        putchar(c);
    }
    putchar('\n');
}

/* ── Lê o nome do usuário, rejeitando entrada vazia ─────────────────────── */
static void ler_nome(char *dest, int tamanho)
{
    int valido = 0;
    do {
        printf("Nome do usuário: ");
        if (fgets(dest, tamanho, stdin) == NULL) {
            dest[0] = '\0';
        }
        /* Remove newline */
        dest[strcspn(dest, "\n")] = '\0';

        if (strlen(dest) == 0) {
            printf("  [ERRO] Nome não pode ser vazio. Tente novamente.\n");
        } else {
            valido = 1;
        }
    } while (!valido);
}

/* ── Lê um inteiro dentro do intervalo [minimo, maximo] ─────────────────── */
static int ler_inteiro(const char *prompt, int minimo, int maximo)
{
    int valor;
    int leituras;
    int valido = 0;

    do {
        printf("%s", prompt);
        leituras = scanf("%d", &valor);

        /* Descarta resto da linha (incluindo '\n') */
        while (getchar() != '\n');

        if (leituras != 1 || valor < minimo || valor > maximo) {
            printf("  [ERRO] Valor inválido. Digite um número entre %d e %d.\n",
                   minimo, maximo);
        } else {
            valido = 1;
        }
    } while (!valido);

    return valor;
}

/* ── Lê o tipo de usuário via menu switch ────────────────────────────────── */
static TipoUsuario ler_tipo_usuario(void)
{
    int opcao;
    int valido = 0;

    printf("\nTipo de usuário:\n");
    printf("  [1] Padrão\n");
    printf("  [2] Premium (%.0f%% de desconto)\n", DESCONTO_PREMIUM);

    do {
        printf("Escolha (1 ou 2): ");
        if (scanf("%d", &opcao) != 1) {
            opcao = 0;
        }
        while (getchar() != '\n');

        switch (opcao) {
            case 1:
            case 2:
                valido = 1;
                break;
            default:
                printf("  [ERRO] Opção inválida. Digite 1 ou 2.\n");
        }
    } while (!valido);

    return (TipoUsuario)opcao;
}

/* ── Retorna a tarifa vigente de acordo com o horário ────────────────────── */
static double obter_tarifa(int hora)
{
    /* Horário de pico: 18h a 21h59 */
    if (hora >= 18 && hora < 22) {
        return TARIFA_PICO;
    }

    /* Horário noturno: 22h a 05h59 */
    if (hora >= 22 || hora < 6) {
        return TARIFA_NOTURNA;
    }

    /* Demais horários: tarifa padrão */
    return TARIFA_PADRAO;
}

/* ── Simula o progresso da recarga minuto a minuto ───────────────────────── */
static void simular_recarga(Sessao *s)
{
    int    minuto;
    double energia_minuto;
    int    hora_atual;
    double tarifa_minuto;

    s->energia_kwh = 0.0;
    s->custo_bruto  = 0.0;

    printf("\n  Progresso da recarga:\n");

    /* Calcula o passo para exibir ~10 atualizações de progresso */
    int passo = s->duracao_min / 10;
    if (passo < 1) passo = 1;

    for (minuto = 1; minuto <= s->duracao_min; minuto++) {
        /* Hora atual: usa (minuto-1) para offset baseado em zero */
        hora_atual    = (s->hora_inicio + (minuto - 1) / 60) % 24;
        tarifa_minuto = obter_tarifa(hora_atual);
        energia_minuto = KWH_POR_MINUTO;

        s->energia_kwh += energia_minuto;
        s->custo_bruto  += energia_minuto * tarifa_minuto;

        /* Exibe barra de progresso a cada ~10% (ou no último minuto) */
        if (minuto % passo == 0 || minuto == s->duracao_min) {
            int pct = (minuto * 100) / s->duracao_min;
            int barras = pct / 5;
            int i;

            printf("  [");
            for (i = 0; i < 20; i++) {
                putchar(i < barras ? '#' : '.');
            }
            printf("] %3d%%  %.2f kWh\r", pct, s->energia_kwh);
            fflush(stdout);
        }
    }

    printf("\n");
}

/* ── Calcula desconto e custo final ─────────────────────────────────────── */
static void calcular_custo(Sessao *s)
{
    if (s->tipo == USUARIO_PREMIUM) {
        s->desconto    = s->custo_bruto * (DESCONTO_PREMIUM / 100.0);
    } else {
        s->desconto    = 0.0;
    }

    s->custo_final = s->custo_bruto - s->desconto;
}

/* ── Determina o nome textual da faixa tarifária ─────────────────────────── */
/* Nota: os valores abaixo devem corresponder às constantes TARIFA_* do topo. */
static const char *nome_tarifa(int hora)
{
    if (hora >= 18 && hora < 22) return "Pico (R$ 1,20/kWh)";    /* TARIFA_PICO    */
    if (hora >= 22 || hora < 6)  return "Noturna (R$ 0,45/kWh)"; /* TARIFA_NOTURNA */
    return "Padrão (R$ 0,75/kWh)";                                /* TARIFA_PADRAO  */
}

/* ── Exibe o relatório final da sessão ───────────────────────────────────── */
static void exibir_relatorio(const Sessao *s)
{
    int hora_fim = (s->hora_inicio + s->duracao_min / 60) % 24;
    int min_fim  = s->duracao_min % 60;

    printf("\n");
    imprimir_linha('=');
    printf("              RELATÓRIO DA SESSÃO\n");
    imprimir_linha('=');

    printf("  Usuário      : %s\n", s->nome);
    printf("  Perfil       : %s\n",
           s->tipo == USUARIO_PREMIUM ? "Premium" : "Padrão");
    imprimir_linha('-');

    printf("  Início       : %02d:00\n", s->hora_inicio);
    printf("  Duração      : %d min  (término aprox. %02d:%02d)\n",
           s->duracao_min, hora_fim, min_fim);
    printf("  Faixa tarifária inicial : %s\n", nome_tarifa(s->hora_inicio));
    imprimir_linha('-');

    printf("  Energia carregada : %.3f kWh\n", s->energia_kwh);
    printf("  Custo bruto       : R$ %.2f\n",  s->custo_bruto);

    if (s->tipo == USUARIO_PREMIUM) {
        printf("  Desconto Premium  : R$ %.2f  (%.0f%%)\n",
               s->desconto, DESCONTO_PREMIUM);
    }

    printf("  TOTAL A PAGAR     : R$ %.2f\n", s->custo_final);
    imprimir_linha('=');

    /* Mensagem de encerramento */
    if (s->custo_final < 10.0) {
        printf("  Sessão econômica! Bom proveito.\n");
    } else if (s->custo_final < 50.0) {
        printf("  Sessão concluída com sucesso.\n");
    } else {
        printf("  Sessão longa. Considere recargas parciais.\n");
    }

    imprimir_linha('=');
    printf("\n");
}
