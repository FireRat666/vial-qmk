// ============================================================================
// Custom RGB Matrix Animations for Keychron K10 HE (QMK / Vial-QMK)
// ============================================================================

// ----------------------------------------------------------------------------
// 1. Declare Custom Effects
// ----------------------------------------------------------------------------
// Flowing Rainbow Reactive Modes
RGB_MATRIX_EFFECT(REACTIVE_RAINBOW_KEY)
RGB_MATRIX_EFFECT(REACTIVE_RAINBOW_SPLASH)
RGB_MATRIX_EFFECT(REACTIVE_RAINBOW_MULTISPLASH)

// Dynamic Multi-Color Shifting Backgrounds with Key Splash
RGB_MATRIX_EFFECT(REACTIVE_CYBERPUNK_FLOW)
RGB_MATRIX_EFFECT(REACTIVE_AURORA_FLOW)
RGB_MATRIX_EFFECT(REACTIVE_SUNSET_WAVE)
RGB_MATRIX_EFFECT(REACTIVE_NEON_CHEVRON)
RGB_MATRIX_EFFECT(REACTIVE_PLASMA_PULSE)

// Classic Standalone Animations
RGB_MATRIX_EFFECT(STARRY_NIGHT)
RGB_MATRIX_EFFECT(FIRE_SIMULATOR)
RGB_MATRIX_EFFECT(FIREWORKS)
RGB_MATRIX_EFFECT(REACTIVE_SPECTRUM_WAVES)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

// Helper function to calculate keypress hit intensity using QMK's g_last_hit_tracker
#if defined(RGB_MATRIX_KEYPRESSES) || defined(RGB_MATRIX_KEYRELEASES)
static uint16_t get_hit_boost(uint8_t led_idx, uint8_t max_dist_param) {
    uint16_t total_boost = 0;
    for (uint8_t h = 0; h < g_last_hit_tracker.count; h++) {
        int16_t dx = g_led_config.point[led_idx].x - g_last_hit_tracker.x[h];
        int16_t dy = g_led_config.point[led_idx].y - g_last_hit_tracker.y[h];
        uint16_t dist = sqrt16(dx * dx + dy * dy);
        uint16_t tick = g_last_hit_tracker.tick[h];

        if (tick < 255 && dist < max_dist_param) {
            uint16_t tf = 255 - tick;
            uint16_t sf = (max_dist_param - dist) * 255 / max_dist_param;
            total_boost += (tf * sf) >> 8;
        }
    }
    return (total_boost > 255) ? 255 : total_boost;
}
#endif

// ----------------------------------------------------------------------------
// 1. Flowing Rainbow Reactive Modes (Key, Splash, MultiSplash)
// ----------------------------------------------------------------------------
static bool reactive_rainbow_runner(effect_params_t *params, uint8_t radius_mode) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint16_t time = scale16by8(g_rgb_timer, rgb_matrix_config.speed / 4);
    uint8_t base_v = rgb_matrix_config.hsv.v / 3;
    if (base_v < 25) base_v = 25;

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        uint8_t hue = rgb_matrix_config.hsv.h + (g_led_config.point[i].x * 255 / 224) + (time >> 8);
        uint16_t val_boost = 0;

#if defined(RGB_MATRIX_KEYPRESSES) || defined(RGB_MATRIX_KEYRELEASES)
        for (uint8_t h = 0; h < g_last_hit_tracker.count; h++) {
            int16_t dx = g_led_config.point[i].x - g_last_hit_tracker.x[h];
            int16_t dy = g_led_config.point[i].y - g_last_hit_tracker.y[h];
            uint16_t dist = sqrt16(dx * dx + dy * dy);

            uint16_t tick = g_last_hit_tracker.tick[h];
            if (tick < 255) {
                uint8_t time_fade = 255 - tick;

                if (radius_mode == 0) { // Single key
                    if (g_last_hit_tracker.index[h] == i) val_boost += time_fade;
                } else if (radius_mode == 1) { // Medium splash
                    uint16_t max_dist = 48;
                    if (dist < max_dist) {
                        val_boost += (time_fade * (max_dist - dist) * 255 / max_dist) >> 8;
                    }
                } else if (radius_mode == 2) { // Large splash
                    uint16_t max_dist = 96;
                    if (dist < max_dist) {
                        val_boost += (time_fade * (max_dist - dist) * 255 / max_dist) >> 8;
                    }
                }
            }
        }
