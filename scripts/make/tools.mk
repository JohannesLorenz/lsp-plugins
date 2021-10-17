# Setup preferred tools
TOOL_LD                 = ld
TOOL_HOST_LD            = ld
TOOL_CC                 = gcc
TOOL_CXX                = g++
TOOL_HOST_CXX           = g++
TOOL_PHP                = php
TOOL_PKG_CONFIG         = pkg-config
TOOL_HOST_PKG_CONFIG    = PKG_CONFIG_SYSROOT_DIR=/ pkg-config

# Setup preferred flags
FLAG_RELRO              = -Wl,-z,relro,-z,now
FLAG_VERSION            = -DLSP_MAIN_VERSION=\"$(LSP_VERSION)\" -DLSP_INSTALL_PREFIX=\"$(PREFIX)\"
FLAG_CXXSTANDARD        = -std=c++98
FLAGS_CTUNE             = -fno-exceptions -fno-rtti \
                          -fdata-sections -ffunction-sections -fno-asynchronous-unwind-tables \
                          -fvisibility=hidden \
                          -pipe -Wall

# Patch flags and tools
ifeq ($(BUILD_PLATFORM),Solaris)
  FLAG_RELRO              =
  TOOL_LD                 = gld
  TOOL_HOST_LD            = gld
endif

# Setup system variables
CC              		 ?= $(TOOL_CC)
CXX                      ?= $(TOOL_CXX)
HOST_CXX                 ?= $(TOOL_HOST_CXX)
PHP                      ?= $(TOOL_PHP)
LD                       ?= $(TOOL_LD)
HOST_LD                  ?= $(TOOL_HOST_LD)
PKG_CONFIG               ?= $(TOOL_PKG_CONFIG)
HOST_PKG_CONFIG          ?= $(TOOL_HOST_PKG_CONFIG)

CFLAGS                   += $(CC_ARCH) $(FLAG_CXXSTANDARD) $(FLAGS_CTUNE) \
                            $(CC_FLAGS) $(FLAG_VERSION)
CXXFLAGS                 += $(CC_ARCH) $(FLAG_CXXSTANDARD) $(FLAGS_CTUNE) \
                            $(CC_FLAGS) $(FLAG_VERSION)
HOST_CXXFLAGS            += $(FLAG_CXXSTANDARD) $(FLAGS_CTUNE) $(CC_FLAGS) $(FLAG_VERSION)
SO_FLAGS                 += $(CC_ARCH) $(FLAG_RELRO) -Wl,--gc-sections -shared -Llibrary -lc -fPIC
MERGE_FLAGS              += $(LD_ARCH) -r
EXE_TEST_FLAGS           += $(LDFLAGS) $(CC_ARCH)
EXE_FLAGS                += $(LDFLAGS) $(CC_ARCH) $(FLAG_RELRO) -Wl,--gc-sections
HOST_EXE_FLAGS           += $(HOST_LDFLAGS) $(FLAG_RELRO) -Wl,--gc-sections

ifeq ($(BUILD_PLATFORM), Linux)
  SO_FLAGS                 += -Wl,--no-undefined
endif

ifeq ($(BUILD_PLATFORM),BSD)
  EXE_FLAGS                += -L/usr/local/lib
endif

ifneq ($(LD_PATH),)
  SO_FLAGS                 += -Wl,-rpath,$(LD_PATH)
  EXE_TEST_FLAGS           += -Wl,-rpath,$(LD_PATH)
  EXE_FLAGS                += -Wl,-rpath,$(LD_PATH)
endif

ifneq ($(LV2_UI),1)
  CFLAGS                   += -DLSP_NO_LV2_UI
  CXXFLAGS                 += -DLSP_NO_LV2_UI
endif

ifneq ($(VST_UI),1)
  CFLAGS                   += -DLSP_NO_VST_UI
  CXXFLAGS                 += -DLSP_NO_VST_UI
endif

export CC
export CXX
export HOST_CXX
export PHP
export LD
export HOST_LD
export PKG_CONFIG
export HOST_PKG_CONFIG

export CFLAGS
export CXXFLAGS
export HOST_CXXFLAGS
export SO_FLAGS
export MERGE_FLAGS
export EXE_TEST_FLAGS
export EXE_FLAGS
export HOST_EXE_FLAGS
