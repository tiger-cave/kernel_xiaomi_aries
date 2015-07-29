/* Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_hitachi.h"
#include <mach/socinfo.h>
#if defined(CONFIG_LEDS_LM3530_ARIES)
#include <linux/led-lm3530-aries.h>
#endif

#define DISP_P1
#define CE_HITACHI_MODE3

#define HITACHI_NO_DELAY 0	/* 0 */
#define HITACHI_CMD_DELAY 50	/* 50 default */
#define HITACHI_CMD_DELAY_10 10	/* 10 */
#define HITACHI_CMD_DELAY_20 20	/* 20 */
#define HITACHI_CMD_DELAY_60 20	/* 60 */
#define HITACHI_SLEEP_OFF_DELAY 120
static struct msm_panel_common_pdata *mipi_hitachi_pdata;

static struct dsi_buf hitachi_tx_buf;
static struct dsi_buf hitachi_rx_buf;

static int mipi_hitachi_lcd_init(void);

/*[3] exit sleep mode*/
static char exit_sleep_mode[2] = { 0x11, 0x00 };	/*DTYPE_DCS_WRITE */
static char mcap_start[2] = { 0xb0, 0x04 };	/*DTYPE_GEN_WRITE2 */

#if defined(DISP_P1)
static char nvm_noload[2] = { 0xd6, 0x01 };	/*DTYPE_GEN_WRITE2 *//*Sharp only */
static char disp_setting[2] = { 0xc1, 0x04 };	/*DTYPE_GEN_WRITE2 *//*Sharp only */
#endif

#if defined(CONFIG_FB_MSM_MIPI_DSI_CABC)
#if defined(CABC_DEFAULT)
static char test[26] = {
	0xB8, 0x18, 0x80, 0x18, 0x18, 0xCF, 0x04, 0x00,
	0x0C, 0x14, 0xAC, 0x14, 0x6C, 0x14, 0x0C, 0x14,
	0xDA, 0x6D, 0xFF, 0xFF, 0x10, 0x37, 0x5A, 0x87,
	0xBE, 0xFF
};				/*DTYPE_GEN_LWRITE */

static char cabc_movie_still[8] = {
	0xB9, 0x1A, 0x18, 0x02, 0x40, 0x85, 0x0A, 0x80
};				/*DTYPE_GEN_LWRITE */

static char cabc_user_inf[8] = {
	0xBA, 0x1A, 0x18, 0x02, 0x40, 0x85, 0x00, 0xD7
};				/*DTYPE_GEN_LWRITE */
#elif defined(CABC_OPTION1)
static char test[26] = {
	0xB8, 0x18, 0x80, 0x18, 0x18, 0xCF, 0x04, 0x00,
	0x0C, 0x14, 0xAC, 0x14, 0x6C, 0x14, 0x0C, 0x14,
	0xDA, 0x6D, 0xFF, 0xFF, 0x10, 0x37, 0x5A, 0x87,
	0xBE, 0xFF
};				/*DTYPE_GEN_LWRITE */

static char cabc_movie_still[8] = {
	0xB9, 0x1A, 0x18, 0x04, 0x40, 0x9F, 0x1F, 0x80
};				/*DTYPE_GEN_LWRITE */

static char cabc_user_inf[8] = {
	0xBA, 0x1A, 0x18, 0x04, 0x40, 0x9F, 0x1F, 0xD7
};				/*DTYPE_GEN_LWRITE */
#elif defined(CABC_OPTION2)
static char test[26] = {
	0xB8, 0x18, 0x80, 0x18, 0x18, 0xCF, 0x04, 0x00,
	0x0C, 0x12, 0x6C, 0x12, 0xAC, 0x12, 0x0C, 0x12,
	0xDA, 0x6D, 0xFF, 0xFF, 0x10, 0x67, 0x89, 0xAF,
	0xD6, 0xFF
};				/*DTYPE_GEN_LWRITE */

static char cabc_movie_still[8] = {
	0xB9, 0x1A, 0x18, 0x04, 0x40, 0x9F, 0x1F, 0x80
};				/*DTYPE_GEN_LWRITE */

