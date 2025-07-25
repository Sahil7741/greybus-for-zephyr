/*
 * Copyright (c) 2014-2015 Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "greybus-manifest.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <zephyr/sys/dlist.h>

#include <zephyr/sys/byteorder.h>
#include <greybus-utils/utils.h>
// #include <nuttx/util.h>

#if defined(CONFIG_BOARD_NATIVE_POSIX_64BIT) || defined(CONFIG_BOARD_NATIVE_POSIX_32BIT) ||        \
	defined(CONFIG_BOARD_NRF52_BSIM)
#include <semaphore.h>
#else
#include <zephyr/posix/semaphore.h>
#endif

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(greybus_manifest, CONFIG_GREYBUS_LOG_LEVEL);

#undef ALIGN
#undef PAD
#define PAD(x, pot) (((x) & (pot - 1)) ? (((x) & ~(pot - 1)) + pot) : (x))
// "align" (really pad) to a 4-byte boundary
#define ALIGN(x)    PAD(x, 4)

// not yet a part of zephyr minimal libc??
extern char *strtok(char *str, const char *delim);

extern void gb_control_register(int cport, int bundle);
extern void gb_gpio_register(int cport, int bundle);
extern void gb_i2c_register(int cport, int bundle);
extern void gb_power_supply_register(int cport, int bundle);
extern void gb_loopback_register(int cport, int bundle);
extern void gb_vibrator_register(int cport, int bundle);
extern void gb_usb_register(int cport, int bundle);
extern void gb_pwm_register(int cport, int bundle);
extern void gb_spi_register(int cport, int bundle);
extern void gb_uart_register(int cport, int bundle);
extern void gb_hid_register(int cport, int bundle);
extern void gb_lights_register(int cport, int bundle);
extern void gb_sdio_register(int cport, int bundle);
extern void gb_camera_register(int cport, int bundle);
extern void gb_audio_mgmt_register(int cport, int bundle);
extern void gb_audio_data_register(int cport, int bundle);

struct greybus {
	sys_dlist_t cports;
	struct greybus_driver *drv;
	size_t max_bundle_id;
	size_t max_property_id;
	size_t max_device_id;
	size_t max_string_id;
	size_t max_patch_property_id;
	size_t max_patch_device_id;
	size_t max_patch_string_id;
};

static struct greybus g_greybus = {
	.cports = SYS_DLIST_STATIC_INIT(&g_greybus.cports),
	.max_bundle_id = 0,
	.max_device_id = 0,
	.max_string_id = 0,
	.max_property_id = 0,
	.max_patch_property_id = 0,
	.max_patch_device_id = 0,
	.max_patch_string_id = 0,
};

#ifdef CONFIG_GREYBUS_STATIC_MANIFEST
static unsigned char *bridge_manifest = (unsigned char *)manifest_mnfb;
#else
static unsigned char *bridge_manifest;
#endif

static void *alloc_cport(void)
{
	struct gb_cport *gb_cport;

	gb_cport = malloc(sizeof(struct gb_cport));
	if (!gb_cport) {
		return NULL;
	}

	sys_dlist_append(&g_greybus.cports, &gb_cport->node);
	return gb_cport;
}

static void free_cport(int cportid)
{
	struct gb_cport *gb_cport, *gb_cport_safe;

	SYS_DLIST_FOR_EACH_CONTAINER_SAFE(&g_greybus.cports, gb_cport, gb_cport_safe, node) {
		if (gb_cport->id == cportid) {
			sys_dlist_remove(&gb_cport->node);
			free(gb_cport);
		}
	}
}

#ifdef CONFIG_GREYBUS
void enable_cports(void)
{
	struct gb_cport *gb_cport;
	__attribute__((unused)) int cport_id;
	__attribute__((unused)) int bundle_id;
	__attribute__((unused)) int protocol;

	SYS_DLIST_FOR_EACH_CONTAINER(&g_greybus.cports, gb_cport, node) {
		cport_id = gb_cport->id;
		bundle_id = gb_cport->bundle;
		protocol = gb_cport->protocol;

#ifdef CONFIG_GREYBUS_CONTROL
		if (protocol == GREYBUS_PROTOCOL_CONTROL) {
			LOG_INF("Registering CONTROL greybus driver.");
			gb_control_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_GPIO
		if (protocol == GREYBUS_PROTOCOL_GPIO) {
			LOG_INF("Registering GPIO greybus driver.");
			gb_gpio_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_I2C
		if (protocol == GREYBUS_PROTOCOL_I2C) {
			LOG_INF("Registering I2C greybus driver.");
			gb_i2c_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_POWER_SUPPLY
		if (protocol == GREYBUS_PROTOCOL_POWER_SUPPLY) {
			LOG_INF("Registering POWER_SUPPLY greybus driver.");
			gb_power_supply_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_LOOPBACK
		if (protocol == GREYBUS_PROTOCOL_LOOPBACK) {
			LOG_INF("Registering Loopback greybus driver.");
			gb_loopback_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_VIBRATOR
		if (protocol == GREYBUS_PROTOCOL_VIBRATOR) {
			LOG_INF("Registering VIBRATOR greybus driver.");
			gb_vibrator_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_USB_HOST
		if (protocol == GREYBUS_PROTOCOL_USB) {
			LOG_INF("Registering USB greybus driver.");
			gb_usb_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_PWM
		if (protocol == GREYBUS_PROTOCOL_PWM) {
			LOG_INF("Registering PWM greybus driver.");
			gb_pwm_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_SPI
		if (protocol == GREYBUS_PROTOCOL_SPI) {
			LOG_INF("Registering SPI greybus driver.");
			gb_spi_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_UART
		if (protocol == GREYBUS_PROTOCOL_UART) {
			LOG_INF("Registering Uart greybus driver. id= %d", cport_id);
			gb_uart_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_HID
		if (protocol == GREYBUS_PROTOCOL_HID) {
			LOG_INF("Registering HID greybus driver. id= %d", cport_id);
			gb_hid_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_LIGHTS
		if (protocol == GREYBUS_PROTOCOL_LIGHTS) {
			LOG_INF("Registering Lights greybus driver. id= %d", cport_id);
			gb_lights_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_SDIO
		if (protocol == GREYBUS_PROTOCOL_SDIO) {
			LOG_INF("Registering SDIO greybus driver.");
			gb_sdio_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_CAMERA
		if (protocol == GREYBUS_PROTOCOL_CAMERA_MGMT) {
			LOG_INF("Registering Camera greybus driver. id= %d", cport_id);
			gb_camera_register(cport_id, bundle_id);
		}
#endif

#ifdef CONFIG_GREYBUS_AUDIO
		if (protocol == GREYBUS_PROTOCOL_AUDIO_MGMT) {
			LOG_INF("Registering Audio MGMT greybus driver.");
			gb_audio_mgmt_register(cport_id, bundle_id);
		}

		if (protocol == GREYBUS_PROTOCOL_AUDIO_DATA) {
			LOG_INF("Registering Audio DATA greybus driver.");
			gb_audio_data_register(cport_id, bundle_id);
		}
#endif
	}
}
#endif

/*
 * Validate the given descriptor.  Its reported size must fit within
 * the number of bytes reamining, and it must have a recognized
 * type.  Check that the reported size is at least as big as what
 * we expect to see.  (It could be bigger, perhaps for a new version
 * of the format.)
 *
 * Returns the number of bytes consumed by the descriptor, or a
 * negative errno.
 */
