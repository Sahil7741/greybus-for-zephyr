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

#ifndef __GREYBUS_MANIFEST_H
#define __GREYBUS_MANIFEST_H

#include <stdint.h>

#ifndef __u8
#define __u8 uint8_t
#endif

#ifndef __le16
#define __le16 uint16_t
#endif

#ifndef __le32
#define __le32 uint32_t
#endif

#ifndef __le64
#define __le64 uint64_t
#endif

#ifndef __packed
#define __packed __attribute__((packed))
#endif

/* Greybus version with which this header complies */
#define GREYBUS_VERSION_MAJOR 0x00
#define GREYBUS_VERSION_MINOR 0x01

enum greybus_descriptor_type {
	GREYBUS_TYPE_INVALID = 0x00,
	GREYBUS_TYPE_INTERFACE = 0x01,
	GREYBUS_TYPE_STRING = 0x02,
	GREYBUS_TYPE_BUNDLE = 0x03,
	GREYBUS_TYPE_CPORT = 0x04,
	GREYBUS_TYPE_MIKROBUS = 0x05,
	GREYBUS_TYPE_PROPERTY = 0x06,
	GREYBUS_TYPE_DEVICE = 0x07,
};

enum greybus_protocol {
	GREYBUS_PROTOCOL_CONTROL = 0x00,
	GREYBUS_PROTOCOL_AP = 0x01,
	GREYBUS_PROTOCOL_GPIO = 0x02,
	GREYBUS_PROTOCOL_I2C = 0x03,
	GREYBUS_PROTOCOL_UART = 0x04,
	GREYBUS_PROTOCOL_HID = 0x05,
	GREYBUS_PROTOCOL_USB = 0x06,
	GREYBUS_PROTOCOL_SDIO = 0x07,
	GREYBUS_PROTOCOL_POWER_SUPPLY = 0x08,
	GREYBUS_PROTOCOL_PWM = 0x09,
	/* 0x0a is unused */
	GREYBUS_PROTOCOL_SPI = 0x0b,
	GREYBUS_PROTOCOL_DISPLAY = 0x0c,
	GREYBUS_PROTOCOL_CAMERA_MGMT = 0x0d,
	GREYBUS_PROTOCOL_SENSOR = 0x0e,
	GREYBUS_PROTOCOL_LIGHTS = 0x0f,
	GREYBUS_PROTOCOL_VIBRATOR = 0x10,
	GREYBUS_PROTOCOL_LOOPBACK = 0x11,
	GREYBUS_PROTOCOL_AUDIO_MGMT = 0x12,
	GREYBUS_PROTOCOL_AUDIO_DATA = 0x13,
	GREYBUS_PROTOCOL_SVC = 0x14,
	GREYBUS_PROTOCOL_FIRMWARE = 0x15,
	GREYBUS_PROTOCOL_CAMERA_DATA = 0x16,
	/* ... */
	GREYBUS_PROTOCOL_RAW = 0xfe,
	GREYBUS_PROTOCOL_VENDOR = 0xff,
};

enum greybus_class_type {
	GREYBUS_CLASS_CONTROL = 0x00,
	GREYBUS_CLASS_AP = 0x01,
	GREYBUS_CLASS_GPIO = 0x02,
	GREYBUS_CLASS_I2C = 0x03,
	GREYBUS_CLASS_UART = 0x04,
	GREYBUS_CLASS_HID = 0x05,
	GREYBUS_CLASS_USB = 0x06,
	GREYBUS_CLASS_SDIO = 0x07,
	GREYBUS_CLASS_POWER_SUPPLY = 0x08,
	GREYBUS_CLASS_PWM = 0x09,
	/* 0x0a is unused */
	GREYBUS_CLASS_SPI = 0x0b,
	GREYBUS_CLASS_DISPLAY = 0x0c,
	GREYBUS_CLASS_CAMERA = 0x0d,
	GREYBUS_CLASS_SENSOR = 0x0e,
	GREYBUS_CLASS_LIGHTS = 0x0f,
	GREYBUS_CLASS_VIBRATOR = 0x10,
	GREYBUS_CLASS_LOOPBACK = 0x11,
	GREYBUS_CLASS_AUDIO = 0x12,
	/* 0x13 is unused */
	GREYBUS_CLASS_SVC = 0x14,
	GREYBUS_CLASS_FIRMWARE = 0x15,
	/* ... */
	GREYBUS_CLASS_RAW = 0xfe,
	GREYBUS_CLASS_VENDOR = 0xff,
};

