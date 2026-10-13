/*
 * BENCHMARK MULTIPLATAFORMA - ARDUINO (EJECUCIÓN)
 * Materia: Microprocesadores
 * 

 */

#include <Arduino.h>

// ROM de 16 bytes para la prueba RLE
const uint8_t DATA_ROM[16] = {
    0x41, 0x41, 0x41, 0x41, // 'A' x 4
    0x42, 0x42,             // 'B' x 2
    0x43,                   // 'C' x 1
    0x44, 0x44, 0x44, 0x44, // 'D' x 4
    0x45, 0x45, 0x45, 0x45, // 'E' x 4
    0x46                    // 'F' x 1
};

// Variables globales de acumulación (Anti Dead-Code Elimination)
volatile uint32_t acc_int = 0;
volatile float acc_float = 0.0f;

void ejecutar_benchmark_secuencial();

void setup() {
    Serial.begin(115200);
    while (!Serial);

    Serial.println(F("\n=================================================="));
    Serial.println(F("   BENCHMARK MULTIPLATAFORMA - SUITE AUTOMÁTICA   "));
    Serial.println(F("==================================================\n"));
    Serial.println(F("Iniciando la secuencia de pruebas en 3 segundos...\n"));
    delay(3000);

    ejecutar_benchmark_secuencial();
}

void loop() {
    // La suite se ejecuta una sola vez al arrancar
}