static int identify_descriptor(struct greybus_descriptor *desc, size_t size, int release)
{
	struct greybus_descriptor_header *desc_header = &desc->header;
	size_t expected_size;
	size_t desc_size;
	struct gb_cport *cport;

	if (size < sizeof(*desc_header)) {
		LOG_ERR("manifest too small");
		return -EINVAL; /* Must at least have header */
	}

	desc_size = (int)sys_le16_to_cpu(desc_header->size);
	if ((size_t)desc_size > size) {
		LOG_ERR("descriptor too big");
		return -EINVAL;
	}

	/* Descriptor needs to at least have a header */
	expected_size = sizeof(*desc_header);

	switch (desc_header->type) {
	case GREYBUS_TYPE_STRING:
		expected_size += sizeof(struct greybus_descriptor_string);
		expected_size += desc->string.length;
		g_greybus.max_string_id = MAX(g_greybus.max_string_id, desc->string.id);
		/* String descriptors are padded to 4 byte boundaries */
		expected_size = ALIGN(expected_size);
		break;
	case GREYBUS_TYPE_INTERFACE:
		expected_size += sizeof(struct greybus_descriptor_interface);
		break;
	case GREYBUS_TYPE_BUNDLE:
		expected_size += sizeof(struct greybus_descriptor_bundle);
		g_greybus.max_bundle_id = MAX(g_greybus.max_bundle_id, desc->bundle.id);
		break;
	case GREYBUS_TYPE_CPORT:
		expected_size += sizeof(struct greybus_descriptor_cport);
		if (desc_size >= expected_size) {
			if (!release) {
				cport = alloc_cport();
				if (!cport) {
					return -ENOMEM;
				}

				cport->id = desc->cport.id;
				cport->bundle = desc->cport.bundle;
				cport->protocol = desc->cport.protocol_id;
				LOG_DBG("cport_id = %d", cport->id);
			} else {
				free_cport(desc->cport.id);
			}
		}
		break;
	case GREYBUS_TYPE_PROPERTY:
		expected_size += sizeof(struct greybus_descriptor_property);
		expected_size += desc->property.length;
		g_greybus.max_property_id = MAX(g_greybus.max_property_id, desc->property.id);
		expected_size = ALIGN(expected_size);
		break;
	case GREYBUS_TYPE_DEVICE:
		expected_size += sizeof(struct greybus_descriptor_device);
		g_greybus.max_device_id = MAX(g_greybus.max_device_id, desc->device.id);
		break;
#ifdef CONFIG_GREYBUS_MIKROBUS
	case GREYBUS_TYPE_MIKROBUS:
		expected_size += sizeof(struct greybus_descriptor_mikrobus);
		break;
#endif
	case GREYBUS_TYPE_INVALID:
	default:
		LOG_ERR("invalid descriptor type (%hhu)", desc_header->type);
		return -EINVAL;
	}
	if (desc_size < expected_size) {
		LOG_ERR("%d: descriptor too small (%zu < %zu)", desc_header->type, desc_size,
			expected_size);
		return -EINVAL;
	}

	/* Descriptor bigger than what we expect */
	if (desc_size > expected_size) {
		LOG_ERR("%d descriptor size mismatch (want %zu got %zu)", desc_header->type,
			expected_size, desc_size);
	}

	return desc_size;
}

