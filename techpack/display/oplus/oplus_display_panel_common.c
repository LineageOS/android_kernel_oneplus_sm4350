/***************************************************************
** Copyright (C),  2020,  OPLUS Mobile Comm Corp.,  Ltd
** File : oplus_display_panel_common.c
** Description : oplus display panel common feature
** Version : 1.0
******************************************************************/
#include "oplus_display_panel_common.h"

int oplus_debug_max_brightness = 0;
int oplus_cabc_status = OPLUS_DISPLAY_CABC_UI;
extern int lcd_closebl_flag;
EXPORT_SYMBOL(oplus_debug_max_brightness);

extern int dsi_display_read_panel_reg(struct dsi_display *display, u8 cmd,
				      void *data, size_t len);
int oplus_cabc_mode = 1;
int oplus_cabc_mode_backup = 1;
int oplus_cabc_lock_flag = 0;

DEFINE_MUTEX(oplus_display_cabc_lock);

extern u32 oplus_last_backlight;

int oplus_display_panel_get_id(void *buf)
{
	struct dsi_display *display = get_main_display();
	int ret = 0;
	unsigned char read[30];
	struct panel_id *panel_rid = buf;

	if (get_oplus_display_power_status() == OPLUS_DISPLAY_POWER_ON) {
		if (display == NULL) {
			printk(KERN_INFO "oplus_display_get_panel_id and main display is null");
			ret = -1;
			return ret;
		}

		ret = dsi_display_read_panel_reg(display, 0xDA, read, 1);

		if (ret < 0) {
			pr_err("failed to read DA ret=%d\n", ret);
			return -EINVAL;
		}

		panel_rid->DA = (uint32_t)read[0];

		ret = dsi_display_read_panel_reg(display, 0xDB, read, 1);

		if (ret < 0) {
			pr_err("failed to read DB ret=%d\n", ret);
			return -EINVAL;
		}

		panel_rid->DB = (uint32_t)read[0];

		ret = dsi_display_read_panel_reg(display, 0xDC, read, 1);

		if (ret < 0) {
			pr_err("failed to read DC ret=%d\n", ret);
			return -EINVAL;
		}

		panel_rid->DC = (uint32_t)read[0];

	} else {
		printk(KERN_ERR
		       "%s oplus_display_get_panel_id, but now display panel status is not on\n",
		       __func__);
		return -EINVAL;
	}

	return ret;
}

int oplus_display_panel_get_max_brightness(void *buf)
{
	uint32_t *max_brightness = buf;
	struct dsi_display *display = get_main_display();

	if (oplus_debug_max_brightness == 0) {
		(*max_brightness) = display->panel->bl_config.brightness_normal_max_level;
	} else {
		(*max_brightness) = oplus_debug_max_brightness;
	}

	return 0;
}

int oplus_display_panel_set_max_brightness(void *buf)
{
	uint32_t *max_brightness = buf;

	oplus_debug_max_brightness = (*max_brightness);

	return 0;
}

int oplus_display_panel_get_vendor(void *buf)
{
	struct panel_info *p_info = buf;
	struct dsi_display *display = get_main_display();
	char *vendor = (char *)display->panel->oplus_priv.vendor_name;
	char *manu_name = (char *)display->panel->oplus_priv.manufacture_name;

	if (!display || !display->panel ||
			!display->panel->oplus_priv.vendor_name ||
			!display->panel->oplus_priv.manufacture_name) {
		pr_err("failed to config lcd proc device");
		return -EINVAL;
	}

	memcpy(p_info->version, vendor,
	       strlen(vendor) > 31 ? 31 : (strlen(vendor) + 1));
	memcpy(p_info->manufacture, manu_name,
	       strlen(manu_name) > 31 ? 31 : (strlen(manu_name) + 1));

	return 0;
}

