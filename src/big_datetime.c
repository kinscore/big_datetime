/*
 * Copyright (C) 2013 Pebble Technology
 * Copyright (C) 2013 Kipling Inscore
 * This software is released under the MIT License.
 */

#include "pebble.h"

// Settings
// TODO: make run-time configurable as appropriate

const GColor fg_color = GColorWhite;
const GColor bg_color = GColorBlack;

#define DATETIME_SYSTEM_FONT
#define DATETIME_FONT FONT_KEY_ROBOTO_BOLD_SUBSET_49
#define DAY_SYSTEM_FONT
#define DAY_FONT FONT_KEY_GOTHIC_18_BOLD
#define DAY_ALIGNMENT GTextAlignmentRight


// not really settings
#define ALL_UNITS ((YEAR_UNIT << 1) - 1)

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168

#define datetime_x 0
#define datetime_y 5
#define datetime_width (SCREEN_WIDTH - 2 * datetime_x)
#define datetime_height 50
#define datetime_line 45

#define day_x 0
#define day_height 25
#define day_y (SCREEN_HEIGHT - day_height)
#define day_width (SCREEN_WIDTH - day_x - 5)

const GRect year_rect = {
  {datetime_x, datetime_y},
  {datetime_width, datetime_height}
};
const GRect date_rect = {
  {datetime_x, datetime_y + datetime_line},
  {datetime_width, datetime_height}
};
const GRect time_rect = {
  {datetime_x, datetime_y + datetime_line * 2},
  {datetime_width, datetime_height}
};
const GRect day_rect = {
  {day_x, day_y},
  {day_width, day_height}
};

TextLayer *text_year_layer;
TextLayer *text_date_layer;
TextLayer *text_time_layer;
TextLayer *text_day_layer;

void update_year(struct tm *tm) {
  static char year_text[] = "0000";
  strftime(year_text, sizeof(year_text), "%Y", tm);
  text_layer_set_text(text_year_layer, year_text);
}

void update_date(struct tm *tm) {
  static char date_text[] = "0000";
  strftime(date_text, sizeof(date_text), "%m%d", tm);
  text_layer_set_text(text_date_layer, date_text);
}

void update_day(struct tm *tm) {
  static char day_text[] = "Xxxxxxxxx";
  strftime(day_text, sizeof(day_text), "%A", tm);
  text_layer_set_text(text_day_layer, day_text);
}

void update_time(struct tm *tm) {
  static char time_text[] = "0000";
  strftime(time_text, sizeof(time_text), clock_is_24h_style() ? "%H%M" : "%I%M", tm);
  text_layer_set_text(text_time_layer, time_text);
}

void update(struct tm *tm, TimeUnits units_changed) {
  if (units_changed & YEAR_UNIT)
    update_year(tm);

  if (units_changed & DAY_UNIT) {
    update_date(tm);
    update_day(tm);
  } else if (units_changed & MONTH_UNIT) {
    update_date(tm);
  }

  // update() would probably not be called if the time hasn't changed
  update_time(tm);
}

void handle_create(void) {
  Window *window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, bg_color);

  Layer *window_layer = window_get_root_layer(window);

#ifdef DATETIME_SYSTEM_FONT
  GFont datetime_font = fonts_get_system_font(DATETIME_FONT);
#else
  GFont datetime_font = fonts_load_custom_font(resource_get_handle(DATETIME_FONT));
#endif
#ifdef DAY_SYSTEM_FONT
  GFont day_font = fonts_get_system_font(DAY_FONT);
#else
  GFont day_font = fonts_load_custom_font(resource_get_handle(DAY_FONT));
#endif

  text_year_layer = text_layer_create(year_rect);
  text_layer_set_text_color(text_year_layer, fg_color);
  text_layer_set_background_color(text_year_layer, GColorClear);
  text_layer_set_font(text_year_layer, datetime_font);
  text_layer_set_text_alignment(text_year_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_year_layer));

  text_date_layer = text_layer_create(date_rect);
  text_layer_set_text_color(text_date_layer, fg_color);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, datetime_font);
  text_layer_set_text_alignment(text_date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

  text_time_layer = text_layer_create(time_rect);
  text_layer_set_text_color(text_time_layer, fg_color);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, datetime_font);
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  text_day_layer = text_layer_create(day_rect);
  text_layer_set_text_color(text_day_layer, fg_color);
  text_layer_set_background_color(text_day_layer, GColorClear);
  text_layer_set_font(text_day_layer, day_font);
  text_layer_set_text_alignment(text_day_layer, DAY_ALIGNMENT);
  layer_add_child(window_layer, text_layer_get_layer(text_day_layer));

  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  update(tm, ALL_UNITS);

  tick_timer_service_subscribe(MINUTE_UNIT, update);
}

void handle_destroy(void) {
  tick_timer_service_unsubscribe();
}

int main(void) {
  handle_create();
  app_event_loop();
  handle_destroy();
  return 0;
}