static int identify_patch_descriptor(struct greybus_descriptor *desc, size_t size)
{
	struct greybus_descriptor_header *desc_header = &desc->header;
	size_t expected_size;
	size_t desc_size;
	int skip = 0;

	if (size < sizeof(*desc_header)) {
		LOG_ERR("manifest too small");
		return -EINVAL; /* Must at least have header */
	}

	desc_size = (int)sys_le16_to_cpu(desc_header->size);
	if ((size_t)desc_size > size) {
		LOG_ERR("descriptor too big");
		return -EINVAL;
	}

	/* Descriptor needs to at least have a header */
	expected_size = sizeof(*desc_header);
	switch (desc_header->type) {
	case GREYBUS_TYPE_STRING:
		expected_size += sizeof(struct greybus_descriptor_string);
		expected_size += desc->string.length;
		desc->string.id += g_greybus.max_string_id;
		g_greybus.max_patch_string_id = MAX(g_greybus.max_patch_string_id, desc->string.id);
		expected_size = ALIGN(expected_size);
		break;
	case GREYBUS_TYPE_INTERFACE:
		expected_size += sizeof(struct greybus_descriptor_interface);
		skip = 1;
		break;
	case GREYBUS_TYPE_BUNDLE:
		expected_size += sizeof(struct greybus_descriptor_bundle);
		skip = 1;
		break;
	case GREYBUS_TYPE_CPORT:
		expected_size += sizeof(struct greybus_descriptor_cport);
		skip = 1;
		break;
	case GREYBUS_TYPE_PROPERTY:
		expected_size += sizeof(struct greybus_descriptor_property);
		expected_size += desc->property.length;
		desc->property.id += g_greybus.max_property_id;
		desc->property.propname_stringid += g_greybus.max_string_id;
		g_greybus.max_patch_property_id =
			MAX(g_greybus.max_patch_property_id, desc->property.id);
		expected_size = ALIGN(expected_size);
		break;
	case GREYBUS_TYPE_DEVICE:
		expected_size += sizeof(struct greybus_descriptor_device);
		desc->device.id += g_greybus.max_device_id;
		desc->device.driver_stringid += g_greybus.max_string_id;
		g_greybus.max_patch_device_id = MAX(g_greybus.max_patch_device_id, desc->device.id);
		break;
#ifdef CONFIG_GREYBUS_MIKROBUS
	case GREYBUS_TYPE_MIKROBUS:
		expected_size += sizeof(struct greybus_descriptor_mikrobus);
		break;
#endif
	case GREYBUS_TYPE_INVALID:
	default:
		LOG_ERR("invalid descriptor type (%hhu)", desc_header->type);
		return -EINVAL;
	}

	if (desc_size < expected_size) {
		LOG_ERR("%d: descriptor too small (%zu < %zu)", desc_header->type, desc_size,
			expected_size);
		return -EINVAL;
	}
	/* Descriptor bigger than what we expect */
	if (desc_size > expected_size) {
		LOG_ERR("%d descriptor size mismatch (want %zu got %zu)", desc_header->type,
			expected_size, desc_size);
	}
	if (skip) {
		return -desc_size;
	}

	return desc_size;
}

