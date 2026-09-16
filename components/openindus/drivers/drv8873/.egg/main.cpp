/**
 * @file main.cpp 
 * @author Georges de Massol
 * @brief DC Motor Melody Player for OI-DC (DC Motor Controller) module
 * 
 * You found my easter egg!
 * 
 * Just copy this file and music.h to your main in your OI-DC based project and run it.
 * 
 * This example plays a melody using the DC motor by rapidly switching directions
 * to create audible tones at musical frequencies.
 * 
 * OI-DC Module Pinout
 *
 * Power Supply (24V)   
 *      +                                                         +----------------
 *      |                                                         |               |
 *      |                                                         |               |     
 * +----|-------------------------+-------+--------------+--------+----+          |
 * |    |        |       |        |       |      |       |        |    |          |
 * |  9V-30V   9V-30V   /B2      /A2      |     /B1     /A1       |    |        _____
 * |   VIN      VIN     DIN4    HB4_2   HB3_2   DIN2   HB2_2   HB1_2   |       /     \
 * |                                                                   |      |   M   |
 * |                                                                   |       \_____/       
 * |   GND      GND     DIN3    HB4_1   HB3_1   DIN1   HB2_1   HB1_1   |          |
 * |    |        |       B2       A2      |      B1      A1       |    |          |
 * |    |        |       |        |       |      |       |        |    |          |
 * +----+-------------------------+-------+--------------+--------+----+          |
 *      |                                                         |               |
 *      |                                                         |               |
 *      -                                                         +----------------
 *     GND
 *
 * @date November 2025
 */

#include "OpenIndus.h"
#include "Arduino.h"
#include "music.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include <math.h>

Dc dc;  // Create DC motor controller object

static const char TAG[] = "Main";

// Volume control (0-100%)
static int g_volume = 80;

// Sound shape parameters (ADSR envelope + staccato)
static float g_attack = 0.05f;    // Attack phase: 5% of note duration
static float g_decay = 0.15f;     // Decay phase: 15% of note duration 
static float g_release = 0.20f;   // Release phase: 20% of note duration 
static float g_sustain = 0.70f;   // Sustain level: 70% of max power
static float g_staccato = 0.7f;   // Staccato: 1.0 = full note, 0.5 = half note (marcato)

// CLI argument structure for volume command
static struct {
    struct arg_int *volume;
    struct arg_end *end;
} volume_args;