/*
 * The string in a string descriptor is not NUL-terminated.  The
 * size of the descriptor will be rounded up to a multiple of 4
 * bytes, by padding the string with 0x00 bytes if necessary.
 */
struct greybus_descriptor_string {
	__u8 length;
	__u8 id;
	__u8 string[0];
} __packed;

/*
 * An interface descriptor describes information about an interface as a whole,
 * *not* the functions within it.
 */
struct greybus_descriptor_interface {
	__u8 vendor_stringid;
	__u8 product_stringid;
	__u8 pad[2];
} __packed;

/*
 * An bundle descriptor defines an identification number and a class for
 * each bundle.
 *
 * @id: Uniquely identifies a bundle within a interface, its sole purpose is to
 * allow CPort descriptors to specify which bundle they are associated with.
 * The first bundle will have id 0, second will have 1 and so on.
 *
 * The largest CPort id associated with an bundle (defined by a
 * CPort descriptor in the manifest) is used to determine how to
 * encode the device id and module number in UniPro packets
 * that use the bundle.
 *
 * @class: It is used by kernel to know the functionality provided by the
 * bundle and will be matched against drivers functinality while probing greybus
 * driver. It should contain one of the values defined in
 * 'enum greybus_class_type'.
 *
 */
struct greybus_descriptor_bundle {
	__u8 id; /* interface-relative id (0..) */
	__u8 class;
	__u8 pad[2];
} __packed;

/*
 * A CPort descriptor indicates the id of the bundle within the
 * module it's associated with, along with the CPort id used to
 * address the CPort.  The protocol id defines the format of messages
 * exchanged using the CPort.
 */
struct greybus_descriptor_cport {
	__le16 id;
	__u8 bundle;
	__u8 protocol_id; /* enum greybus_protocol */
} __packed;

/*
 * A mikrobus descriptor is used to describe the details
 * about the bus configuration for the add-on board
 * connected to the mikrobus port.
 */
struct greybus_descriptor_mikrobus {
	__u8 pin_state[12];
} __packed;

/*
 * A property descriptor is used to pass named properties
 * to device drivers through the unified device properties
 * interface under linux/property.h
 */
struct greybus_descriptor_property {
	__u8 length;
	__u8 id;
	__u8 propname_stringid;
	__u8 type;
	__u8 value[0];
} __packed;

/*
 * A device descriptor is used to describe the
 * details required by a add-on board device
 * driver.
 */
struct greybus_descriptor_device {
	__u8 id;
	__u8 driver_stringid;
	__u8 protocol;
	__u8 reg;
	__le32 max_speed_hz;
	__u8 irq;
	__u8 irq_type;
	__u8 mode;
	__u8 prop_link;
	__u8 gpio_link;
	__u8 reg_link;
	__u8 clock_link;
	__u8 pad[1];
} __packed;

struct greybus_descriptor_header {
	__le16 size;
	__u8 type; /* enum greybus_descriptor_type */
	__u8 pad;
} __packed;

struct greybus_descriptor {
	struct greybus_descriptor_header header;
	union {
		struct greybus_descriptor_string string;
		struct greybus_descriptor_interface interface;
		struct greybus_descriptor_bundle bundle;
		struct greybus_descriptor_cport cport;
		struct greybus_descriptor_mikrobus mikrobus;
		struct greybus_descriptor_property property;
		struct greybus_descriptor_device device;
	};
} __packed;

struct greybus_manifest_header {
	__le16 size;
	__u8 version_major;
	__u8 version_minor;
} __packed;

struct greybus_manifest {
	struct greybus_manifest_header header;
	struct greybus_descriptor descriptors[0];
} __packed;

#endif /* __GREYBUS_MANIFEST_H */