static char cabc_user_inf[8] = {
	0xBA, 0x1A, 0x18, 0x04, 0x40, 0x9F, 0x1F, 0xD7
};				/*DTYPE_GEN_LWRITE */
#elif defined(CABC_OPTION3)
static char test[26] = {
	0xB8, 0x18, 0x80, 0x18, 0x18, 0xCF, 0x04, 0x00,
	0x0C, 0x10, 0x6C, 0x10, 0xAC, 0x10, 0x0C, 0x10,
	0xDA, 0x6D, 0xFF, 0xFF, 0x10, 0x8C, 0xAA, 0xC7,
	0xE3, 0xFF
};				/*DTYPE_GEN_LWRITE */

static char cabc_movie_still[8] = {
	0xB9, 0x1A, 0x18, 0x04, 0x40, 0x9F, 0x1F, 0x80
};				/*DTYPE_GEN_LWRITE */

static char cabc_user_inf[8] = {
	0xBA, 0x1A, 0x18, 0x04, 0x40, 0x9F, 0x1F, 0xD7
};				/*DTYPE_GEN_LWRITE */
#elif defined(CABC_OPTION4)
static char test[26] = {
	0xB8, 0x18, 0x80, 0x18, 0x18, 0xCF, 0x04, 0x00,
	0x0C, 0x0C, 0x6C, 0x0C, 0xAC, 0x0C, 0x0C, 0x0C,
	0xDA, 0x6D, 0xFF, 0xFF, 0x10, 0xB3, 0xC9, 0xDC,
	0xEE, 0xFF
};				/*DTYPE_GEN_LWRITE */

static char cabc_movie_still[8] = {
	0xB9, 0x1A, 0x18, 0x04, 0x40, 0x9F, 0x1F, 0x80
};				/*DTYPE_GEN_LWRITE */

static char cabc_user_inf[8] = {
	0xBA, 0x1A, 0x18, 0x04, 0x40, 0x9F, 0x1F, 0xD7
};				/*DTYPE_GEN_LWRITE */
#elif defined(CABC_OPTION5)
static char test[26] = {
	0xB8, 0x18, 0x80, 0x18, 0x18, 0xCF, 0x1F, 0x00,
	0x0C, 0x0F, 0x6C, 0x0F, 0x6C, 0x0F, 0x0C, 0x0F,
	0xDA, 0x6D, 0xFF, 0xFF, 0x10, 0x67, 0x89, 0xAF,
	0xD6, 0xFF
};				/*DTYPE_GEN_LWRITE */

static char cabc_movie_still[8] = {
	0xB9, 0x00, 0x3F, 0x04, 0x40, 0x9F, 0x1F, 0x80
};				/*DTYPE_GEN_LWRITE */

static char cabc_user_inf[8] = {
	0xBA, 0x00, 0x3F, 0x04, 0x40, 0x9F, 0x1F, 0xD7
};				/*DTYPE_GEN_LWRITE */
#elif defined(CABC_OPTION6)
static char test[26] = {
	0xB8, 0x18, 0x80, 0x18, 0x18, 0xCF, 0x1F, 0x00,
	0x0C, 0x12, 0x6C, 0x11, 0x6C, 0x12, 0x0C, 0x12,
	0xDA, 0x6D, 0xFF, 0xFF, 0x10, 0x67, 0xA3, 0xDB,
	0xFB, 0xFF
};				/*DTYPE_GEN_LWRITE */

static char cabc_movie_still[8] = {
	0xB9, 0x00, 0x30, 0x18, 0x18, 0x9F, 0x1F, 0x80
};				/*DTYPE_GEN_LWRITE */