int oplus_display_panel_get_ccd_check(void *buf)
{
	struct dsi_display *display = get_main_display();
	struct mipi_dsi_device *mipi_device;
	int rc = 0;
	unsigned int *ccd_check = buf;

	if (!display || !display->panel) {
		pr_err("failed for: %s %d\n", __func__, __LINE__);
		return -EINVAL;
	}

	if (get_oplus_display_power_status() != OPLUS_DISPLAY_POWER_ON) {
		printk(KERN_ERR"%s display panel in off status\n", __func__);
		return -EFAULT;
	}

	if (display->panel->panel_mode != DSI_OP_CMD_MODE) {
		pr_err("only supported for command mode\n");
		return -EFAULT;
	}

	if (!(display && display->panel->oplus_priv.vendor_name) ||
			!strcmp(display->panel->oplus_priv.vendor_name, "NT37800")) {
		(*ccd_check) = 0;
		goto end;
	}

	mipi_device = &display->panel->mipi_device;

	mutex_lock(&display->display_lock);
	mutex_lock(&display->panel->panel_lock);

	if (!dsi_panel_initialized(display->panel)) {
		rc = -EINVAL;
		goto unlock;
	}

	rc = dsi_display_cmd_engine_enable(display);

	if (rc) {
		pr_err("%s, cmd engine enable failed\n", __func__);
		goto unlock;
	}

	/* enable the clk vote for CMD mode panels */
	if (display->config.panel_mode == DSI_OP_CMD_MODE) {
		dsi_display_clk_ctrl(display->dsi_clk_handle,
				     DSI_CORE_CLK, DSI_CLK_ON);
	}

	if (!strcmp(display->panel->oplus_priv.vendor_name, "AMB655UV01")) {
		{
			char value[] = { 0x5A, 0x5A };
			rc = mipi_dsi_dcs_write(mipi_device, 0xF0, value, sizeof(value));
		}
		{
			char value[] = { 0x44, 0x50 };
			rc = mipi_dsi_dcs_write(mipi_device, 0xE7, value, sizeof(value));
		}
		usleep_range(1000, 1100);
		{
			char value[] = { 0x03 };
			rc = mipi_dsi_dcs_write(mipi_device, 0xB0, value, sizeof(value));
		}

	} else {
		{
			char value[] = { 0x5A, 0x5A };
			rc = mipi_dsi_dcs_write(mipi_device, 0xF0, value, sizeof(value));
		}
		{
			char value[] = { 0x02 };
			rc = mipi_dsi_dcs_write(mipi_device, 0xB0, value, sizeof(value));
		}
		{
			char value[] = { 0x44, 0x50 };
			rc = mipi_dsi_dcs_write(mipi_device, 0xCC, value, sizeof(value));
		}
		usleep_range(1000, 1100);
		{
			char value[] = { 0x05 };
			rc = mipi_dsi_dcs_write(mipi_device, 0xB0, value, sizeof(value));
		}
	}

	if (display->config.panel_mode == DSI_OP_CMD_MODE) {
		rc = dsi_display_clk_ctrl(display->dsi_clk_handle,
					  DSI_CORE_CLK, DSI_CLK_OFF);
	}

	dsi_display_cmd_engine_disable(display);

	mutex_unlock(&display->panel->panel_lock);
	mutex_unlock(&display->display_lock);

	if (!strcmp(display->panel->oplus_priv.vendor_name, "AMB655UV01")) {
		{
			unsigned char read[10];

			rc = dsi_display_read_panel_reg(display, 0xE1, read, 1);

			pr_err("read ccd_check value = 0x%x rc=%d\n", read[0], rc);
			(*ccd_check) = read[0];
		}

	} else {
		{
			unsigned char read[10];

			rc = dsi_display_read_panel_reg(display, 0xCC, read, 1);

			pr_err("read ccd_check value = 0x%x rc=%d\n", read[0], rc);
			(*ccd_check) = read[0];
		}
	}

	mutex_lock(&display->display_lock);
	mutex_lock(&display->panel->panel_lock);

	if (!dsi_panel_initialized(display->panel)) {
		rc = -EINVAL;
		goto unlock;
	}

	rc = dsi_display_cmd_engine_enable(display);

	if (rc) {
		pr_err("%s, cmd engine enable failed\n", __func__);
		goto unlock;
	}

	if (display->config.panel_mode == DSI_OP_CMD_MODE) {
		dsi_display_clk_ctrl(display->dsi_clk_handle,
				     DSI_CORE_CLK, DSI_CLK_ON);
	}

	{
		char value[] = { 0xA5, 0xA5 };
		rc = mipi_dsi_dcs_write(mipi_device, 0xF0, value, sizeof(value));
	}

	if (display->config.panel_mode == DSI_OP_CMD_MODE) {
		rc = dsi_display_clk_ctrl(display->dsi_clk_handle,
					  DSI_CORE_CLK, DSI_CLK_OFF);
	}

	dsi_display_cmd_engine_disable(display);
unlock:

	mutex_unlock(&display->panel->panel_lock);
	mutex_unlock(&display->display_lock);
end:
	pr_err("[%s] ccd_check = %d\n",  display->panel->oplus_priv.vendor_name,
	       (*ccd_check));
	return 0;
}

