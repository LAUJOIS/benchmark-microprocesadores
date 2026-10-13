/*
 * BENCHMARK MULTIPLATAFORMA - ESP32-S3
 * Materia: Microprocesadores
 * 


 * - Incluye FPU en Hardware (Single Precision 32-bit)
 * - Fijado explícitamente al Núcleo 1 mediante xTaskCreatePinnedToCore
 * - Medición de microsegundos con esp_timer_get_time()
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

// Prototipos de funciones
void ejecutar_benchmark_secuencial();
void tarea_benchmark(void *pvParameters);

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000); // Espera inicialización del puerto USB/Serial

    Serial.println(F("\n=================================================="));
    Serial.println(F("    BENCHMARK DE RENDIMIENTO - ESP32-S3 (240 MHz) "));
    Serial.println(F("==================================================\n"));

    // Se crea la tarea fijada explícitamente al Núcleo 1 (Core 1) según exigencia de la guía
    xTaskCreatePinnedToCore(
        tarea_benchmark,   // Función de la tarea
        "BenchmarkTask",   // Nombre de la tarea
        8192,              // Tamaño de stack (bytes)
        NULL,              // Parámetros
        1,                 // Prioridad
        NULL,              // Manejador
        1                  // Núcleo 1 (Core 1)
    );
}

void loop() {
    // El loop principal queda libre ya que el benchmark se ejecuta en su propia tarea en Core 1
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void tarea_benchmark(void *pvParameters) {
    Serial.println(F("Iniciando la secuencia de pruebas en 3 segundos...\n"));
    vTaskDelay(pdMS_TO_TICKS(3000));

    ejecutar_benchmark_secuencial();

    // Elimina la tarea de la memoria al finalizar la suite
    vTaskDelete(NULL);
}

void ejecutar_benchmark_secuencial() {

    // -------------------------------------------------------------------
    // 1. SUMA DE ENTEROS DE 32 BITS
    // -------------------------------------------------------------------
    Serial.println(F("--- [1/8] PRUEBA: SUMA ENTERA (32 BITS) ---"));
    acc_int = 0;
    int64_t t0 = esp_timer_get_time(); // Microsegundos de alta precisión
    for (uint32_t i = 1; i <= 1000000UL; i++) acc_int += i;
    int64_t t1 = esp_timer_get_time();
    
    float t_add_sec = (t1 - t0) / 1000000.0f;
    float ops_sec_add = 1000000.0f / t_add_sec;

    Serial.println(F("1. Carga Procesada:   1,000,000 operaciones enteras"));
    Serial.printf("   - Checksum Anti-DCE: 0x%X\n", acc_int);
    Serial.printf("2. Tiempo Total:      %.2f ms (%.4f s)\n", t_add_sec * 1000.0f, t_add_sec);
    Serial.printf("3. Metrica Final:     %.0f ops/seg (%.4f MOPS)\n\n", ops_sec_add, ops_sec_add / 1e6f);

    // -------------------------------------------------------------------
    // 2. MULTIPLICACIÓN DE ENTEROS DE 32 BITS
    // -------------------------------------------------------------------
    Serial.println(F("--- [2/8] PRUEBA: MULTIPLICACION ENTERA (32 BITS) ---"));
    acc_int = 0;
    uint32_t mul_b = 12345;
    t0 = esp_timer_get_time();
    for (uint32_t i = 1; i <= 1000000UL; i++) {
        uint32_t prod = i * mul_b;
        acc_int ^= prod;
        mul_b += 3;
    }
    t1 = esp_timer_get_time();

    float t_mul_sec = (t1 - t0) / 1000000.0f;
    float ops_sec_mul = 1000000.0f / t_mul_sec;

    Serial.println(F("1. Carga Procesada:   1,000,000 operaciones enteras"));
    Serial.printf("   - Checksum Anti-DCE: 0x%X\n", acc_int);
    Serial.printf("2. Tiempo Total:      %.2f ms (%.4f s)\n", t_mul_sec * 1000.0f, t_mul_sec);
    Serial.printf("3. Metrica Final:     %.0f ops/seg (%.4f MOPS)\n\n", ops_sec_mul, ops_sec_mul / 1e6f);

    // -------------------------------------------------------------------
    // 3. DIVISIÓN DE ENTEROS DE 32 BITS
    // -------------------------------------------------------------------
    Serial.println(F("--- [3/8] PRUEBA: DIVISION ENTERA (32 BITS) ---"));
    acc_int = 0;
    const uint32_t K_DIV = 7;
    t0 = esp_timer_get_time();
    for (uint32_t i = 1; i <= 1000000UL; i++) acc_int += (i / K_DIV);
    t1 = esp_timer_get_time();

    float t_div_sec = (t1 - t0) / 1000000.0f;
    float ops_sec_div = 1000000.0f / t_div_sec;

    Serial.println(F("1. Carga Procesada:   1,000,000 operaciones enteras"));
    Serial.printf("   - Checksum Anti-DCE: 0x%X\n", acc_int);
    Serial.printf("2. Tiempo Total:      %.2f ms (%.4f s)\n", t_div_sec * 1000.0f, t_div_sec);
    Serial.printf("3. Metrica Final:     %.0f ops/seg (%.4f MOPS)\n\n", ops_sec_div, ops_sec_div / 1e6f);

    // -------------------------------------------------------------------
    // 4. SUMA FLOTANTE (FLOAT 32 BITS - FPU HARDWARE)
    // -------------------------------------------------------------------
    Serial.println(F("--- [4/8] PRUEBA: SUMA PUNTO FLOTANTE (FPU HARDWARE) ---"));
    acc_float = 0.0f;
    t0 = esp_timer_get_time();
    for (uint32_t i = 1; i <= 1000000UL; i++) acc_float += (float)i;
    t1 = esp_timer_get_time();

    float t_fadd_sec = (t1 - t0) / 1000000.0f;
    float flops_add = 1000000.0f / t_fadd_sec;

    Serial.println(F("1. Carga Procesada:   1,000,000 operaciones flotantes"));
    Serial.printf("   - Acumulador Anti-DCE: %.4f\n", acc_float);
    Serial.printf("2. Tiempo Total:      %.2f ms (%.4f s)\n", t_fadd_sec * 1000.0f, t_fadd_sec);
    Serial.printf("3. Metrica Final:     %.0f FLOPS/s (%.4f MFLOPS)\n\n", flops_add, flops_add / 1e6f);

    // -------------------------------------------------------------------
    // 5. MULTIPLICACIÓN FLOTANTE (FLOAT 32 BITS - FPU HARDWARE)
    // -------------------------------------------------------------------
    Serial.println(F("--- [5/8] PRUEBA: MULTIPLICACION PUNTO FLOTANTE (FPU HARDWARE) ---"));
    acc_float = 1.0f;
    t0 = esp_timer_get_time();
    for (uint32_t i = 1; i <= 1000000UL; i++) acc_float += ((float)i * 1.00001f);
    t1 = esp_timer_get_time();

    float t_fmul_sec = (t1 - t0) / 1000000.0f;
    float flops_mul = 1000000.0f / t_fmul_sec;

    Serial.println(F("1. Carga Procesada:   1,000,000 operaciones flotantes"));
    Serial.printf("   - Acumulador Anti-DCE: %.4f\n", acc_float);
    Serial.printf("2. Tiempo Total:      %.2f ms (%.4f s)\n", t_fmul_sec * 1000.0f, t_fmul_sec);
    Serial.printf("3. Metrica Final:     %.0f FLOPS/s (%.4f MFLOPS)\n\n", flops_mul, flops_mul / 1e6f);

    // -------------------------------------------------------------------
    // 6. DIVISIÓN FLOTANTE (FLOAT 32 BITS - FPU HARDWARE)
    // -------------------------------------------------------------------
    Serial.println(F("--- [6/8] PRUEBA: DIVISION PUNTO FLOTANTE (FPU HARDWARE) ---"));
    acc_float = 1.0f;
    t0 = esp_timer_get_time();
    for (uint32_t i = 1; i <= 1000000UL; i++) acc_float += ((float)i / 3.14159f);
    t1 = esp_timer_get_time();

    float t_fdiv_sec = (t1 - t0) / 1000000.0f;
    float flops_div = 1000000.0f / t_fdiv_sec;

    Serial.println(F("1. Carga Procesada:   1,000,000 operaciones flotantes"));
    Serial.printf("   - Acumulador Anti-DCE: %.4f\n", acc_float);
    Serial.printf("2. Tiempo Total:      %.2f ms (%.4f s)\n", t_fdiv_sec * 1000.0f, t_fdiv_sec);
    Serial.printf("3. Metrica Final:     %.0f FLOPS/s (%.4f MFLOPS)\n\n", flops_div, flops_div / 1e6f);

    // -------------------------------------------------------------------
    // 7. EVALUACIÓN DE NÚMEROS PRIMOS (2 A 60,000)
    // -------------------------------------------------------------------
    Serial.println(F("--- [7/8] PRUEBA: EVALUACION DE PRIMOS (2 A 60,000) ---"));
    uint16_t total_primos = 0;
    t0 = esp_timer_get_time();

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
    t1 = esp_timer_get_time();

    float t_primos_sec = (t1 - t0) / 1000000.0f;
    float primos_sec = total_primos / t_primos_sec;

    Serial.println(F("1. Carga Procesada:   59,999 numeros evaluados (Rango 2-60,000)"));
    Serial.printf("   - Primos Hallados:  %d / 6057\n", total_primos);
    Serial.printf("2. Tiempo Total:      %.2f ms (%.4f s)\n", t_primos_sec * 1000.0f, t_primos_sec);
    Serial.printf("3. Metrica Final:     %.2f primos/seg\n\n", primos_sec);

    // -------------------------------------------------------------------
    // 8. COMPRESIÓN RLE (10,000 PASADAS)
    // -------------------------------------------------------------------
    Serial.println(F("--- [8/8] PRUEBA: COMPRESION RLE (10,000 PASADAS) ---"));
    acc_int = 0;
    t0 = esp_timer_get_time();

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
    t1 = esp_timer_get_time();

    float t_rle_sec = (t1 - t0) / 1000000.0f;
    float bytes_totales = 16.0f * 10001.0f;
    float throughput_mb = bytes_totales / (1024.0f * 1024.0f) / t_rle_sec;

    Serial.println(F("1. Carga Procesada:   10,001 pasadas sobre ROM (160,016 B)"));
    Serial.printf("   - Checksum Anti-DCE: 0x%X\n", acc_int);
    Serial.printf("2. Tiempo Total:      %.2f ms (%.4f s)\n", t_rle_sec * 1000.0f, t_rle_sec);
    Serial.printf("3. Metrica Final:     %.2f MB/s\n\n", throughput_mb);

    Serial.println(F("=================================================="));
    Serial.println(F("          SUITE AUTOMÁTICA FINALIZADA             "));
    Serial.println(F("=================================================="));
}