static char cabc_user_inf[8] = {
	0xBA, 0x00, 0x30, 0x18, 0x18, 0x9F, 0x1F, 0xD7
};				/*DTYPE_GEN_LWRITE */
#else
static char test[26] = {
	0xB8, 0x18, 0x80, 0x18, 0x18, 0xCF, 0x1F, 0x00,
	0x0C, 0x0E, 0x6C, 0x0E, 0x6C, 0x0E, 0x0C, 0x0E,
	0xDA, 0x6D, 0xFF, 0xFF, 0x10, 0x8C, 0xD2, 0xFF,
	0xFF, 0xFF
};				/*DTYPE_GEN_LWRITE *//* Set CABC */

static char cabc_movie_still[8] = {
	0xB9, 0x00, 0x3F, 0x18, 0x18, 0x9F, 0x1F, 0x80
};				/*DTYPE_GEN_LWRITE */

static char cabc_user_inf[8] = {
	0xBA, 0x00, 0x3F, 0x18, 0x18, 0x9F, 0x1F, 0xD7
};				/*DTYPE_GEN_LWRITE */
#endif
#endif

static char mcap_end[2] = { 0xb0, 0x03 };	/*DTYPE_GEN_WRITE2 */

#if defined(CONFIG_FB_MSM_MIPI_DSI_CABC)
static char write_display_brightness[3] = { 0x51, 0xE, 0xFF };	/* DTYPE_DCS_LWRITE */
static char write_cabc[2] = { 0x55, 0x01 };	/* DTYPE_DCS_WRITE1 */
static char write_control_display[2] = { 0x53, 0x2C };	/* DTYPE_DCS_WRITE1 */
#else
static char write_display_brightness[3] = { 0x51, 0xE, 0xFF };	/* DTYPE_DCS_LWRITE */
static char write_cabc[2] = { 0x55, 0x00 };	/* DTYPE_DCS_WRITE1 */
static char write_control_display[2] = { 0x53, 0x00 };	/* DTYPE_DCS_WRITE1 */
#endif