int oplus_display_panel_get_serial_number(void *buf)
{
	int ret = 0, i;
	unsigned char read[30] = {0};
	struct panel_serial_number *panel_rnum = buf;
	struct dsi_display *display = get_main_display();
	unsigned base_index = 10;

	panel_rnum->date = 0;
	panel_rnum->precise_time = 0;

	if (!display) {
		printk(KERN_INFO
		       "oplus_display_get_panel_serial_number and main display is null");
		return -1;
	}

	if (get_oplus_display_power_status() != OPLUS_DISPLAY_POWER_ON) {
		printk(KERN_ERR"%s display panel in off status\n", __func__);
		return ret;
	}

	/*
	 * for some unknown reason, the panel_serial_info may read dummy,
	 * retry when found panel_serial_info is abnormal.
	 */
	for (i = 0; i < 10; i++) {
		ret = dsi_display_read_panel_reg(get_main_display(), 0xA1, read, 17);

		if (ret < 0) {
			ret = scnprintf(buf, PAGE_SIZE,
					"Get panel serial number failed, reason:%d", ret);
			msleep(20);
			continue;
		}

		/*  0xA1			   11th		12th	13th	14th	15th
		 *  HEX				0x32		0x0C	0x0B	0x29	0x37
		 *  Bit		   [D7:D4][D3:D0] [D5:D0] [D5:D0] [D5:D0] [D5:D0]
		 *  exp			  3	  2	   C	   B	   29	  37
		 *  Yyyy,mm,dd	  2014   2m	  12d	 11h	 41min   55sec
		 *  panel_rnum.data[24:21][20:16] [15:8]  [7:0]
		 *  panel_rnum:precise_time					  [31:24] [23:16] [reserved]
		*/
		panel_rnum->date =
			(((read[base_index] & 0xF0) << 20)
			 | ((read[base_index] & 0x0F) << 16)
			 | ((read[base_index + 1] & 0x1F) << 8)
			 | (read[base_index + 2] & 0x1F));

		panel_rnum->precise_time =
			(((read[base_index + 3] & 0x3F) << 24)
			 | ((read[base_index + 4] & 0x3F) << 16)
			 | (read[base_index + 5] << 8)
			 | (read[base_index + 6]));

		if (!read[base_index]) {
			/*
			 * the panel we use always large than 2011, so
			 * force retry when year is 2011
			 */
			msleep(20);
			continue;
		}

		break;
	}

	return 0;
}
int oplus_display_get_cabc_status(void *buf)
{
	uint32_t *cabc_status = buf;
	struct dsi_display *display = NULL;
	struct dsi_panel *panel = NULL;

	display = get_main_display();
	if (!display) {
		DSI_ERR("No display device\n");
		return -ENODEV;
	}

	panel = display->panel;
	if (!panel) {
		DSI_ERR("No panel device\n");
		return -ENODEV;
	}

	if(panel->oplus_priv.cabc_enabled) {
		*cabc_status = oplus_cabc_status;
	} else {
		*cabc_status = OPLUS_DISPLAY_CABC_OFF;
	}
	return 0;
}

/*
__oplus_display_set_cabc_status:

@mode: CABC mode to be set
Returns: 0 if no error.

Checks if the CABC is being turned on while device is locked. If it is locked and CABC is being set then return -1
*/
int __oplus_display_set_cabc_status(int mode)
{
	mutex_lock(&oplus_display_cabc_lock);

	if (mode != oplus_cabc_mode) {
		oplus_cabc_mode = mode;
	}

	if (oplus_cabc_mode == OPLUS_DISPLAY_CABC_ENTER_SPECIAL) {
		oplus_cabc_lock_flag = 1;
		oplus_cabc_mode = OPLUS_DISPLAY_CABC_OFF;
	} else if (oplus_cabc_mode == OPLUS_DISPLAY_CABC_EXIT_SPECIAL) {
		oplus_cabc_lock_flag = 0;
		oplus_cabc_mode = oplus_cabc_mode_backup;
	} else {
		oplus_cabc_mode_backup = oplus_cabc_mode;
	}

	mutex_unlock(&oplus_display_cabc_lock);
	printk("%s,oplus_cabc mode is %d, oplus_cabc_mode_backup is %d, oplus_lock_mode is %d\n",
               __func__, oplus_cabc_mode, oplus_cabc_mode_backup, oplus_cabc_lock_flag);
	if (oplus_cabc_mode == oplus_cabc_mode_backup && oplus_cabc_lock_flag) {
		printk("%s locked, nothing to do\n", __func__);			/*If device is locked, don't send CABC*/
		return -1;
	}
	return 0;
}