void playMelody()
{
    const Note* melody = music;
    int melodyLen = music_length;

    printf("Melody progress: 0%%");
    for (int n = 0; n < melodyLen; n++) {
        if (melody[n].freq > 0) {
            int period_us = 1000000 / melody[n].freq;
            int totalCycles = (melody[n].durationMs * 1000) / period_us;
            
            // Apply staccato/marcato - shorten the actual playing time
            int playingCycles = (int)(totalCycles * g_staccato);
            int silentCycles = totalCycles - playingCycles;
            
            // Apply exponential scaling for perceptual volume
            float expScale = (g_volume / 100.0f) * (g_volume / 100.0f);
            float scaledVolume = 20.0f + (80.0f * expScale);
            float maxPower = scaledVolume;
            float minPower = 15.0f;
            
            // ADSR envelope phases (in cycles)
            int attackEnd = (int)(playingCycles * g_attack);
            int decayEnd = (int)(playingCycles * (g_attack + g_decay));
            int releaseLen = (int)(playingCycles * g_release);
            if (releaseLen < 1) releaseLen = 1; // Always at least 1 cycle for release
            int releaseStart = playingCycles - releaseLen;
            if (releaseStart < decayEnd) releaseStart = decayEnd; // Prevent overlap

            // Sustain level
            float sustainPower = minPower + (maxPower - minPower) * g_sustain;

            int vibrato = 0;
            for (int i = 0; i < playingCycles; i++) {
                float power;

                if (i < attackEnd && attackEnd > 0) {
                    // Attack: ramp up from minPower to maxPower
                    float attackProgress = (float)i / (float)attackEnd;
                    power = minPower + (maxPower - minPower) * attackProgress;
                } else if (i < decayEnd && decayEnd > attackEnd) {
                    // Decay: drop from maxPower to sustainPower
                    float decayProgress = (float)(i - attackEnd) / (float)(decayEnd - attackEnd);
                    power = maxPower - (maxPower - sustainPower) * decayProgress;
                } else if (i < releaseStart) {
                    // Sustain: hold at sustainPower
                    power = sustainPower;
                    // Apply vibrato effect
                    vibrato = 1 + 9 * (1.0 - (scaledVolume / 100.0));
                } else if (i < playingCycles) {
                    // Release: fade from sustainPower to minPower
                    float releaseProgress = (float)(i - releaseStart) / (float)(playingCycles - releaseStart);
                    power = sustainPower - (sustainPower - minPower) * releaseProgress;
                } else {
                    power = minPower;
                }

                unsigned long cycleStartTime = micros();
                dc.run(MOTOR_1, FORWARD, power - vibrato);
                while (micros() - cycleStartTime < period_us / 2);
                cycleStartTime = micros();
                dc.run(MOTOR_1, REVERSE, power);
                while (micros() - cycleStartTime < period_us / 2);
            }
            
            // Staccato silence at end of note
            if (silentCycles > 0) {
                dc.stop(MOTOR_1);
                delay((silentCycles * period_us) / 1000);
            }
        } else {
            // Rest (no sound)
            dc.stop(MOTOR_1);
            delay(melody[n].durationMs);
        }
        dc.stop(MOTOR_1);
        // print percentage played
        static int lastPercentPlayed = -1;
        int percentPlayed = (int)(((n + 1) / (float)melodyLen) * 100);
        if (percentPlayed != lastPercentPlayed) {
            lastPercentPlayed = percentPlayed;
            printf("\rMelody progress: %d%%", percentPlayed);
            // purge output buffer
            fflush(stdout);
        }
        delay(20);  // Small gap between notes
    }
    printf("\n");
}

// CLI command handler for volume
static int cmd_volume(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&volume_args);
    
    if (nerrors != 0) {
        arg_print_errors(stderr, volume_args.end, argv[0]);
        return 1;
    }
    
    if (volume_args.volume->count > 0) {
        int vol = volume_args.volume->ival[0];
        if (vol < 0) vol = 0;
        if (vol > 100) vol = 100;
        g_volume = vol;
        printf("Volume set to %d%%\n", g_volume);
    } else {
        printf("Current volume: %d%%\n", g_volume);
    }
    
    return 0;
}

// CLI command handler for play
static int cmd_play(int argc, char **argv)
{
    printf("Playing melody in the background at volume %d%%...\n", g_volume);
    xTaskCreate([](void*){
        playMelody();
        vTaskDelete(NULL);
    }, "MelodyTask", 4096, NULL, 5, NULL);
    return 0;
}

// Register CLI commands
static void register_cli_commands(void)
{
    // Volume command
    volume_args.volume = arg_int0(NULL, NULL, "<0-100>", "Set volume percentage (0-100)");
    volume_args.end = arg_end(2);
    
    const esp_console_cmd_t volume_cmd = {
        .command = "volume",
        .help = "Get or set volume (0-100%)",
        .hint = NULL,
        .func = &cmd_volume,
        .argtable = &volume_args,
        .func_w_context = NULL,
        .context = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&volume_cmd));
    
    // Play command
    const esp_console_cmd_t play_cmd = {
        .command = "play",
        .help = "Play the melody",
        .hint = NULL,
        .func = &cmd_play,
        .argtable = NULL,
        .func_w_context = NULL,
        .context = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&play_cmd));
}

void setup(void)
{
    dc.stop(MOTOR_1);
    delay(2000);
    ESP_LOGI(TAG, "🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚");
    ESP_LOGI(TAG, "Starting Melody Player");
    ESP_LOGI(TAG, "🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚🥚");
    
    // Register CLI commands
    register_cli_commands();
    ESP_LOGI(TAG, "CLI commands registered: 'volume', 'play'");
    
    playMelody();
    
    ESP_LOGI(TAG, "Melody finished!");
}

void loop(void)
{
    delay(5000);
}
