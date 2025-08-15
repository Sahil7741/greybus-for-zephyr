#!/usr/bin/env python3

import os
import sys
import re
from manifesto import *


def get_string_descriptors(defines, iface):
    sd = {}
    for key in defines:
        val = defines[key]
        if key.endswith('_P_compatible_IDX_0') and val == '"zephyr,greybus-string"':
            node = key[:-len('_P_compatible_IDX_0')]
            id_ = int(defines[node + '_P_id'])
            string = defines[node + '_P_greybus_string'].strip('\"')
            sd[id_] = StringDescriptor(id_, string, None)
            if id_ == iface.vsid or id_ == iface.psid:
                sd[id_].parent = iface
    return sd


def get_interface_descriptor(defines):
    iface = None
    for key in defines:
        val = defines[key]
        if key.endswith('_P_compatible_IDX_0') and val == '"zephyr,greybus-interface"':
            node = key[:-len('_P_compatible_IDX_0')]
            vph = defines[node + '_P_vendor_string_id_IDX_0_PH']
            pph = defines[node + '_P_product_string_id_IDX_0_PH']
            vsid = int(defines[vph + '_P_id'])
            psid = int(defines[pph + '_P_id'])
            iface = InterfaceDescriptor(vsid, psid, None)
            break
    return iface

def get_mikrobus_descriptor(defines):
    mikrobus = None
    for key in defines:
        val = defines[key]
        # Accept both legacy and current compatibles for MikroBUS
        if key.endswith('_P_compatible_IDX_0') and val in ('"zephyr,greybus-mikrobus"', '"zephyr,greybus-mikrobusid"'):
            node = key[:-len('_P_compatible_IDX_0')]
            pwm = int(defines.get(node + '_P_pwmstate', '0'))
            _int = int(defines.get(node + '_P_intstate', '0'))
            rx = int(defines.get(node + '_P_rxstate', '0'))
            tx = int(defines.get(node + '_P_txstate', '0'))
            scl = int(defines.get(node + '_P_sclstate', '0'))
            sda = int(defines.get(node + '_P_sdastate', '0'))
            mosi = int(defines.get(node + '_P_mosistate', '0'))
            miso = int(defines.get(node + '_P_misostate', '0'))
            sck = int(defines.get(node + '_P_sckstate', '0'))
            cs = int(defines.get(node + '_P_csstate', '0'))
            rst = int(defines.get(node + '_P_rststate', '0'))
            an = int(defines.get(node + '_P_anstate', '0'))
            mikrobus = MikrobusDescriptor(pwm, _int, rx, tx, scl, \
                            sda, mosi, miso, sck, cs, rst, an, None)
            break
    return mikrobus

def get_device_descriptors(defines):
    device = {}
    for key in defines:
        val = defines[key]
        if key.endswith('_P_compatible_IDX_0') and val == '"zephyr,greybus-device"':
            node = key[:-len('_P_compatible_IDX_0')]
            dsh = defines[node + '_P_driver_string_id_IDX_0_PH']
            dsi = int(defines[dsh + '_P_id'])
            id_ = int(defines[node + '_P_id'])
            protocol = int(defines[node + '_P_protocol'])
            addr = int(defines[node + '_P_addr'])
            if node + '_P_irq' in defines.keys():
                irq = int(defines[node + '_P_irq'])
            else:
                irq = 0
            if node + '_P_irqtype' in defines.keys():
                irq_type = int(defines[node + '_P_irqtype'])
            else:
                irq_type = 0
            if node + '_P_max_speed_hz' in defines.keys():
                maxspeedhz = int(defines[node + '_P_max_speed_hz'])
            else:
                maxspeedhz = 0
            if node + '_P_mode' in defines.keys():
                mode = int(defines[node + '_P_mode'])
            else:
                mode = 0
            if node + '_P_prop_link' in defines.keys():
                proplink = int(defines[node + '_P_prop_link'])
            else:
                proplink = 0
            if node + '_P_gpio_link' in defines.keys():
                gpiolink = int(defines[node + '_P_gpio_link'])
            else:
                gpiolink = 0
            if node + '_P_reg_link' in defines.keys():
                reglink = int(defines[node + '_P_reg_link'])
            else:
                reglink = 0
            if node + '_P_clock_link' in defines.keys():
                clocklink = int(defines[node + '_P_clock_link'])
            else:
                clocklink = 0
            props = [dsi, protocol, addr, irq, irq_type, \
                     maxspeedhz, mode, proplink, gpiolink, reglink, clocklink]
            device[id_] = DeviceDescriptor(id_, props, None)
    return device