static bool _manifest_parse(void *data, size_t size, int release)
{
	struct greybus_manifest *manifest = data;
	struct greybus_manifest_header *header = &manifest->header;
	struct greybus_descriptor *desc;
	uint16_t manifest_size;

	if (!release) {
		/* we have to have at _least_ the manifest header */
		if (size <= sizeof(manifest->header)) {
			LOG_ERR("short manifest (%zu)", size);
			return false;
		}

		/* Make sure the size is right */
		manifest_size = sys_le16_to_cpu(header->size);
		if (manifest_size != size) {
			LOG_ERR("manifest size mismatch %zu != %hu", size, manifest_size);
			return false;
		}

		/* Validate major/minor number */
		if (header->version_major > GREYBUS_VERSION_MAJOR) {
			LOG_ERR("manifest version too new (%hhu.%hhu > %hhu.%hhu)",
				header->version_major, header->version_minor, GREYBUS_VERSION_MAJOR,
				GREYBUS_VERSION_MINOR);
			return false;
		}
	}

	/* OK, find all the descriptors */
	desc = (struct greybus_descriptor *)(header + 1);
	size -= sizeof(*header);
	while (size) {
		int desc_size;

		desc_size = identify_descriptor(desc, size, release);
		if (desc_size <= 0) {
			return false;
		}
		desc = (struct greybus_descriptor *)((char *)desc + desc_size);
		size -= desc_size;
	}

	return true;
}

/*
 * Parse a buffer containing a interface manifest.
 *
 * If we find anything wrong with the content/format of the buffer
 * we reject it.
 *
 * The first requirement is that the manifest's version is
 * one we can parse.
 *
 * We make an initial pass through the buffer and identify all of
 * the descriptors it contains, keeping track for each its type
 * and the location size of its data in the buffer.
 *
 * Returns true if parsing was successful, false otherwise.
 */
bool manifest_parse(void *data, size_t size)
{
	return _manifest_parse(data, size, 0);
}

/*
 * patch a buffer containing a interface manifest
 * with a manifest fragment, manifest fragment will have
 * mikrobus, device descriptor information.
 */
bool manifest_patch(uint8_t **mnfb, void *data, size_t size)
{
	struct greybus_manifest *manifest = (struct greybus_manifest *)*mnfb;
	struct greybus_manifest_header *header = &manifest->header;
	struct greybus_manifest *manifest_fragment = (struct greybus_manifest *)data;
	struct greybus_manifest_header *fragment_header = &manifest_fragment->header;
	struct greybus_descriptor *desc;
	uint16_t manifest_size = sys_le16_to_cpu(header->size);
	uint16_t manifest_fragment_size;

	/* we have to have at _least_ the manifest header */
	if (size <= sizeof(manifest_fragment->header)) {
		LOG_ERR("short manifest fragment(%zu)", size);
		return false;
	}

	/* Make sure the size is right */
	manifest_fragment_size = sys_le16_to_cpu(fragment_header->size);
	if (manifest_fragment_size != size) {
		LOG_ERR("manifest fragment size mismatch %zu != %hu", size, manifest_fragment_size);
		return false;
	}

	/* Validate major/minor number */
	if (fragment_header->version_major > GREYBUS_VERSION_MAJOR) {
		LOG_ERR("manifest fragment version too new (%hhu.%hhu > %hhu.%hhu)",
			fragment_header->version_major, fragment_header->version_minor,
			GREYBUS_VERSION_MAJOR, GREYBUS_VERSION_MINOR);
		return false;
	}
	// LOG_HEXDUMP_ERR(manifest, manifest_size, "original manifest ");
	g_greybus.max_patch_property_id = 0;
	g_greybus.max_patch_device_id = 0;
	g_greybus.max_patch_string_id = 0;
	/* OK, find all the descriptors */
	desc = (struct greybus_descriptor *)(fragment_header + 1);
	size -= sizeof(*fragment_header);
	while (size) {
		int desc_size;
		desc_size = identify_patch_descriptor(desc, size);
		if (desc_size <= 0) {
			desc_size = -desc_size;
			desc = (struct greybus_descriptor *)((char *)desc + desc_size);
			size -= desc_size;
			continue;
		}
		size -= desc_size;
		*mnfb = realloc(*mnfb, manifest_size + desc_size);
		memcpy((char *)*mnfb + manifest_size, desc, desc_size);
		manifest = (struct greybus_manifest *)*mnfb;
		header = &manifest->header;
		header->size += desc_size;
		manifest_size += desc_size;
		desc = (struct greybus_descriptor *)((char *)desc + desc_size);
	}
	g_greybus.max_property_id += g_greybus.max_patch_property_id;
	g_greybus.max_device_id += g_greybus.max_patch_device_id;
	g_greybus.max_string_id += g_greybus.max_patch_string_id;
	return true;
}