int oplus_display_set_cabc_status(void *buf)
{
	int rc = 0;
	uint32_t *cabc_status = buf;
	struct dsi_display *display = NULL;
	struct dsi_panel *panel = NULL;

	DSI_ERR("oplus_display_set_cabc_status *cabc_status=%d\n", *cabc_status);

	display = get_main_display();
	if (!display) {
		DSI_ERR("No display device\n");
		return -ENODEV;
	}

	panel = display->panel;
	if (!panel) {
		DSI_ERR("No panel device\n");
		return -ENODEV;
	}

	if (display->panel->panel_type == DSI_DISPLAY_PANEL_TYPE_OLED) {		/*If Panel is OLED don't send CABC commands*/
                printk("oled cabc no need!\n");
                return -EINVAL;
        }

	if (!panel->oplus_priv.cabc_enabled) {
		DSI_ERR("This project don't support cabc\n");
		return -EFAULT;
	}

	if (*cabc_status >= OPLUS_DISPLAY_CABC_UNKNOWN) {
		DSI_ERR("Unknown cabc status = [%d]\n", *cabc_status);
		return -EINVAL;
	}

	if(__oplus_display_set_cabc_status(*cabc_status)) {
		return rc;
	}

	*cabc_status = oplus_cabc_mode;

	if(get_oplus_display_power_status() == OPLUS_DISPLAY_POWER_ON) {
			if (oplus_last_backlight == 0) {				/*If backlight is 0 don't send CABC commands*/
				printk(KERN_INFO "backlight is 0, not send cabc cmd\n");
				return rc;
			}

			if (*cabc_status == OPLUS_DISPLAY_CABC_OFF) {
				rc = dsi_panel_tx_cmd_set(panel, DSI_CMD_CABC_OFF);
				if (rc) {
					DSI_ERR("[%s] failed to send DSI_CMD_CABC_OFF cmds, rc=%d\n",
							panel->name, rc);
				}
			} else if (*cabc_status == OPLUS_DISPLAY_CABC_UI) {
				rc = dsi_panel_tx_cmd_set(panel, DSI_CMD_CABC_UI);
				if (rc) {
					DSI_ERR("[%s] failed to send DSI_CMD_CABC_UI cmds, rc=%d\n",
							panel->name, rc);
				}
			} else if (*cabc_status == OPLUS_DISPLAY_CABC_IMAGE) {
				rc = dsi_panel_tx_cmd_set(panel, DSI_CMD_CABC_IMAGE);
				if (rc) {
					DSI_ERR("[%s] failed to send DSI_CMD_CABC_IMAGE cmds, rc=%d\n",
							panel->name, rc);
				}
			}  else if (*cabc_status == OPLUS_DISPLAY_CABC_VIDEO) {
				rc = dsi_panel_tx_cmd_set(panel, DSI_CMD_CABC_VIDEO);
				if (rc) {
					DSI_ERR("[%s] failed to send DSI_CMD_CABC_VIDEO cmds, rc=%d\n",
							panel->name, rc);
				}
			}
		oplus_cabc_status = *cabc_status;
		pr_err("debug for %s, buf = [%s], oplus_cabc_status = %d\n",
				__func__, buf, oplus_cabc_status);
	} else {
		pr_err("debug for %s, buf = [%s], but display panel status is not on!\n",
				__func__, *cabc_status);
	}
	return rc;
}

int oplus_display_panel_get_closebl_flag(void *data)
{
	uint32_t *closebl_flag = data;

	(*closebl_flag) = lcd_closebl_flag;
	pr_err("oplus_display_get_closebl_flag = %d\n", lcd_closebl_flag);

	return 0;
}

int oplus_display_panel_set_closebl_flag(void *data)
{
	uint32_t *closebl = data;

	pr_err("lcd_closebl_flag = %d\n", (*closebl));
	if (1 != (*closebl))
		lcd_closebl_flag = 0;
	pr_err("oplus_display_set_closebl_flag = %d\n", lcd_closebl_flag);

	return 0;
}
