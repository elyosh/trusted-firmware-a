/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/debug.h>
#include <ddr.h>
#include <lib/utils.h>
#include <load_img.h>

#include "plat_common.h"
#include <platform_def.h>

#ifdef CONFIG_STATIC_DDR
#error not implemented
#elif defined(CONFIG_DDR_NODIMM)
#if CONFIG_DDR_NODIMM == 1
/*
 * Orion Revision 0.1: 9 x Micron MT40A1G8SA-062E:R (SDP, 8GB w/ ECC)
 *
 */
static const struct dimm_params static_dimm = {
	.mpart = "Fixed DDR Config 1",
	.n_ranks = 1,
	.die_density = 0x5, // encoded per spd byte 4, 0b101 = 8Gbit
	.rank_density = 0x200000000, // 8GB
	.capacity = 0x200000000, // 8GB
	.primary_sdram_width = 64,
	.ec_sdram_width = 8, // 8 bit ecc extension
	.rdimm = 0,
	.package_3ds = 0,
	.device_width = 8,
	.rc = 0,

	.n_row_addr = 16,
	.n_col_addr = 10,
	.edc_config = 2, // enable ecc
	.bank_addr_bits = 0, // 4 banks
	.bank_group_bits = 2, // 4 bank groups
	.burst_lengths_bitmask = 0xc, // enable 4 & 8-bit burst (DDR4 spec)

	.mirrored_dimm = 0,

	// timings based on MT40A1G8SA-062E:R DDR4-3200 22-22-22
	.mtb_ps = 125, // MTB per SPD spec
	.ftb_10th_ps = 10, // default value, unused by nxp ddr driver
	.taa_ps = 13750, // min. 13.75ns
	.tfaw_ps = 21000, // min: max(21ns or 20CK) (this 8Gbit sdram has 1KB pages)

	.tckmin_x_ps = 625, // 3200 (CK=1600)
	.tckmax_ps = 1250, // 1600 (CK=800)

	// TODO fix
	.caslat_x = 0b00000001011111111111110000000000, // CL = [10-22,24] (1 << CL)

	.trcd_ps = 13750, // 13.75ns - CL22-22-22
	.trp_ps = 13750, // 13.75ns - CL22-22-22
	.tras_ps = 32000, // 32ns

	.trfc1_ps = 350000, // 350ns
	.trfc2_ps = 260000, // 260ns
	.trfc4_ps = 160000, // 160ns
	.trrds_ps = 2500, // min: max(4CK or 2.5ns) // TODO check
	.trrdl_ps = 4900, // min: max(4CK or 4.9ns)
	.tccdl_ps = 5000, // min: max(4CK or 5ns)
	.trfc_slr_ps = 0,

	.trc_ps = 45750, // tras + trp 45.75ns
	.twr_ps = 15000, // 15ns - MT40 OK

	.refresh_rate_ps = 7800000, // 1x mode 7.8us for standard temperature range (TODO: pick correct range based on temperature?!)
	// .extended_op_srt = 0,

	// .rcw = {}, // only for registered dimm
	.dq_mapping = {
		0x01, // DQ[0:3]:   lower nibble, bit order 3120
		0x21, // DQ[4:7]:   upper nibble, bit order 4576
		0x01, // DQ[8:11]:  lower nibble, bit order 2031
		0x21, // DQ[12:15]: upper nibble, bit order 6574
		0x01, // DQ[16:19]: lower nibble, bit order 3021
		0x21, // DQ[20:23]: upper nibble, bit order 7564
		0x01, // DQ[24:27]: lower nibble, bit order 2301
		0x21, // DQ[28:31]: upper nibble, bit order 6547
		0x01, // ECC[0:3]:  lower nibble, bit order 0213
		0x21, // ECC[4:7]:  upper nibble, bit order 4576
		0x01, // DQ[32:35]: lower nibble, bit order 2130
		0x21, // DQ[36:39]: upper nibble, bit order 6574
		0x01, // DQ[40:43]: lower nibble, bit order 2031
		0x21, // DQ[44:47]: upper nibble, bit order 7465
		0x01, // DQ[48:51]: lower nibble, bit order 3021
		0x21, // DQ[52:55]: upper nibble, bit order 7564
		0x01, // DQ[56:59]: lower nibble, bit order 2130
		0x21, // DQ[60:63]: upper nibble, bit order 5746
	},
	.dq_mapping_ors = 0,
};
#endif /* CONFIG_DDR_NODIMM == 1 */

