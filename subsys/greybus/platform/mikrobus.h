/*
 * Copyright (c) 2021 Vaishnav M A. BeagleBoard.org Foundation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SUBSYS_GREYBUS_PLATFORM_MIKROBUS_H_
#define SUBSYS_GREYBUS_PLATFORM_MIKROBUS_H_

#include <stddef.h>
#include <stdint.h>

#ifdef CONFIG_GREYBUS_MIKROBUS
int manifest_get_fragment_clickid(uint8_t **mnfb, size_t *mnfb_size, uint8_t id);
#else
static inline int manifest_get_fragment_clickid(uint8_t **mnfb, size_t *mnfb_size, uint8_t id)
{
    return -ENOTSUP;
}
#endif /* CONFIG_GREYBUS_MIKROBUS */

#endif /* SUBSYS_GREYBUS_PLATFORM_MIKROBUS_H_ */
