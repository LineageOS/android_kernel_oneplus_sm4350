/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2017-2019, The Linux Foundation. All rights reserved.
 */

#ifndef _OPLUS_CAM_FLASH_DEV_H_
#define _OPLUS_CAM_FLASH_DEV_H_

#include <linux/leds.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/leds-qpnp-flash.h>
#include <media/v4l2-subdev.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-event.h>
#include <media/cam_sensor.h>
#include <media/cam_req_mgr.h>
#include "cam_req_mgr_util.h"
#include "cam_req_mgr_interface.h"
#include "cam_subdev.h"
#include "cam_mem_mgr.h"
#include "cam_sensor_cmn_header.h"
#include "cam_soc_util.h"
#include "cam_debug_util.h"
#include "cam_sensor_io.h"
#include "cam_flash_core.h"
#include "cam_context.h"

typedef enum {
	EXT_NONE=-1,
	EXT_LDO1,
	EXT_LDO2,
	EXT_LDO3,
	EXT_LDO4,
	EXT_LDO5,
	EXT_LDO6,
	EXT_LDO7,
	EXT_MAX
} EXT_SELECT;
extern int wl2868c_ldo_enable(EXT_SELECT ldonum,unsigned int value);
extern int wl2868c_ldo_disable(EXT_SELECT ldonum,unsigned int value);

void oplus_cam_flash_proc_init(struct cam_flash_ctrl *flash_ctl, struct platform_device *pdev);
int cam_flash_on(struct cam_flash_ctrl *flash_ctrl,
	struct cam_flash_frame_setting *flash_data,
	int mode);

#endif /*_CAM_FLASH_DEV_H_*/