int ddr_get_ddr_params(struct dimm_params *pdimm,
			    struct ddr_conf *conf)
{
	// channel 1
	conf->dimm_in_use[0] = 1;
	memcpy(&pdimm[0], &static_dimm, sizeof(struct dimm_params));

	/* 1 module */
	return 0x1;
}
#endif /* CONFIG_STATIC_DDR */

int ddr_board_options(struct ddr_info *priv)
{
	struct memctl_opt *popts = &priv->opt;

	popts->caslat_override = 0;
	popts->caslat_override_value = 0;
	popts->auto_self_refresh_en = 1;
	popts->output_driver_impedance = 0; // 34 Ohm
	popts->twot_en = 0;
	popts->threet_en = 0;
	popts->addt_lat_override = 0;
	popts->addt_lat_override_value = 0;
	popts->phy_atx_impedance = 30;
	popts->skip2d = 0;
	popts->vref_dimm = U(0x19);		/* range 1, 83.4% */

	popts->rtt_override = 0;
	popts->rtt_park = 120U;
	popts->otf_burst_chop_en = 0;
	popts->burst_length = DDR_BL8;
	popts->trwt_override = 1;
	popts->bstopre = 0;			/* auto precharge */
	popts->addr_hash = 1;
	popts->trwt = 0x3;
	popts->twrt = 0x3;
	popts->trrt = 0x3;
	popts->twwt = 0x3;
	popts->vref_phy = U(0x5D);		/* 72% */
	popts->odt = 60U;
	popts->phy_tx_impedance = 28U;

	return 0;
}

#ifdef NXP_WARM_BOOT
long long init_ddr(uint32_t wrm_bt_flg)
#else
long long init_ddr(void)
#endif
{
	int spd_addr[] = { 0x51 };
	struct ddr_info info;
	struct sysinfo sys;
	long long dram_size;

	zeromem(&sys, sizeof(sys));
	if (get_clocks(&sys) != 0) {
		ERROR("System clocks are not set\n");
		panic();
	}
	debug("platform clock %lu\n", sys.freq_platform);
	debug("DDR PLL1 %lu\n", sys.freq_ddr_pll0);
	debug("DDR PLL2 %lu\n", sys.freq_ddr_pll1);

	zeromem(&info, sizeof(info));

	/* Set two DDRC. Unused DDRC will be removed automatically. */
	info.num_ctlrs = NUM_OF_DDRC;
	info.spd_addr = spd_addr;
	info.ddr[0] = (void *)NXP_DDR_ADDR;
	info.ddr[1] = (void *)NXP_DDR2_ADDR;
	info.phy[0] = (void *)NXP_DDR_PHY1_ADDR;
	info.phy[1] = (void *)NXP_DDR_PHY2_ADDR;
	info.clk = get_ddr_freq(&sys, 0);
	info.img_loadr = load_img;
	info.phy_gen2_fw_img_buf = PHY_GEN2_FW_IMAGE_BUFFER;
	if (info.clk == 0) {
		info.clk = get_ddr_freq(&sys, 1);
	}
	info.dimm_on_ctlr = DDRC_NUM_DIMM;

	info.warm_boot_flag = DDR_WRM_BOOT_NT_SUPPORTED;

	dram_size = dram_init(&info
#if defined(NXP_HAS_CCN504) || defined(NXP_HAS_CCN508)
				    , NXP_CCN_HN_F_0_ADDR
#endif
			);


	if (dram_size < 0) {
		ERROR("DDR init failed.\n");
	}

	return dram_size;
}