#endif

        uint16_t final_v = base_v + val_boost;
        if (final_v > rgb_matrix_config.hsv.v) final_v = rgb_matrix_config.hsv.v;

        HSV hsv = { hue, rgb_matrix_config.hsv.s, (uint8_t)final_v };
        RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    return led_max < RGB_MATRIX_LED_COUNT;
}

static bool REACTIVE_RAINBOW_KEY(effect_params_t *params) { return reactive_rainbow_runner(params, 0); }
static bool REACTIVE_RAINBOW_SPLASH(effect_params_t *params) { return reactive_rainbow_runner(params, 1); }
static bool REACTIVE_RAINBOW_MULTISPLASH(effect_params_t *params) { return reactive_rainbow_runner(params, 2); }

// ----------------------------------------------------------------------------
// 2. Dynamic Shifting Multi-Color Backgrounds with Key Splash
// ----------------------------------------------------------------------------

// A. Cyberpunk Flow (Shifting Cyan <-> Hot Pink / Purple)
static bool REACTIVE_CYBERPUNK_FLOW(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    uint16_t time = scale16by8(g_rgb_timer, rgb_matrix_config.speed / 4);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        uint8_t wave = sin8((time >> 2) + (g_led_config.point[i].x * 255 / 224));
        uint8_t bg_hue = 128 + ((wave * 92) >> 8); // Cyan (128) to Pink/Purple (220)
        uint8_t base_v = rgb_matrix_config.hsv.v / 3;
        if (base_v < 25) base_v = 25;

        uint16_t val_boost = 0;
#if defined(RGB_MATRIX_KEYPRESSES) || defined(RGB_MATRIX_KEYRELEASES)
        val_boost = get_hit_boost(i, 55);
#endif

        uint16_t final_v = base_v + val_boost;
        if (final_v > rgb_matrix_config.hsv.v) final_v = rgb_matrix_config.hsv.v;

        HSV hsv = { bg_hue, rgb_matrix_config.hsv.s, (uint8_t)final_v };
        RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < RGB_MATRIX_LED_COUNT;
}

// B. Aurora Borealis Flow (Emerald Green <-> Teal <-> Deep Violet)
static bool REACTIVE_AURORA_FLOW(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    uint16_t time = scale16by8(g_rgb_timer, rgb_matrix_config.speed / 4);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        uint8_t wave = sin8((time >> 2) + (g_led_config.point[i].x * 3) + (g_led_config.point[i].y * 2));
        uint8_t bg_hue = 75 + ((wave * 115) >> 8); // Emerald Green (75) to Violet (190)
        uint8_t base_v = rgb_matrix_config.hsv.v / 3;
        if (base_v < 25) base_v = 25;

        uint16_t val_boost = 0;
#if defined(RGB_MATRIX_KEYPRESSES) || defined(RGB_MATRIX_KEYRELEASES)
        val_boost = get_hit_boost(i, 55);
#endif

        uint16_t final_v = base_v + val_boost;
        if (final_v > rgb_matrix_config.hsv.v) final_v = rgb_matrix_config.hsv.v;

        HSV hsv = { bg_hue, rgb_matrix_config.hsv.s, (uint8_t)final_v };
        RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < RGB_MATRIX_LED_COUNT;
}

// C. Sunset Wave (Flame Orange <-> Crimson <-> Purple)
static bool REACTIVE_SUNSET_WAVE(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    uint16_t time = scale16by8(g_rgb_timer, rgb_matrix_config.speed / 4);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        uint8_t wave = sin8((time >> 2) + (g_led_config.point[i].x * 255 / 224));
        uint8_t bg_hue = 15 + ((wave * 195) >> 8); // Orange (15) to Crimson/Purple (210)
        uint8_t base_v = rgb_matrix_config.hsv.v / 3;
        if (base_v < 25) base_v = 25;

        uint16_t val_boost = 0;
#if defined(RGB_MATRIX_KEYPRESSES) || defined(RGB_MATRIX_KEYRELEASES)
        val_boost = get_hit_boost(i, 55);
#endif

        uint16_t final_v = base_v + val_boost;
        if (final_v > rgb_matrix_config.hsv.v) final_v = rgb_matrix_config.hsv.v;

        HSV hsv = { bg_hue, rgb_matrix_config.hsv.s, (uint8_t)final_v };
        RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < RGB_MATRIX_LED_COUNT;
}

