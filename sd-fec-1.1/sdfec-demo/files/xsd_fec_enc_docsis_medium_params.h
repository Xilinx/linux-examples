
#ifndef XSD_FEC_ENC_DOCSIS_MEDIUM_PARAMS_H
#define XSD_FEC_ENC_DOCSIS_MEDIUM_PARAMS_H

#include "sdfec_usr_intf.h"

typedef struct xsdfec_user_ldpc_code_params user_params;

extern u32 xsd_fec_enc_docsis_medium_sc_table_size;
extern u32 xsd_fec_enc_docsis_medium_sc_table[2];
extern u32 xsd_fec_enc_docsis_medium_la_table_size;
extern u32 xsd_fec_enc_docsis_medium_la_table[5];
extern u32 xsd_fec_enc_docsis_medium_qc_table_size;
extern u32 xsd_fec_enc_docsis_medium_qc_table[131];
extern user_params xsd_fec_enc_docsis_medium_params;

#endif
