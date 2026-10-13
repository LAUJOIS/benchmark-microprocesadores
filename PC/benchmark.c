/*
 * BENCHMARK MULTIPLATAFORMA - PC (x86) / RASPBERRY PI (ARM)
 * Materia: Microprocesadores
 * 
 * - Mide: Aritmética Entera, Punto Flotante, Primos y Compresión RLE
 * - Temporización: clock_gettime(CLOCK_MONOTONIC)
 * - Anti Dead-Code Elimination: Variables volatile y checksums finales
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

// ROM de 16 bytes para la prueba RLE
static const uint8_t DATA_ROM[16] = {
    0x41, 0x41, 0x41, 0x41, // 'A' x 4
    0x42, 0x42,             // 'B' x 2
    0x43,                   // 'C' x 1
    0x44, 0x44, 0x44, 0x44, // 'D' x 4
    0x45, 0x45, 0x45, 0x45, // 'E' x 4
    0x46                    // 'F' x 1
};

// Variables globales acumuladoras (Anti-DCE)
volatile uint32_t acc_int = 0;
volatile float acc_float = 0.0f;

// Función para obtener tiempo actual en segundos con precisión de nanosegundos
double get_time_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

void ejecutar_benchmark_secuencial(void) {
    double t0, t1;

    // -------------------------------------------------------------------
    // 1. SUMA DE ENTEROS DE 32 BITS
    // -------------------------------------------------------------------
    printf("--- [1/8] PRUEBA: SUMA ENTERA (32 BITS) ---\n");
    acc_int = 0;
    t0 = get_time_sec();
    for (uint32_t i = 1; i <= 1000000UL; i++) {
        acc_int += i;
    }
    t1 = get_time_sec();

    double t_add_sec = t1 - t0;
    double ops_sec_add = 1000000.0 / t_add_sec;

    printf("1. Carga Procesada:   1,000,000 operaciones enteras\n");
    printf("   - Checksum Anti-DCE: 0x%X\n", acc_int);
    printf("2. Tiempo Total:      %.3f ms (%.6f s)\n", t_add_sec * 1000.0, t_add_sec);
    printf("3. Metrica Final:     %.0f ops/seg (%.4f MOPS)\n\n", ops_sec_add, ops_sec_add / 1e6);

    // -------------------------------------------------------------------
    // 2. MULTIPLICACIÓN DE ENTEROS DE 32 BITS
    // -------------------------------------------------------------------
    printf("--- [2/8] PRUEBA: MULTIPLICACION ENTERA (32 BITS) ---\n");
    acc_int = 0;
    uint32_t mul_b = 12345;
    t0 = get_time_sec();
    for (uint32_t i = 1; i <= 1000000UL; i++) {
        uint32_t prod = i * mul_b;
        acc_int ^= prod;
        mul_b += 3;
    }
    t1 = get_time_sec();

    double t_mul_sec = t1 - t0;
    double ops_sec_mul = 1000000.0 / t_mul_sec;

    printf("1. Carga Procesada:   1,000,000 operaciones enteras\n");
    printf("   - Checksum Anti-DCE: 0x%X\n", acc_int);
    printf("2. Tiempo Total:      %.3f ms (%.6f s)\n", t_mul_sec * 1000.0, t_mul_sec);
    printf("3. Metrica Final:     %.0f ops/seg (%.4f MOPS)\n\n", ops_sec_mul, ops_sec_mul / 1e6);

    // -------------------------------------------------------------------
    // 3. DIVISIÓN DE ENTEROS DE 32 BITS
    // -------------------------------------------------------------------
    printf("--- [3/8] PRUEBA: DIVISION ENTERA (32 BITS) ---\n");
    acc_int = 0;
    const uint32_t K_DIV = 7;
    t0 = get_time_sec();
    for (uint32_t i = 1; i <= 1000000UL; i++) {
        acc_int += (i / K_DIV);
    }
    t1 = get_time_sec();

    double t_div_sec = t1 - t0;
    double ops_sec_div = 1000000.0 / t_div_sec;

    printf("1. Carga Procesada:   1,000,000 operaciones enteras\n");
    printf("   - Checksum Anti-DCE: 0x%X\n", acc_int);
    printf("2. Tiempo Total:      %.3f ms (%.6f s)\n", t_div_sec * 1000.0, t_div_sec);
    printf("3. Metrica Final:     %.0f ops/seg (%.4f MOPS)\n\n", ops_sec_div, ops_sec_div / 1e6);

    // -------------------------------------------------------------------
    // 4. SUMA FLOTANTE (FLOAT 32 BITS)
    // -------------------------------------------------------------------
    printf("--- [4/8] PRUEBA: SUMA PUNTO FLOTANTE (FLOAT) ---\n");
    acc_float = 0.0f;
    t0 = get_time_sec();
    for (uint32_t i = 1; i <= 1000000UL; i++) {
        acc_float += (float)i;
    }
    t1 = get_time_sec();

    double t_fadd_sec = t1 - t0;
    double flops_add = 1000000.0 / t_fadd_sec;

    printf("1. Carga Procesada:   1,000,000 operaciones flotantes\n");
    printf("   - Acumulador Anti-DCE: %.4f\n", acc_float);
    printf("2. Tiempo Total:      %.3f ms (%.6f s)\n", t_fadd_sec * 1000.0, t_fadd_sec);
    printf("3. Metrica Final:     %.0f FLOPS/s (%.4f MFLOPS)\n\n", flops_add, flops_add / 1e6);

    // -------------------------------------------------------------------
    // 5. MULTIPLICACIÓN FLOTANTE (FLOAT 32 BITS)
    // -------------------------------------------------------------------
    printf("--- [5/8] PRUEBA: MULTIPLICACION PUNTO FLOTANTE (FLOAT) ---\n");
    acc_float = 1.0f;
    t0 = get_time_sec();
    for (uint32_t i = 1; i <= 1000000UL; i++) {
        acc_float += ((float)i * 1.00001f);
    }
    t1 = get_time_sec();

    double t_fmul_sec = t1 - t0;
    double flops_mul = 1000000.0 / t_fmul_sec;

    printf("1. Carga Procesada:   1,000,000 operaciones flotantes\n");
    printf("   - Acumulador Anti-DCE: %.4f\n", acc_float);
    printf("2. Tiempo Total:      %.3f ms (%.6f s)\n", t_fmul_sec * 1000.0, t_fmul_sec);
    printf("3. Metrica Final:     %.0f FLOPS/s (%.4f MFLOPS)\n\n", flops_mul, flops_mul / 1e6);

    // -------------------------------------------------------------------
    // 6. DIVISIÓN FLOTANTE (FLOAT 32 BITS)
    // -------------------------------------------------------------------
    printf("--- [6/8] PRUEBA: DIVISION PUNTO FLOTANTE (FLOAT) ---\n");
    acc_float = 1.0f;
    t0 = get_time_sec();
    for (uint32_t i = 1; i <= 1000000UL; i++) {
        acc_float += ((float)i / 3.14159f);
    }
    t1 = get_time_sec();

    double t_fdiv_sec = t1 - t0;
    double flops_div = 1000000.0 / t_fdiv_sec;

    printf("1. Carga Procesada:   1,000,000 operaciones flotantes\n");
    printf("   - Acumulador Anti-DCE: %.4f\n", acc_float);
    printf("2. Tiempo Total:      %.3f ms (%.6f s)\n", t_fdiv_sec * 1000.0, t_fdiv_sec);
    printf("3. Metrica Final:     %.0f FLOPS/s (%.4f MFLOPS)\n\n", flops_div, flops_div / 1e6);

    // -------------------------------------------------------------------
    // 7. EVALUACIÓN DE NÚMEROS PRIMOS (2 A 60,000)
    // -------------------------------------------------------------------
    printf("--- [7/8] PRUEBA: EVALUACION DE PRIMOS (2 A 60,000) ---\n");
    uint16_t total_primos = 0;
    t0 = get_time_sec();

    for (uint16_t N = 2; N <= 60000; N++) {
        uint16_t D = 2;
        while (1) {
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
    t1 = get_time_sec();

    double t_primos_sec = t1 - t0;
    double primos_sec = (double)total_primos / t_primos_sec;

    printf("1. Carga Procesada:   59,999 numeros evaluados (Rango 2-60,000)\n");
    printf("   - Primos Hallados:  %d / 6057\n", total_primos);
    printf("2. Tiempo Total:      %.3f ms (%.6f s)\n", t_primos_sec * 1000.0, t_primos_sec);
    printf("3. Metrica Final:     %.2f primos/seg\n\n", primos_sec);

    // -------------------------------------------------------------------
    // 8. COMPRESIÓN RLE (10,000 PASADAS)
    // -------------------------------------------------------------------
    printf("--- [8/8] PRUEBA: COMPRESION RLE (10,000 PASADAS) ---\n");
    acc_int = 0;
    t0 = get_time_sec();

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
    t1 = get_time_sec();

    double t_rle_sec = t1 - t0;
    double bytes_totales = 16.0 * 10001.0;
    double throughput_mb = (bytes_totales / (1024.0 * 1024.0)) / t_rle_sec;

    printf("1. Carga Procesada:   10,001 pasadas sobre ROM (160,016 B)\n");
    printf("   - Checksum Anti-DCE: 0x%X\n", acc_int);
    printf("2. Tiempo Total:      %.3f ms (%.6f s)\n", t_rle_sec * 1000.0, t_rle_sec);
    printf("3. Metrica Final:     %.2f MB/s\n\n", throughput_mb);
}

int main(void) {
    printf("==================================================\n");
    printf("   BENCHMARK DE RENDIMIENTO - PC (x86) / RPI ARM   \n");
    printf("==================================================\n\n");

    ejecutar_benchmark_secuencial();

    printf("==================================================\n");
    printf("           SUITE AUTOMÁTICA FINALIZADA            \n");
    printf("==================================================\n");

    return 0;
}