// D. Neon Chevron Flow (Diagonal multi-color sweep)
static bool REACTIVE_NEON_CHEVRON(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    uint16_t time = scale16by8(g_rgb_timer, rgb_matrix_config.speed / 4);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        uint8_t bg_hue = rgb_matrix_config.hsv.h + (g_led_config.point[i].x + g_led_config.point[i].y) + (time >> 8);
        uint8_t base_v = rgb_matrix_config.hsv.v / 3;
        if (base_v < 25) base_v = 25;

        uint16_t val_boost = 0;
#if defined(RGB_MATRIX_KEYPRESSES) || defined(RGB_MATRIX_KEYRELEASES)
        val_boost = get_hit_boost(i, 55);
#endif

        uint16_t final_v = base_v + val_boost;
        if (final_v > rgb_matrix_config.hsv.v) final_v = rgb_matrix_config.hsv.v;

        HSV hsv = { bg_hue, rgb_matrix_config.hsv.s, (uint8_t)final_v };
        RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < RGB_MATRIX_LED_COUNT;
}

// E. Plasma Pulse (2D multi-color fluid wave)
static bool REACTIVE_PLASMA_PULSE(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    uint16_t time = scale16by8(g_rgb_timer, rgb_matrix_config.speed / 4);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        uint8_t p1 = sin8((time >> 2) + g_led_config.point[i].x * 3);
        uint8_t p2 = cos8((time >> 2) + g_led_config.point[i].y * 4);
        uint8_t bg_hue = rgb_matrix_config.hsv.h + ((p1 + p2) >> 1);
        uint8_t base_v = rgb_matrix_config.hsv.v / 3;
        if (base_v < 25) base_v = 25;

        uint16_t val_boost = 0;
#if defined(RGB_MATRIX_KEYPRESSES) || defined(RGB_MATRIX_KEYRELEASES)
        val_boost = get_hit_boost(i, 55);
#endif

        uint16_t final_v = base_v + val_boost;
        if (final_v > rgb_matrix_config.hsv.v) final_v = rgb_matrix_config.hsv.v;

        HSV hsv = { bg_hue, rgb_matrix_config.hsv.s, (uint8_t)final_v };
        RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < RGB_MATRIX_LED_COUNT;
}

// ----------------------------------------------------------------------------
// 3. Standalone Classic Animations (Starry Night, Fire, Fireworks, Spectrum Waves)
// ----------------------------------------------------------------------------

// A. Starry Night / Starfield
static bool STARRY_NIGHT(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    uint16_t time = scale16by8(g_rgb_timer, rgb_matrix_config.speed / 4);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        uint8_t star_phase = sin8((time >> 2) + i * 47);
        uint8_t star_v = scale8(star_phase, rgb_matrix_config.hsv.v / 4);

        uint16_t val_boost = 0;
#if defined(RGB_MATRIX_KEYPRESSES) || defined(RGB_MATRIX_KEYRELEASES)
        val_boost = get_hit_boost(i, 40);
#endif

        uint16_t final_v = star_v + val_boost;
        if (final_v > rgb_matrix_config.hsv.v) final_v = rgb_matrix_config.hsv.v;

        uint8_t hue = (val_boost > 50) ? 130 : 170;
        uint8_t sat = (val_boost > 50) ? 100 : 220;

        HSV hsv = { hue, sat, (uint8_t)final_v };
        RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < RGB_MATRIX_LED_COUNT;
}