void ejecutar_benchmark_secuencial() {

    // -------------------------------------------------------------------
    // 1. SUMA DE ENTEROS DE 32 BITS
    // -------------------------------------------------------------------
    Serial.println(F("--- [1/8] PRUEBA: SUMA ENTERA (32 BITS) ---"));
    acc_int = 0;
    unsigned long t0 = micros();
    for (uint32_t i = 1; i <= 1000000UL; i++) acc_int += i;
    unsigned long t1 = micros();
    
    float t_add_sec = (t1 - t0) / 1000000.0;
    float ops_sec_add = 1000000.0 / t_add_sec;

    Serial.println(F("1. Carga Procesada:   1,000,000 operaciones enteras"));
    Serial.print(F("   - Checksum Anti-DCE: 0x")); Serial.println(acc_int, HEX);
    Serial.print(F("2. Tiempo Total:      ")); Serial.print(t_add_sec * 1000.0, 2); Serial.print(F(" ms (")); Serial.print(t_add_sec, 3); Serial.println(F(" s)"));
    Serial.print(F("3. Metrica Final:     ")); Serial.print(ops_sec_add, 0); Serial.print(F(" ops/seg (")); Serial.print(ops_sec_add / 1e6, 4); Serial.println(F(" MOPS)\n"));

    // -------------------------------------------------------------------
    // 2. MULTIPLICACIÓN DE ENTEROS DE 32 BITS
    // -------------------------------------------------------------------
    Serial.println(F("--- [2/8] PRUEBA: MULTIPLICACION ENTERA (32 BITS) ---"));
    acc_int = 0;
    uint32_t mul_b = 12345;
    t0 = micros();
    for (uint32_t i = 1; i <= 1000000UL; i++) {
        uint32_t prod = i * mul_b;
        acc_int ^= prod;
        mul_b += 3;
    }
    t1 = micros();

    float t_mul_sec = (t1 - t0) / 1000000.0;
    float ops_sec_mul = 1000000.0 / t_mul_sec;

    Serial.println(F("1. Carga Procesada:   1,000,000 operaciones enteras"));
    Serial.print(F("   - Checksum Anti-DCE: 0x")); Serial.println(acc_int, HEX);
    Serial.print(F("2. Tiempo Total:      ")); Serial.print(t_mul_sec * 1000.0, 2); Serial.print(F(" ms (")); Serial.print(t_mul_sec, 3); Serial.println(F(" s)"));
    Serial.print(F("3. Metrica Final:     ")); Serial.print(ops_sec_mul, 0); Serial.print(F(" ops/seg (")); Serial.print(ops_sec_mul / 1e6, 4); Serial.println(F(" MOPS)\n"));

    // -------------------------------------------------------------------
    // 3. DIVISIÓN DE ENTEROS DE 32 BITS
    // -------------------------------------------------------------------
    Serial.println(F("--- [3/8] PRUEBA: DIVISION ENTERA (32 BITS) ---"));
    acc_int = 0;
    const uint32_t K_DIV = 7;
    t0 = micros();
    for (uint32_t i = 1; i <= 1000000UL; i++) acc_int += (i / K_DIV);
    t1 = micros();

    float t_div_sec = (t1 - t0) / 1000000.0;
    float ops_sec_div = 1000000.0 / t_div_sec;

    Serial.println(F("1. Carga Procesada:   1,000,000 operaciones enteras"));
    Serial.print(F("   - Checksum Anti-DCE: 0x")); Serial.println(acc_int, HEX);
    Serial.print(F("2. Tiempo Total:      ")); Serial.print(t_div_sec * 1000.0, 2); Serial.print(F(" ms (")); Serial.print(t_div_sec, 3); Serial.println(F(" s)"));
    Serial.print(F("3. Metrica Final:     ")); Serial.print(ops_sec_div, 0); Serial.print(F(" ops/seg (")); Serial.print(ops_sec_div / 1e6, 4); Serial.println(F(" MOPS)\n"));

    // -------------------------------------------------------------------
    // 4. SUMA FLOTANTE (FLOAT 32 BITS)
    // -------------------------------------------------------------------
    Serial.println(F("--- [4/8] PRUEBA: SUMA PUNTO FLOTANTE (FLOAT) ---"));
    acc_float = 0.0f;
    t0 = micros();
    for (uint32_t i = 1; i <= 1000000UL; i++) acc_float += (float)i;
    t1 = micros();

    float t_fadd_sec = (t1 - t0) / 1000000.0;
    float flops_add = 1000000.0 / t_fadd_sec;

    Serial.println(F("1. Carga Procesada:   1,000,000 operaciones flotantes"));
    Serial.print(F("   - Acumulador Anti-DCE: ")); Serial.println(acc_float, 4);
    Serial.print(F("2. Tiempo Total:      ")); Serial.print(t_fadd_sec * 1000.0, 2); Serial.print(F(" ms (")); Serial.print(t_fadd_sec, 3); Serial.println(F(" s)"));
    Serial.print(F("3. Metrica Final:     ")); Serial.print(flops_add, 0); Serial.print(F(" FLOPS/s (")); Serial.print(flops_add / 1e6, 4); Serial.println(F(" MFLOPS)\n"));

    // -------------------------------------------------------------------
    // 5. MULTIPLICACIÓN FLOTANTE (FLOAT 32 BITS)
    // -------------------------------------------------------------------
    Serial.println(F("--- [5/8] PRUEBA: MULTIPLICACION PUNTO FLOTANTE (FLOAT) ---"));
    acc_float = 1.0f;
    t0 = micros();
    for (uint32_t i = 1; i <= 1000000UL; i++) acc_float += ((float)i * 1.00001f);
    t1 = micros();

    float t_fmul_sec = (t1 - t0) / 1000000.0;
    float flops_mul = 1000000.0 / t_fmul_sec;

    Serial.println(F("1. Carga Procesada:   1,000,000 operaciones flotantes"));
    Serial.print(F("   - Acumulador Anti-DCE: ")); Serial.println(acc_float, 4);
    Serial.print(F("2. Tiempo Total:      ")); Serial.print(t_fmul_sec * 1000.0, 2); Serial.print(F(" ms (")); Serial.print(t_fmul_sec, 3); Serial.println(F(" s)"));
    Serial.print(F("3. Metrica Final:     ")); Serial.print(flops_mul, 0); Serial.print(F(" FLOPS/s (")); Serial.print(flops_mul / 1e6, 4); Serial.println(F(" MFLOPS)\n"));

    // -------------------------------------------------------------------
    // 6. DIVISIÓN FLOTANTE (FLOAT 32 BITS)
    // -------------------------------------------------------------------
    Serial.println(F("--- [6/8] PRUEBA: DIVISION PUNTO FLOTANTE (FLOAT) ---"));
    acc_float = 1.0f;
    t0 = micros();
    for (uint32_t i = 1; i <= 1000000UL; i++) acc_float += ((float)i / 3.14159f);
    t1 = micros();

    float t_fdiv_sec = (t1 - t0) / 1000000.0;
    float flops_div = 1000000.0 / t_fdiv_sec;

    Serial.println(F("1. Carga Procesada:   1,000,000 operaciones flotantes"));
    Serial.print(F("   - Acumulador Anti-DCE: ")); Serial.println(acc_float, 4);
    Serial.print(F("2. Tiempo Total:      ")); Serial.print(t_fdiv_sec * 1000.0, 2); Serial.print(F(" ms (")); Serial.print(t_fdiv_sec, 3); Serial.println(F(" s)"));
    Serial.print(F("3. Metrica Final:     ")); Serial.print(flops_div, 0); Serial.print(F(" FLOPS/s (")); Serial.print(flops_div / 1e6, 4); Serial.println(F(" MFLOPS)\n"));

    // -------------------------------------------------------------------
    // 7. EVALUACIÓN DE NÚMEROS PRIMOS (2 A 60,000)
    // -------------------------------------------------------------------
    Serial.println(F("--- [7/8] PRUEBA: EVALUACION DE PRIMOS (2 A 60,000) ---"));
    uint16_t total_primos = 0;
    t0 = millis();

    for (uint16_t N = 2; N <= 60000; N++) {
        uint16_t D = 2;
        while (true) {
            uint16_t rem = N % D;
            if (rem == 0) {
                if (N == D) total_primos++;
                break;
            } else {
                if ((uint32_t)D * D >= N) {
                    total_primos++;
                    break;
                } else {
                    D++;
                }
            }
        }
        if (N == 60000) break;
    }
    t1 = millis();

    float t_primos_sec = (t1 - t0) / 1000.0;
    float primos_sec = total_primos / t_primos_sec;

    Serial.println(F("1. Carga Procesada:   59,999 numeros evaluados (Rango 2-60,000)"));
    Serial.print(F("   - Primos Hallados:  ")); Serial.print(total_primos); Serial.println(F(" / 6057"));
    Serial.print(F("2. Tiempo Total:      ")); Serial.print(t_primos_sec * 1000.0, 2); Serial.print(F(" ms (")); Serial.print(t_primos_sec, 3); Serial.println(F(" s)"));
    Serial.print(F("3. Metrica Final:     ")); Serial.print(primos_sec, 2); Serial.println(F(" primos/seg\n"));

    // -------------------------------------------------------------------
    // 8. COMPRESIÓN RLE (10,000 PASADAS)
    // -------------------------------------------------------------------
    Serial.println(F("--- [8/8] PRUEBA: COMPRESION RLE (10,000 PASADAS) ---"));
    acc_int = 0;
    t0 = micros();

    for (uint16_t pass = 0; pass <= 10000; pass++) {
        uint8_t rle_run = 1;
        for (int idx = 0; idx < 15; idx++) {
            if (DATA_ROM[idx] == DATA_ROM[idx + 1]) {
                rle_run++;
            } else {
                acc_int += (DATA_ROM[idx] ^ rle_run);
                rle_run = 1;
            }
        }
        acc_int += (DATA_ROM[15] ^ rle_run);
    }
    t1 = micros();

    float t_rle_sec = (t1 - t0) / 1000000.0;
    float bytes_totales = 16.0 * 10001.0;
    float throughput_mb = bytes_totales / (1024.0 * 1024.0) / t_rle_sec;

    Serial.println(F("1. Carga Procesada:   10,001 pasadas sobre ROM (160,016 B)"));
    Serial.print(F("   - Checksum Anti-DCE: 0x")); Serial.println(acc_int, HEX);
    Serial.print(F("2. Tiempo Total:      ")); Serial.print(t_rle_sec * 1000.0, 2); Serial.print(F(" ms (")); Serial.print(t_rle_sec, 3); Serial.println(F(" s)"));
    Serial.print(F("3. Metrica Final:     ")); Serial.print(throughput_mb, 2); Serial.println(F(" MB/s\n"));

    Serial.println(F("=================================================="));
    Serial.println(F("          SUITE AUTOMÁTICA FINALIZADA             "));
    Serial.println(F("=================================================="));
}