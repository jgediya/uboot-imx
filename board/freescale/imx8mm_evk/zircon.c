// SPDX-License-Identifier: BSD-3-Clause

/*
 * Copyright 2022 The Fuchsia Authors.
 */

#include <zircon/zircon.h>

#define PDEV_VID_NXP 0x09
#define PDEV_PID_IMX8MMEVK 0x01

static const zbi_mem_range_t mem_config[] = {
	{
		.type = ZBI_MEM_RANGE_RAM,
		.paddr = 0x40000000,
		.length = 0x7E000000, // 2GB - 32MB for optee
	},
	{
		.type = ZBI_MEM_RANGE_RESERVED,
		.paddr = 0xBE000000,
		.length = 0x2000000, // 32MB for optee
	},
	{
		.type = ZBI_MEM_RANGE_PERIPHERAL,
		.paddr = 0,
		.length = 0x40000000,
	},
};

static const zbi_dcfg_simple_t uart_driver = {
	.mmio_phys = 0x30890000,
	.irq = 59,
};

static const zbi_dcfg_arm_gic_v3_driver_t gicv3_driver = {
	.mmio_phys = 0x38800000,
	.gicd_offset = 0x00000,
	.gicr_offset = 0x80000,
	.gicr_stride = 0x20000,
	.ipi_base = 9,
};

static const zbi_dcfg_arm_psci_driver_t psci_driver = {
	.use_hvc = false,
};

static const zbi_dcfg_arm_generic_timer_driver_t timer_driver = {
	.irq_phys = 30,
	.irq_virt = 27,
	.freq_override = 8000000,
};

static const zbi_platform_id_t platform_id = {
	.vid = PDEV_VID_NXP,
	.pid = PDEV_PID_IMX8MMEVK,
	.board_name = "imx8mmevk",
};

static void add_cpu_topology(zbi_header_t *zbi)
{
#define TOPOLOGY_CPU_COUNT 4
	zbi_topology_node_t nodes[TOPOLOGY_CPU_COUNT];

	for (uint8_t index = 0; index < TOPOLOGY_CPU_COUNT; index++) {
		nodes[index] = (zbi_topology_node_t){
			.entity_type = ZBI_TOPOLOGY_ENTITY_PROCESSOR,
				.parent_index = ZBI_TOPOLOGY_NO_PARENT,
				.entity = {
					.processor = {
						.logical_ids = {index},
						.logical_id_count = 1,
						.flags = (uint16_t)(index == 0 ? ZBI_TOPOLOGY_PROCESSOR_PRIMARY : 0),
						.architecture = ZBI_TOPOLOGY_ARCH_ARM,
						.architecture_info = {
							.arm = {
								.cpu_id = index,
								.gic_id = index,
							},
						},
					},
				},
		};
	}

	zircon_append_boot_item(
			zbi, ZBI_TYPE_CPU_TOPOLOGY, sizeof(zbi_topology_node_t), &nodes,
			sizeof(zbi_topology_node_t) * TOPOLOGY_CPU_COUNT);
}

int zircon_preboot(zbi_header_t *zbi)
{
	add_cpu_topology(zbi);

	zircon_append_boot_item(
			zbi, ZBI_TYPE_MEM_CONFIG, 0, &mem_config,
			sizeof(zbi_mem_range_t) *
			(sizeof(mem_config) / sizeof(mem_config[0])));

	zircon_append_boot_item(zbi, ZBI_TYPE_KERNEL_DRIVER,
			ZBI_KERNEL_DRIVER_IMX_UART, &uart_driver,
			sizeof(uart_driver));

	zircon_append_boot_item(zbi, ZBI_TYPE_KERNEL_DRIVER,
			ZBI_KERNEL_DRIVER_ARM_GIC_V3, &gicv3_driver,
			sizeof(gicv3_driver));

	zircon_append_boot_item(zbi, ZBI_TYPE_KERNEL_DRIVER,
			ZBI_KERNEL_DRIVER_ARM_PSCI, &psci_driver,
			sizeof(psci_driver));

	zircon_append_boot_item(zbi, ZBI_TYPE_KERNEL_DRIVER,
			ZBI_KERNEL_DRIVER_ARM_GENERIC_TIMER,
			&timer_driver, sizeof(timer_driver));

	zircon_append_boot_item(zbi, ZBI_TYPE_PLATFORM_ID, 0, &platform_id,
			sizeof(platform_id));

	return 0;
}
