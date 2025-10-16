ACLOCAL_AMFLAGS = -Im4
EXTRA_CFLAGS	 = -DDEBUG
AM_CXXFLAGS = -Wall -Wundef -Wno-trigraphs -Werror -std=c++17

if USE_GLIB
    ipa_kernel_tests_CPPFLAGS  = $(AM_CFLAGS) -DUSE_GLIB -Dstrlcpy=g_strlcpy @GLIB_CFLAGS@
    ipa_kernel_tests_LDFLAGS = -lpthread @GLIB_LIBS@
endif

requiredlibs = -lipanat
ipa_kernel_tests_LDADD =  $(requiredlibs)

ipa_kernel_testsdir            = $(prefix)
ipa_kernel_tests_PROGRAMS      = ipa_kernel_tests
dist_ipa_kernel_tests_SCRIPTS  = run.sh
# Tests are added in alphabetical order
ipa_kernel_tests_SOURCES =\
		DataPathTestFixture.cpp \
		DataPathTests.cpp \
		ExceptionsTestFixture.cpp \
		ExceptionsTests.cpp \
		ExceptionTests.cpp \
		Feature.cpp \
		Filtering.cpp \
		FilteringEthernetBridgingTestFixture.cpp \
		FilteringEthernetBridgingTests.cpp \
		FilteringTest.cpp \
		HeaderInsertion.cpp \
		HeaderInsertionTests.cpp \
		HeaderProcessingContextTestFixture.cpp \
		HeaderProcessingContextTests.cpp \
		HeaderRemovalTestFixture.cpp \
		HeaderRemovalTests.cpp \
		InterfaceAbstraction.cpp \
		IPAFilteringTable.cpp \
		IPAInterruptsTestFixture.cpp \
		IPAInterruptsTests.cpp \
		IPv4Packet.cpp \
		IPv6CTTest.cpp \
		Logger.cpp \
		main.cpp \
		MBIMAggregationTestFixtureConf11.cpp \
		MBIMAggregationTests.cpp \
		NatTest.cpp \
		Pipe.cpp \
		PipeTestFixture.cpp \
		PipeTests.cpp \
		RNDISAggregationTestFixture.cpp \
		RNDISAggregationTests.cpp \
		RoutingDriverWrapper.cpp \
		RoutingTests.cpp \
		TestManager.cpp \
		TestBase.cpp \
		TestsUtils.cpp \
		TLPAggregationTestFixture.cpp \
		TLPAggregationTests.cpp \
		UlsoTest.cpp
