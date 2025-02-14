/*
 * Copyright 2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <plat_common.h>

#pragma weak board_enable_povdd
#pragma weak board_disable_povdd

// TODO implement enabling / disabling POVDD
bool board_enable_povdd(void)
{
#ifdef CONFIG_POVDD_ENABLE
	return true;
#else
	return false;
#endif
}

bool board_disable_povdd(void)
{
#ifdef CONFIG_POVDD_ENABLE
	return true;
#else
	return false;
#endif
}
