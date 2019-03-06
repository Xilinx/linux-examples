#
# This file is the sdfec-demo recipe.
#

SUMMARY = "Simple bserver application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

RM_WORK_EXCLUDE += "sdfec-demo"

DEPENDS += "libmetal libsdfecusrintf"

SRC_URI = "\
            file://xdata_source_top.c \
            file://Makefile \
            file://xdata_source_top_sinit.c \
            file://xdata_source_top.h \
            file://xdata_source_top_hw.h \
            file://xmonitor.c \
            file://xmonitor_sinit.c \
            file://xmonitor.h \
            file://xmonitor_hw.h \
            file://xstats_top.c \
            file://xstats_top_sinit.c \
            file://xstats_top.h \
            file://xstats_top_hw.h \
            file://xsd_fec_dec_docsis_long_params.c \
            file://xsd_fec_dec_docsis_long_params.h \
            file://xsd_fec_dec_docsis_medium_params.c \
            file://xsd_fec_dec_docsis_medium_params.h \
            file://xsd_fec_dec_docsis_short_params.c \
            file://xsd_fec_dec_docsis_short_params.h \
            file://xsd_fec_enc_docsis_long_params.c \
            file://xsd_fec_enc_docsis_long_params.h \
            file://xsd_fec_enc_docsis_medium_params.c \
            file://xsd_fec_enc_docsis_medium_params.h \
            file://xsd_fec_enc_docsis_short_params.c \
            file://xsd_fec_enc_docsis_short_params.h \
            file://usr_params.h \
            file://usr_params.c \
            file://demo_test_drivers.c \
            file://main.c \
            file://demo_test_drivers.h \
          "

S = "${WORKDIR}"

do_compile() {
         oe_runmake
}

do_install() {
         install -d ${D}${bindir}
         install -m 0755 sdfec-demo ${D}${bindir}
}