bool manifest_release(void *data, size_t size)
{
	return _manifest_parse(data, size, 1);
}

static int get_interface_id(char *fname)
{
	char *iid_str;
	int iid = 0;
	char tmp[256];

	strcpy(tmp, fname);
	iid_str = strtok(tmp, "-");
	if (!strncmp(iid_str, "IID", 3)) {
		iid = strtol(fname + 4, NULL, 0);
	}

	return iid;
}

void *get_manifest_blob(void)
{
	return bridge_manifest;
}

void set_manifest_blob(void *blob)
{
	bridge_manifest = blob;
}

void parse_manifest_blob(void *manifest)
{
	struct greybus_manifest_header *mh = manifest;

	manifest_parse(mh, sys_le16_to_cpu(mh->size));
}

void release_manifest_blob(void *manifest)
{
	struct greybus_manifest_header *mh = manifest;

	manifest_release(mh, sys_le16_to_cpu(mh->size));
}

void enable_manifest(char *name, void *manifest, int device_id)
{
	if (!manifest) {
		manifest = get_manifest_blob();
	}

	if (manifest) {
		parse_manifest_blob(manifest);
		int iid = get_interface_id(name);
		if (iid > 0) {
			LOG_INF("%s interface inserted", name);
		} else {
			LOG_ERR("invalid interface ID, no hotplug plug event sent");
		}
	} else {
		LOG_ERR("missing manifest blob, no hotplug event sent");
	}
}

void disable_manifest(char *name, void *priv, int device_id)
{
	void *manifest;

	manifest = get_manifest_blob();
	if (manifest) {
		release_manifest_blob(manifest);
	}
}

sys_dlist_t *get_manifest_cports(void)
{
	return &g_greybus.cports;
}

/* TODO: Replace this with sys_dlist_len after rebasing to master */
size_t manifest_get_num_cports(void)
{
	struct gb_cport *gb_cport;
	size_t r = 0;

	SYS_DLIST_FOR_EACH_CONTAINER(&g_greybus.cports, gb_cport, node) {
		r++;
	}

	return r;
}

size_t manifest_get_num_cports_bundle(int bundle_id)
{
	struct gb_cport *gb_cport;
	size_t r = 0;

	SYS_DLIST_FOR_EACH_CONTAINER(&g_greybus.cports, gb_cport, node) {
		if (gb_cport->bundle == bundle_id) {
			r++;
		}
	}

	return r;
}

unsigned int manifest_get_start_cport_bundle(int bundle_id)
{
	struct gb_cport *gb_cport;
	unsigned int cport_id = UINT_MAX;

	SYS_DLIST_FOR_EACH_CONTAINER(&g_greybus.cports, gb_cport, node) {
		if (gb_cport->bundle == bundle_id && gb_cport->id < cport_id) {
			cport_id = gb_cport->id;
		}
	}

	return cport_id;
}

int get_manifest_size(void)
{
	struct greybus_manifest_header *mh = get_manifest_blob();

	return mh ? sys_le16_to_cpu(mh->size) : 0;
}

size_t manifest_get_max_bundle_id(void)
{
	return g_greybus.max_bundle_id;
}