#if defined(CONFIG_FB_MSM_MIPI_DSI_CE)
#if defined(CE_HITACHI_MODE1)
static char write_ce_on_jdi[33] = {
	0xCA, 0x01, 0x80, 0xDC, 0xF0, 0xDC, 0xF0, 0xDC,
	0xF0, 0x0C, 0x3F, 0x14, 0x80, 0x0A, 0x4A, 0x37,
	0xA0, 0x55, 0xF8, 0x0C, 0x0C, 0x20, 0x10, 0x3F,
	0x3F, 0x00, 0x00, 0x10, 0x10, 0x3F, 0x3F, 0x3F,
	0x3F
};				/* DTYPE_GEN_LWRITE */
#elif defined(CE_HITACHI_MODE2)
static char write_ce_on_jdi[33] = {
	0xCA, 0x01, 0x80, 0xDC, 0xF0, 0xDC, 0xF0, 0xDC,
	0xF0, 0x18, 0x3F, 0x14, 0x8A, 0x0A, 0x4A, 0x37,
	0xA0, 0x55, 0xF8, 0x0C, 0x0C, 0x20, 0x10, 0x3F,
	0x3F, 0x19, 0xD6, 0x10, 0x10, 0x3F, 0x3F, 0x3F,
	0x3F
};				/* DTYPE_GEN_LWRITE */
#elif defined(CE_HITACHI_MODE3)
static char write_ce_on_jdi[33] = {
	0xCA, 0x01, 0x80, 0x88, 0x8C, 0xBC, 0x8C, 0x8C,
	0x8C, 0x18, 0x3F, 0x14, 0xFF, 0x0A, 0x4A, 0x37,
	0xA0, 0x55, 0xF8, 0x0C, 0x0C, 0x20, 0x10, 0x3F,
	0x3F, 0x00, 0x00, 0x10, 0x10, 0x3F, 0x3F, 0x3F,
	0x3F
};				/* DTYPE_GEN_LWRITE */
#elif defined(CE_HITACHI_MODE4)
static char write_ce_on_jdi[33] = {
	0xCA, 0x01, 0x80, 0x8A, 0x8C, 0xDC, 0x96, 0x96,
	0x90, 0x18, 0x3F, 0x14, 0xFF, 0x0A, 0x4A, 0x37,
	0xA0, 0x55, 0xF8, 0x0C, 0x0C, 0x20, 0x10, 0x3F,
	0x3F, 0x00, 0x00, 0x10, 0x10, 0x3F, 0x3F, 0x3F,
	0x3F
};				/* DTYPE_GEN_LWRITE */
#else
static char write_ce_on_jdi[33] = {
	0xCA, 0x01, 0x80, 0xDC, 0xF0, 0xDC, 0xF0, 0xDC,
	0xF0, 0x0C, 0x3F, 0x14, 0x80, 0x0A, 0x4A, 0x37,
	0xA0, 0x55, 0xF8, 0x0C, 0x0C, 0x20, 0x10, 0x3F,
	0x3F, 0x00, 0x00, 0x10, 0x10, 0x3F, 0x3F, 0x3F,
	0x3F
};				/* DTYPE_GEN_LWRITE */
#endif
#if defined(CE_SHARP_EXAMPLE1)
static char write_ce_on[33] = {
	0xCA, 0x01, 0x80, 0x83, 0xA5, 0xC8, 0xD2, 0xDC,
	0xDC, 0x08, 0x20, 0x80, 0xFF, 0x0A, 0x4A, 0x37,
	0xA0, 0x55, 0xF8, 0x0C, 0x0C, 0x20, 0x10, 0x3F,
	0x3F, 0x00, 0x00, 0x10, 0x10, 0x3F, 0x3F, 0x3F,
	0x3F
};				/* DTYPE_GEN_LWRITE */
#elif defined(CE_SHARP_EXAMPLE2)
static char write_ce_on[33] = {
	0xCA, 0x01, 0x80, 0xDC, 0xF0, 0xDC, 0xF0, 0xDC,
	0xF0, 0x0C, 0x3F, 0x14, 0x80, 0x0A, 0x4A, 0x37,
	0xA0, 0x55, 0xF8, 0x0C, 0x0C, 0x20, 0x10, 0x3F,
	0x3F, 0x00, 0x00, 0x10, 0x10, 0x3F, 0x3F, 0x3F,
	0x3F
};				/* DTYPE_GEN_LWRITE */
#elif defined(CE_SHARP_EXAMPLE3)
static char write_ce_on[33] = {
	0xCA, 0x01, 0x80, 0xDC, 0xF0, 0xDC, 0xF0, 0xDC,
	0xF0, 0x18, 0x3F, 0x14, 0x8A, 0x0A, 0x4A, 0x37,
	0xA0, 0x55, 0xF8, 0x0C, 0x0C, 0x20, 0x10, 0x3F,
	0x3F, 0x00, 0x00, 0x10, 0x10, 0x3F, 0x3F, 0x3F,
	0x3F
};				/* DTYPE_GEN_LWRITE */
#elif defined(CE_SHARP_EXAMPLE4)
static char write_ce_on[33] = {
	0xCA, 0x01, 0x80, 0xDC, 0xF0, 0xDC, 0xF0, 0xDC,
	0xF0, 0x18, 0x3F, 0x14, 0x8A, 0x0A, 0x4A, 0x37,
	0xA0, 0x55, 0xF8, 0x0C, 0x0C, 0x20, 0x10, 0x3F,
	0x3F, 0x19, 0xD6, 0x10, 0x10, 0x3F, 0x3F, 0x3F,
	0x3F
};				/* DTYPE_GEN_LWRITE */
#else
static char write_ce_on[33] = {
	0xCA, 0x01, 0x80, 0x8A, 0x8C, 0xC8, 0x8C, 0x80,
	0x8C, 0x18, 0x3F, 0x14, 0xFF, 0x0A, 0x4A, 0x37,
	0xA0, 0x55, 0xF8, 0x0C, 0x0C, 0x20, 0x10, 0x3F,
	0x3F, 0x00, 0x00, 0x10, 0x10, 0x3F, 0x3F, 0x3F,
	0x3F
};				/* DTYPE_GEN_LWRITE */
#endif
#endif
static char set_tear_on[2] = { 0x35, 0x00 };	/*DTYPE_DCS_WRITE1 */

