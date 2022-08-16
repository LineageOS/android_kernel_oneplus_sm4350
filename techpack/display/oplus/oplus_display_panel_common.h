/***************************************************************
** Copyright (C),  2020,  OPLUS Mobile Comm Corp.,  Ltd
** File : oplus_display_panel_common.c
** Description : oplus display panel common feature
** Version : 1.0
******************************************************************/
#ifndef _OPLUS_DISPLAY_PANEL_COMMON_H_
#define _OPLUS_DISPLAY_PANEL_COMMON_H_

#include <linux/err.h>
#include "dsi_display.h"
#include "dsi_panel.h"
#include "dsi_ctrl.h"
#include "dsi_ctrl_hw.h"
#include "dsi_drm.h"
#include "dsi_clk.h"
#include "dsi_pwr.h"
#include "sde_dbg.h"

struct panel_id {
	uint32_t DA;
	uint32_t DB;
	uint32_t DC;
};

struct panel_info {
	char version[32];
	char manufacture[32];
};

struct panel_serial_number {
	uint32_t date;		 /*year:month:day:hour  8bit uint*/
	uint32_t precise_time; /* minute:second:reserved:reserved 8bit uint*/
};

int oplus_display_panel_get_id(void *buf);
int oplus_display_panel_get_max_brightness(void *buf);
int oplus_display_panel_set_max_brightness(void *buf);
int oplus_display_panel_get_vendor(void *buf);
int oplus_display_panel_get_ccd_check(void *buf);
int oplus_display_panel_get_serial_number(void *buf);
int oplus_display_panel_get_closebl_flag(void *data);
int oplus_display_panel_set_closebl_flag(void *data);
int __oplus_display_set_cabc_status(int mode);
int oplus_display_set_cabc_status(void *buf);
int oplus_display_get_cabc_status(void *buf);
#endif /*_OPLUS_DISPLAY_PANEL_COMMON_H_*/