// B. Fire Simulator
static bool FIRE_SIMULATOR(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    uint16_t time = scale16by8(g_rgb_timer, rgb_matrix_config.speed / 2);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        uint8_t y_pos = g_led_config.point[i].y;
        uint8_t heat = sin8((time >> 2) + g_led_config.point[i].x * 3 + y_pos * 4);
        heat = scale8(heat, y_pos * 4);

        uint16_t val_boost = 0;
#if defined(RGB_MATRIX_KEYPRESSES) || defined(RGB_MATRIX_KEYRELEASES)
        val_boost = get_hit_boost(i, 45);
#endif

        uint16_t total_heat = heat + val_boost;
        if (total_heat > 255) total_heat = 255;

        uint8_t hue = (total_heat * 35) / 255;
        uint8_t sat = 255 - ((total_heat > 180) ? (total_heat - 180) : 0);
        uint8_t val = scale8(total_heat, rgb_matrix_config.hsv.v);

        HSV hsv = { hue, sat, val };
        RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < RGB_MATRIX_LED_COUNT;
}

// C. Fireworks (Expanding multi-color ring explosions on keypress)
static bool FIREWORKS(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        uint16_t spark_val = 15;
        uint8_t spark_hue = rgb_matrix_config.hsv.h;
        uint8_t spark_sat = rgb_matrix_config.hsv.s;

#if defined(RGB_MATRIX_KEYPRESSES) || defined(RGB_MATRIX_KEYRELEASES)
        for (uint8_t h = 0; h < g_last_hit_tracker.count; h++) {
            int16_t dx = g_led_config.point[i].x - g_last_hit_tracker.x[h];
            int16_t dy = g_led_config.point[i].y - g_last_hit_tracker.y[h];
            uint16_t dist = sqrt16(dx * dx + dy * dy);
            uint16_t tick = g_last_hit_tracker.tick[h];

            uint16_t ring_radius = (tick * 100) / 255;
            int16_t delta = (int16_t)dist - (int16_t)ring_radius;
            if (delta < 0) delta = -delta;

            if (delta < 12 && tick < 240) {
                uint16_t intensity = (12 - delta) * (255 - tick) / 12;
                if (intensity > spark_val) {
                    spark_val = intensity;
                    spark_hue = (h * 73 + tick * 2) & 0xFF;
                    spark_sat = 200;
                }
            }
        }
#endif

        if (spark_val > rgb_matrix_config.hsv.v) spark_val = rgb_matrix_config.hsv.v;

        HSV hsv = { spark_hue, spark_sat, (uint8_t)spark_val };
        RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < RGB_MATRIX_LED_COUNT;
}

// D. Reactive Spectrum Waves (Continuous spectrum + shockwave color ripples)
static bool REACTIVE_SPECTRUM_WAVES(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    uint16_t time = scale16by8(g_rgb_timer, rgb_matrix_config.speed / 4);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        uint8_t base_hue = rgb_matrix_config.hsv.h + (g_led_config.point[i].x * 255 / 224) + (time >> 8);
        uint8_t base_v = rgb_matrix_config.hsv.v / 3;
        if (base_v < 25) base_v = 25;

        uint16_t val_boost = 0;
        uint8_t hue_shift = 0;

#if defined(RGB_MATRIX_KEYPRESSES) || defined(RGB_MATRIX_KEYRELEASES)
        for (uint8_t h = 0; h < g_last_hit_tracker.count; h++) {
            int16_t dx = g_led_config.point[i].x - g_last_hit_tracker.x[h];
            int16_t dy = g_led_config.point[i].y - g_last_hit_tracker.y[h];
            uint16_t dist = sqrt16(dx * dx + dy * dy);
            uint16_t tick = g_last_hit_tracker.tick[h];

            uint16_t wave_r = (tick * 120) / 255;
            int16_t diff = (int16_t)dist - (int16_t)wave_r;
            if (diff < 0) diff = -diff;

            if (diff < 16 && tick < 255) {
                uint16_t wave_intensity = (16 - diff) * (255 - tick) / 16;
                val_boost += wave_intensity;
                hue_shift += (wave_intensity >> 1);
            }
        }
#endif

        uint16_t final_v = base_v + val_boost;
        if (final_v > rgb_matrix_config.hsv.v) final_v = rgb_matrix_config.hsv.v;

        HSV hsv = { base_hue + hue_shift, rgb_matrix_config.hsv.s, (uint8_t)final_v };
        RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < RGB_MATRIX_LED_COUNT;
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