/*Cmd for GRAM Access*/
static char set_column_address[5] = { 0x2A, 0x00, 0x00, 0x02, 0xCF };	/* DTYPE_DCS_LWRITE */
static char set_page1_address[5] = { 0x2B, 0x00, 0x00, 0x04, 0xFF };	/* DTYPE_DCS_LWRITE */

/*[4] enter sleep mode*/
static char set_tear_off[2] = { 0x34, 0x00 };	/*DTYPE_DCS_WRITE */
static char enter_sleep_mode[2] = { 0x10, 0x00 };	/* DTYPE_DCS_WRITE */

/*[5] set display on*/
static char set_address_mode[2] = { 0x36, 0x00 };	/* DTYPE_DCS_WRITE1 */
static char set_pixel_format[2] = { 0x3a, 0x77 };	/* DTYPE_DCS_WRITE1 */
static char set_display_on[2] = { 0x29, 0x00 };	/* DTYPE_DCS_WRITE */

/*[6] set display off*/
static char set_display_off[2] = { 0x28, 0x00 };	/* DTYPE_DCS_WRITE */

/*[7] enter deep standby*/

/*[8] exit deep standby*/

/*[9] enter video Mode*/
static char set_interface_video[2] = { 0xB3, 0x40 };	/*DTYPE_GEN_WRITE2 */

/*[10] enter command mode*/
static char set_interface_cmd1[2] = { 0xB3, 0xA0 };	/*DTYPE_GEN_WRITE2 */
static char set_interface_cmd2[2] = { 0xB3, 0x00 };	/*DTYPE_GEN_WRITE2 */

static struct dsi_cmd_desc hitachi_hitachi_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, HITACHI_SLEEP_OFF_DELAY,
	 sizeof(exit_sleep_mode), exit_sleep_mode}
	,
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(mcap_start), mcap_start}
	,
#if defined(CONFIG_FB_MSM_MIPI_DSI_CE)
	{DTYPE_GEN_LWRITE, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(write_ce_on_jdi), write_ce_on_jdi}
	,
#else
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(write_ce_off), write_ce_off}
	,
#endif
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(mcap_end), mcap_end}
	,
	{DTYPE_DCS_LWRITE, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(write_display_brightness), write_display_brightness}
	,
	{DTYPE_DCS_WRITE1, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(write_control_display), write_control_display}
	,
	{DTYPE_DCS_WRITE1, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(write_cabc), write_cabc}
	,
	{DTYPE_DCS_LWRITE, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(set_column_address), set_column_address}
	,
	{DTYPE_DCS_LWRITE, 1, 0, 0, HITACHI_CMD_DELAY_20,
	 sizeof(set_page1_address), set_page1_address}
	,
	{DTYPE_DCS_WRITE1, 1, 0, 0, HITACHI_CMD_DELAY_20,
	 sizeof(set_address_mode), set_address_mode}
	,
	{DTYPE_DCS_WRITE1, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(set_pixel_format), set_pixel_format}
	,
	{DTYPE_DCS_WRITE, 1, 0, 0, HITACHI_CMD_DELAY_20,
	 sizeof(set_display_on), set_display_on}
	,
};

static struct dsi_cmd_desc hitachi_hitachi_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, HITACHI_CMD_DELAY_20,
	 sizeof(set_display_off), set_display_off}
	,
	{DTYPE_DCS_WRITE, 1, 0, 0, HITACHI_CMD_DELAY_60,
	 sizeof(enter_sleep_mode), enter_sleep_mode}
	,
};

static struct dsi_cmd_desc hitachi_sharp_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, HITACHI_SLEEP_OFF_DELAY,
	 sizeof(exit_sleep_mode), exit_sleep_mode}
	,
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(mcap_start), mcap_start}
	,
#if defined(DISP_P1)
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(nvm_noload), nvm_noload}
	,
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(disp_setting), disp_setting}
	,