def get_property_descriptors(defines):
    _property = {}
    for key in defines:
        val = defines[key]
        if key.endswith('_P_compatible_IDX_0') and val == '"zephyr,greybus-property"':
            node = key[:-len('_P_compatible_IDX_0')]
            nsh = defines[node + '_P_name_string_id_IDX_0_PH']
            nsi = int(defines[nsh + '_P_id'])
            id_ = int(defines[node + '_P_id'])
            _type = int(defines[node + '_P_type'])   
            if node + '_P_value' in defines.keys():
                value = re.sub(re.compile("/\*.*?\*/",re.DOTALL ) ,"" ,defines[node + '_P_value'])
                value = value.replace('{','').replace('}','').strip()
                value = list(map(int, value.split(',')))
            else:
                value = 0
            _property[id_] = PropertyDescriptor(id_, nsi, _type, value, None)
    return _property

def get_bundle_descriptors(defines):
    bd = {}
    for key in defines:
        val = defines[key]
        if key.endswith('_P_compatible_IDX_0') and val == '"zephyr,greybus-bundle"':
            node = key[:-len('_P_compatible_IDX_0')]
            id_ = int(defines[node + '_P_id'])
            class_ = int(defines[node + '_P_bundle_class'])
            bd[id_] = BundleDescriptor(id_, class_, None)
    return bd


def get_cport_descriptors(defines):
    # add keys as necessary
    cport_keys = ['"zephyr,greybus-control"', '"zephyr,greybus-gpio-controller"',
                  '"zephyr,greybus-i2c-controller"', '"zephyr,greybus-spi-controller"']
    cd = {}
    for key in defines:
        val = defines[key]
        if key.endswith('_P_compatible_IDX_0') and val in cport_keys:
            node = key[:-len('_P_compatible_IDX_0')]
            id_ = int(defines[node + '_P_id'])
            bid = int(defines[defines[node + '_PARENT'] + '_P_id'])
            proto = int(defines[node + '_P_cport_protocol'])
            cd[id_] = CPortDescriptor(id_, bid, proto, None)
    return cd


def dt2mnfs(fn):

    # extract defines
    defines = {}
    with open(fn) as f:
        for line in f:
            line = line.strip()
            if line.startswith('#define '):
                line = line[len('#define '):]
                subs = line.split()
                key = subs[0]
                val = line[len(key + ' '):]
                val = val.strip()
                defines[key] = val

    # Feature gates: default to excluding non-standard descriptors for Linux host compatibility
    include_device = os.getenv('GBUTIL_INCLUDE_DEVICE_DESC', '0').lower() in ('1','true','yes','y')
    include_property = os.getenv('GBUTIL_INCLUDE_PROPERTY_DESC', '0').lower() in ('1','true','yes','y')
    include_mikrobus = os.getenv('GBUTIL_INCLUDE_MIKROBUS_DESC', '0').lower() in ('1','true','yes','y')

    interface_desc = get_interface_descriptor(defines)

    mikrobus_desc = None
    if include_mikrobus:
        mikrobus_desc = get_mikrobus_descriptor(defines)

    # Only parse devices/properties when enabled to avoid referencing unavailable classes
    device_descs = {}
    if include_device:
        device_descs = get_device_descriptors(defines)

    property_descs = {}
    if include_property:
        property_descs = get_property_descriptors(defines)

    # Strings, bundles, cports
    string_descs = get_string_descriptors(defines, interface_desc)
    bundle_descs = get_bundle_descriptors(defines)
    cport_descs = get_cport_descriptors(defines)

    # Determine which strings are actually referenced
    used_string_ids = set()
    if interface_desc is not None:
        used_string_ids.add(interface_desc.vsid)
        used_string_ids.add(interface_desc.psid)
    if include_device:
        for d in device_descs:
            try:
                # props[0] holds driver string id per get_device_descriptors()
                used_string_ids.add(device_descs[d].props[0])
            except Exception:
                pass
    if include_property:
        for p in property_descs:
            try:
                used_string_ids.add(property_descs[p].name_stringid)
            except Exception:
                pass

    m = Manifest()
    m.add_header(ManifestHeader(0, 1))

    # Only add used strings to avoid "excess descriptors" on host
    for sid in sorted(used_string_ids):
        if sid in string_descs:
            m.add_string_desc(string_descs[sid])

    m.add_interface_desc(interface_desc)

    if include_mikrobus and mikrobus_desc is not None:
        m.add_mikrobus_desc(mikrobus_desc)

    for d in bundle_descs:
        m.add_bundle_desc(bundle_descs[d])
    for d in cport_descs:
        m.add_cport_desc(cport_descs[d])

    if include_device:
        for d in device_descs:
            m.add_device_desc(device_descs[d])
    if include_property:
        for d in property_descs:
            m.add_property_desc(property_descs[d])
    return m


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('usage: {} <input> <output>'.format(sys.argv[0]))
        sys.exit(1)
    mnfs = dt2mnfs(sys.argv[1])
    with open(sys.argv[2], 'w') as f:
        f.write(str(mnfs))
    sys.exit(0)
