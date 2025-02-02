// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2022 The Fuchsia Authors.
 */

#ifndef _ZIRCON_ZIRCON_H_
#define _ZIRCON_ZIRCON_H_

#include <compiler.h>
#include <zircon/image.h>
#include <zircon/driver-config.h>

/* called before booting to allow board specific additions to the bootdata */
int zircon_preboot(zbi_header_t *zbi);

void zircon_append_boot_item(zbi_header_t *container, uint32_t type,
			     uint32_t extra, const void *payload,
			     uint32_t length);

#endif
