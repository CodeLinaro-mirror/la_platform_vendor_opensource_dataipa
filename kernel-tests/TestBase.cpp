/*
 * Copyright (c) 2017,2020 The Linux Foundation. All rights reserved.
 * Changes from Qualcomm Technologies, Inc. are provided under the following license:
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.​
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#include "TestBase.h"
#include "TestManager.h"
#include "linux/msm_ipa.h"

//////////////////////////////////////////////////////////////////////

TestBase::TestBase() :
		m_runInRegression(true),
		m_minIPAHwType(IPA_HW_v1_1),
		m_maxIPAHwType(IPA_HW_MAX)
{
	m_mem_type = DFLT_NAT_MEM_TYPE;
}

//////////////////////////////////////////////////////////////////////

void TestBase::Register(TestBase &test)
{
	TestManager::GetInstance()->Register(test);
}

//////////////////////////////////////////////////////////////////////

//Empty default implementation, a test does not have to implement Setup()
bool TestBase::Setup()
{
	return true;
}

//////////////////////////////////////////////////////////////////////

//Empty default implementation, a test does not have to implement Teardown()
bool TestBase::Teardown()
{
	return true;
}

//////////////////////////////////////////////////////////////////////

TestBase::~TestBase()
{

}

//////////////////////////////////////////////////////////////////////
