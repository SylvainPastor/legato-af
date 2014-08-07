/*******************************************************************************
 * Copyright (c) 2012 Sierra Wireless and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *   http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *     Laurent Barthelemy for Sierra Wireless - initial API and implementation
 *     Romain Perier      for Sierra Wireless - initial API and implementation
 *******************************************************************************/

#include <stdlib.h>
#include "swi_system.h"
#include "swi_log.h"
#include "testutils.h"

DEFINE_TEST(test_sys_Init)
{
  rc_ReturnCode_t res;

  res = swi_sys_Init();
  ASSERT_TESTCASE_IS_OK(res);

  res = swi_sys_Init();
  ASSERT_TESTCASE_IS_OK(res);

  res = swi_sys_Init();
  ASSERT_TESTCASE_IS_OK(res);
}

DEFINE_TEST(test_sys_Destroy)
{
  rc_ReturnCode_t res;

  res = swi_sys_Destroy();
  ASSERT_TESTCASE_IS_OK(res);

  res = swi_sys_Destroy();
  ASSERT_TESTCASE_IS_OK(res);

  res = swi_sys_Destroy();
  ASSERT_TESTCASE_IS_OK(res);
}

DEFINE_TEST(test_sys_Reboot)
{
  rc_ReturnCode_t res;

  res = swi_sys_Reboot("TEST1 FOR REBOOT");
  ASSERT_TESTCASE_IS_OK(res);

  res = swi_sys_Reboot("TEST2 FOR REBOOT");
  ASSERT_TESTCASE_IS_OK(res);

  res = swi_sys_Reboot("");
  ASSERT_TESTCASE_IS_OK(res);

  res = swi_sys_Reboot(NULL);
  ASSERT_TESTCASE_IS_OK(res);
}

int main(void)
{
  INIT_TEST("DT_TEST");

  test_sys_Init();
  test_sys_Reboot();
  test_sys_Destroy();
  return 0;
}
