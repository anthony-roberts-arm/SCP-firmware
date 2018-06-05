/*
 * Arm SCP/MCP Software
 * Copyright (c) 2015-2018, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>
#include <fwk_element.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>
#include <mod_reg_sensor.h>
#include <mod_sensor.h>
#include <system_mmap.h>

enum REG_SENSOR_DEVICES {
    REG_SENSOR_DEV_SOC_TEMP,
    REG_SENSOR_DEV_COUNT,
};

/*
 * Register Sensor driver config
 */
static const struct fwk_element reg_sensor_element_table[] = {
    [REG_SENSOR_DEV_SOC_TEMP] = {
        .name = "Soc Temperature",
        .data = &((struct mod_reg_sensor_dev_config) {
            .reg = (uintptr_t)(SENSOR_SOC_TEMP),
        }),
    },
    [REG_SENSOR_DEV_COUNT] = {},
};

static const struct fwk_element *get_reg_sensor_element_table(fwk_id_t id)
{
    return reg_sensor_element_table;
}

struct fwk_module_config config_reg_sensor = {
    .get_element_table = get_reg_sensor_element_table,
};


/*
 * Sensor module config
 */
static const struct mod_sensor_dev_config soctemp_config = {
    .driver_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_REG_SENSOR,
                                REG_SENSOR_DEV_SOC_TEMP),
    .info = &((struct mod_sensor_info) {
        .type = MOD_SENSOR_TYPE_DEGREES_C,
        .update_interval = 0,
        .update_interval_multiplier = 0,
        .unit_multiplier = 0,
    }),
};

static const struct fwk_element sensor_element_table[] = {
    [0] = {
        .name = "Soc Temperature",
        .data = &soctemp_config,
    },
    [1] = {},
};

static const struct fwk_element *get_sensor_element_table(fwk_id_t module_id)
{
    return sensor_element_table;
}

struct fwk_module_config config_sensor = {
    .get_element_table = get_sensor_element_table,
    .data = NULL,
};