#endif
#if defined(CONFIG_FB_MSM_MIPI_DSI_CABC)
	{DTYPE_GEN_LWRITE, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(test), test}
	,
	{DTYPE_GEN_LWRITE, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(cabc_movie_still), cabc_movie_still}
	,
	{DTYPE_GEN_LWRITE, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(cabc_user_inf), cabc_user_inf}
	,
#endif
#if defined(CONFIG_FB_MSM_MIPI_DSI_CE)
	{DTYPE_GEN_LWRITE, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(write_ce_on), write_ce_on}
	,
#else
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(write_ce_off), write_ce_off}
	,
#endif
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(mcap_end), mcap_end}
	,
	{DTYPE_DCS_LWRITE, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(write_display_brightness), write_display_brightness}
	,
	{DTYPE_DCS_WRITE1, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(write_control_display), write_control_display}
	,
	{DTYPE_DCS_WRITE1, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(write_cabc), write_cabc}
	,
	{DTYPE_DCS_LWRITE, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(set_column_address), set_column_address}
	,
	{DTYPE_DCS_LWRITE, 1, 0, 0, HITACHI_CMD_DELAY_20,
	 sizeof(set_page1_address), set_page1_address}
	,
	{DTYPE_DCS_WRITE1, 1, 0, 0, HITACHI_CMD_DELAY_20,
	 sizeof(set_address_mode), set_address_mode}
	,
	{DTYPE_DCS_WRITE1, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(set_pixel_format), set_pixel_format}
	,
	{DTYPE_DCS_WRITE, 1, 0, 0, HITACHI_CMD_DELAY_20,
	 sizeof(set_display_on), set_display_on}
	,
};

static struct dsi_cmd_desc hitachi_sharp_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, HITACHI_CMD_DELAY_20,
	 sizeof(set_display_off), set_display_off}
	,
	{DTYPE_DCS_WRITE, 1, 0, 0, HITACHI_CMD_DELAY_60,
	 sizeof(enter_sleep_mode), enter_sleep_mode}
	,
};

static struct dsi_cmd_desc hitachi_videomode_on_cmds[] = {
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(mcap_start), mcap_start}
	,
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(set_interface_video), set_interface_video}
	,
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(mcap_end), mcap_end}
	,
	{DTYPE_DCS_WRITE1, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(set_tear_on), set_tear_on}
	,
};

static struct dsi_cmd_desc hitachi_videomode_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(set_tear_off), set_tear_off}
	,
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(mcap_start), mcap_start}
	,
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_CMD_DELAY_20,
	 sizeof(set_interface_cmd1), set_interface_cmd1}
	,
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(set_interface_cmd2), set_interface_cmd2}
	,
	{DTYPE_GEN_WRITE2, 1, 0, 0, HITACHI_NO_DELAY,
	 sizeof(mcap_end), mcap_end}
	,
};

static char manufacture_id[2] = { 0x04, 0x00 };	/* DTYPE_DCS_READ */

static struct dsi_cmd_desc hitachi_manufacture_id_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(manufacture_id), manufacture_id
};

static u32 manu_id;

static void mipi_hitachi_manufature_cb(u32 data)
{
	manu_id = data;
	pr_info("%s: manufature_id=%x\n", __func__, manu_id);
}

static uint32 mipi_hitachi_manufacture_id(struct msm_fb_data_type *mfd)
{
	struct dcs_cmd_req cmdreq;
	cmdreq.cmds = &hitachi_manufacture_id_cmd;
	cmdreq.cmds_cnt = 1;
	cmdreq.flags = CMD_REQ_RX | CMD_REQ_COMMIT;
	cmdreq.rlen = 3;
	cmdreq.cb = mipi_hitachi_manufature_cb;
	mipi_dsi_cmdlist_put(&cmdreq);

	return manu_id;
}

extern int mipanel_id(void);
extern void mipanel_set_id(int id);

