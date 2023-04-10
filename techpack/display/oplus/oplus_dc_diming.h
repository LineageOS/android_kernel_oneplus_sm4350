/***************************************************************
** Copyright (C),  2020,  oplus Mobile Comm Corp.,  Ltd
** File : oplus_dc_diming.h
** Description : oplus dc_diming feature
** Version : 1.0
******************************************************************/
#ifndef _OPLUS_DC_DIMING_H_
#define _OPLUS_DC_DIMING_H_

#include <drm/drm_connector.h>

#include "dsi_panel.h"
#include "dsi_defs.h"
#include "oplus_display_panel_hbm.h"

int sde_connector_update_backlight(struct drm_connector *connector, bool post);

int sde_connector_update_hbm(struct drm_connector *connector);

int oplus_seed_bright_to_alpha(int brightness);

struct dsi_panel_cmd_set *oplus_dsi_update_seed_backlight(
	struct dsi_panel *panel, int brightness,
	enum dsi_cmd_set_type type);
int oplus_display_panel_get_dim_alpha(void *buf);
int oplus_display_panel_set_dim_alpha(void *buf);
int oplus_display_panel_get_dim_dc_alpha(void *buf);
#endif /*_OPLUS_DC_DIMING_H_*/
