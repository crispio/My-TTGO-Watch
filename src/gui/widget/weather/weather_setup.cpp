/****************************************************************************
 *   Tu May 22 21:23:51 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
 ****************************************************************************/
 
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <config.h>

#include "weather.h"
#include "weather_fetch.h"
#include "weather_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"

lv_obj_t *weather_widget_setup_tile = NULL;
lv_obj_t *weather_apikey_textfield = NULL;
lv_obj_t *weather_lat_textfield = NULL;
lv_obj_t *weather_lon_textfield = NULL;
lv_obj_t *weather_autosync_onoff = NULL;
lv_style_t weather_widget_setup_style;

LV_IMG_DECLARE(exit_32px);

static void weather_apikey_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_weather_widget_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void weather_autosync_onoff_event_handler( lv_obj_t * obj, lv_event_t event );

void weather_widget_setup_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {

    weather_config_t *weather_config = weather_get_config();

    lv_style_init( &weather_widget_setup_style );
    lv_style_set_radius( &weather_widget_setup_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color( &weather_widget_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &weather_widget_setup_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &weather_widget_setup_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color( &weather_widget_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_image_recolor( &weather_widget_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

    weather_widget_setup_tile = lv_obj_create( tile, NULL);
    lv_obj_set_size( weather_widget_setup_tile, hres , vres);
    lv_obj_align( weather_widget_setup_tile, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style( weather_widget_setup_tile, LV_OBJ_PART_MAIN, &weather_widget_setup_style );

    lv_obj_t *exit_btn = lv_imgbtn_create( weather_widget_setup_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( exit_btn, weather_widget_setup_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_weather_widget_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( weather_widget_setup_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( exit_label, "open weather setup");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *weather_apikey_cont = lv_obj_create( weather_widget_setup_tile, NULL );
    lv_obj_set_size(weather_apikey_cont, hres , 40);
    lv_obj_add_style( weather_apikey_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( weather_apikey_cont, weather_widget_setup_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    lv_obj_t *weather_apikey_label = lv_label_create( weather_apikey_cont, NULL);
    lv_obj_add_style( weather_apikey_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( weather_apikey_label, "appid");
    lv_obj_align( weather_apikey_label, weather_apikey_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    weather_apikey_textfield = lv_textarea_create( weather_apikey_cont, NULL);
    lv_textarea_set_text( weather_apikey_textfield, weather_config->apikey );
    lv_textarea_set_pwd_mode( weather_apikey_textfield, false);
    lv_textarea_set_one_line( weather_apikey_textfield, true);
    lv_textarea_set_cursor_hidden( weather_apikey_textfield, true);
    lv_obj_set_width( weather_apikey_textfield, LV_HOR_RES /4 * 3 );
    lv_obj_align( weather_apikey_textfield, weather_apikey_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( weather_apikey_textfield, weather_apikey_event_cb );

    lv_obj_t *weather_lat_cont = lv_obj_create( weather_widget_setup_tile, NULL );
    lv_obj_set_size(weather_lat_cont, hres / 2 , 40 );
    lv_obj_add_style( weather_lat_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( weather_lat_cont, weather_apikey_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );
    lv_obj_t *weather_lat_label = lv_label_create( weather_lat_cont, NULL);
    lv_obj_add_style( weather_lat_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( weather_lat_label, "lat");
    lv_obj_align( weather_lat_label, weather_lat_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    weather_lat_textfield = lv_textarea_create( weather_lat_cont, NULL);
    lv_textarea_set_text( weather_lat_textfield, weather_config->lat );
    lv_textarea_set_pwd_mode( weather_lat_textfield, false);
    lv_textarea_set_one_line( weather_lat_textfield, true);
    lv_textarea_set_cursor_hidden( weather_lat_textfield, true);
    lv_obj_set_width( weather_lat_textfield, LV_HOR_RES / 4 );
    lv_obj_align( weather_lat_textfield, weather_lat_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( weather_lat_textfield, weather_apikey_event_cb );

    lv_obj_t *weather_lon_cont = lv_obj_create( weather_widget_setup_tile, NULL );
    lv_obj_set_size(weather_lon_cont, hres / 2 , 40 );
    lv_obj_add_style( weather_lon_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( weather_lon_cont, weather_apikey_cont, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0 );
    lv_obj_t *weather_lon_label = lv_label_create( weather_lon_cont, NULL);
    lv_obj_add_style( weather_lon_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( weather_lon_label, "lon");
    lv_obj_align( weather_lon_label, weather_lon_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    weather_lon_textfield = lv_textarea_create( weather_lon_cont, NULL);
    lv_textarea_set_text( weather_lon_textfield, weather_config->lon );
    lv_textarea_set_pwd_mode( weather_lon_textfield, false);
    lv_textarea_set_one_line( weather_lon_textfield, true);
    lv_textarea_set_cursor_hidden( weather_lon_textfield, true);
    lv_obj_set_width( weather_lon_textfield, LV_HOR_RES / 4 );
    lv_obj_align( weather_lon_textfield, weather_lon_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( weather_lon_textfield, weather_apikey_event_cb );

    lv_obj_t *weather_autosync_cont = lv_obj_create( weather_widget_setup_tile, NULL );
    lv_obj_set_size( weather_autosync_cont, hres , 40);
    lv_obj_add_style( weather_autosync_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( weather_autosync_cont, weather_lat_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );
    weather_autosync_onoff = lv_switch_create( weather_autosync_cont, NULL );
    lv_switch_off( weather_autosync_onoff, LV_ANIM_ON );
    lv_obj_align( weather_autosync_onoff, weather_autosync_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( weather_autosync_onoff, weather_autosync_onoff_event_handler );
    lv_obj_t *weather_autosync_label = lv_label_create( weather_autosync_cont, NULL);
    lv_obj_add_style( weather_autosync_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( weather_autosync_label, "sync if wifi connected");
    lv_obj_align( weather_autosync_label, weather_autosync_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    if ( weather_config->autosync )
        lv_switch_on( weather_autosync_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( weather_autosync_onoff, LV_ANIM_OFF );
}

static void weather_apikey_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}

static void weather_autosync_onoff_event_handler( lv_obj_t * obj, lv_event_t event ) {
    if(event == LV_EVENT_VALUE_CHANGED) {
        weather_config_t *weather_config = weather_get_config();
        if( lv_switch_get_state( obj ) ) {
            weather_config->autosync = true;
        }
        else {
            weather_config->autosync = false;
        }
    }
}

static void exit_weather_widget_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       keyboard_hide();
                                        weather_config_t *weather_config = weather_get_config();
                                        strcpy( weather_config->apikey, lv_textarea_get_text( weather_apikey_textfield ) );
                                        strcpy( weather_config->lat, lv_textarea_get_text( weather_lat_textfield ) );
                                        strcpy( weather_config->lon, lv_textarea_get_text( weather_lon_textfield ) );
                                        weather_save_config();
                                        weather_jump_to_forecast();
                                        break;
    }
}