static int mipi_hitachi_lcd_on(struct platform_device *pdev)
{
	struct dcs_cmd_req cmdreq;
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	static int blon;

	if (!blon) {
		blon = 1;
		return 0;
	}

	mfd = platform_get_drvdata(pdev);
	mipi = &mfd->panel_info.mipi;

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

#if defined(CONFIG_LEDS_LM3530_ARIES)
	backlight_brightness_set(0);
#endif

	if (mipi->mode == DSI_VIDEO_MODE) {
		cmdreq.cmds = hitachi_videomode_on_cmds;
		cmdreq.cmds_cnt = ARRAY_SIZE(hitachi_videomode_on_cmds);
	} else if (mipanel_id()) {
		cmdreq.cmds = hitachi_hitachi_on_cmds;
		cmdreq.cmds_cnt = ARRAY_SIZE(hitachi_hitachi_on_cmds);
	} else {
		cmdreq.cmds = hitachi_sharp_on_cmds;
		cmdreq.cmds_cnt = ARRAY_SIZE(hitachi_sharp_on_cmds);
	}
	cmdreq.flags = CMD_REQ_COMMIT;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;
	mipi_dsi_cmdlist_put(&cmdreq);

	mipi_hitachi_manufacture_id(mfd);

	return 0;
}

static int mipi_hitachi_lcd_off(struct platform_device *pdev)
{
	struct dcs_cmd_req cmdreq;
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;

	mfd = platform_get_drvdata(pdev);
	mipi = &mfd->panel_info.mipi;

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	if (mipi->mode == DSI_VIDEO_MODE) {
		cmdreq.cmds = hitachi_videomode_off_cmds;
		cmdreq.cmds_cnt = ARRAY_SIZE(hitachi_videomode_off_cmds);
	} else if (mipanel_id()) {
		cmdreq.cmds = hitachi_hitachi_off_cmds;
		cmdreq.cmds_cnt = ARRAY_SIZE(hitachi_hitachi_off_cmds);
	} else {
		cmdreq.cmds = hitachi_sharp_off_cmds;
		cmdreq.cmds_cnt = ARRAY_SIZE(hitachi_sharp_off_cmds);
	}
	cmdreq.flags = CMD_REQ_COMMIT;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;
	mipi_dsi_cmdlist_put(&cmdreq);

#if defined(CONFIG_LEDS_LM3530_ARIES)
	backlight_brightness_set(0);
#endif

	return 0;
}

static int mipi_hitachi_lcd_probe(struct platform_device *pdev)
{
	if (pdev->id == 0) {
		mipi_hitachi_pdata = pdev->dev.platform_data;
		return 0;
	}

	msm_fb_add_device(pdev);

	return 0;
}

static void mipi_hitachi_set_backlight(struct msm_fb_data_type *mfd)
{
	int ret = -EPERM;
	int bl_level;

	bl_level = mfd->bl_level;

#if defined(CONFIG_LEDS_LM3530_ARIES)
	backlight_brightness_set(bl_level);
	return;
#endif

	if (mipi_hitachi_pdata && mipi_hitachi_pdata->pmic_backlight)
		ret = mipi_hitachi_pdata->pmic_backlight(bl_level);
	else
		pr_err("%s(): Backlight level set failed\n", __func__);
}

static struct platform_driver this_driver = {
	.probe = mipi_hitachi_lcd_probe,
	.driver = {
		   .name = "mipi_hitachi",
		   },
};

static struct msm_fb_panel_data hitachi_panel_data = {
	.on = mipi_hitachi_lcd_on,
	.off = mipi_hitachi_lcd_off,
	.set_backlight = mipi_hitachi_set_backlight,
};

static int ch_used[3];

int mipi_hitachi_device_register(struct msm_panel_info *pinfo,
				 u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_hitachi", (panel << 8) | channel);
	if (!pdev)
		return -ENOMEM;

	hitachi_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &hitachi_panel_data,
				       sizeof(hitachi_panel_data));
	if (ret) {
		pr_err("%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		pr_err("%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}
	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

static int __init mipi_hitachi_lcd_init(void)
{
	mipi_dsi_buf_alloc(&hitachi_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&hitachi_rx_buf, DSI_BUF_SIZE);

	return platform_driver_register(&this_driver);
}

module_init(mipi_hitachi_lcd_init);
