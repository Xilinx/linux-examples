#include "xsd_fec_enc_docsis_medium_params.h"

u32 xsd_fec_enc_docsis_medium_sc_table_size = 2;
u32 xsd_fec_enc_docsis_medium_sc_table[2] = {
  0x0000cccc,
  0x0000000c
};
u32 xsd_fec_enc_docsis_medium_la_table_size = 5;
u32 xsd_fec_enc_docsis_medium_la_table[5] = {
  0x00000019,
  0x00000d1a,
  0x00001119,
  0x00000f19,
  0x00000f19
};
u32 xsd_fec_enc_docsis_medium_qc_table_size = 131;
u32 xsd_fec_enc_docsis_medium_qc_table[131] = {
  0x00028e00,
  0x00029e01,
  0x00027102,
  0x00027c03,
  0x00025c04,
  0x00022c05,
  0x00025d06,
  0x00024607,
  0x0002ac08,
  0x00020309,
  0x0002190a,
  0x00022c0b,
  0x00028d0c,
  0x0002a00d,
  0x0002320e,
  0x00022d0f,
  0x00027610,
  0x00025411,
  0x00024013,
  0x00024214,
  0x00026115,
  0x00020116,
  0x00027317,
  0x00020818,
  0x00026c19,
  0x0006161c,
  0x00003600,
  0x0000ac01,
  0x00009102,
  0x00001c03,
  0x00003704,
  0x00001305,
  0x00009f06,
  0x00001607,
  0x00006008,
  0x00000c09,
  0x0000550a,
  0x0000800c,
  0x0000050d,
  0x00009e0e,
  0x0000780f,
  0x00003310,
  0x0000ab11,
  0x00024112,
  0x00008d13,
  0x00002a15,
  0x00005316,
  0x00000717,
  0x00002719,
  0x0002791a,
  0x0002541b,
  0x0004651c,
  0x0006ab1d,
  0x00003f00,
  0x00000b01,
  0x00007002,
  0x00007203,
  0x00003d04,
  0x00007b05,
  0x00004806,
  0x00003707,
  0x00007208,
  0x00001409,
  0x0000350a,
  0x0000720b,
  0x00002a0c,
  0x0000210d,
  0x0000040e,
  0x0000420f,
  0x0000a310,
  0x00003211,
  0x00002e12,
  0x00001113,
  0x0000af14,
  0x00005c18,
  0x0000291a,
  0x00008a1b,
  0x0004221d,
  0x00062f1e,
  0x00001c00,
  0x0000a001,
  0x00006602,
  0x00002c03,
  0x00000804,
  0x00005405,
  0x00007e06,
  0x00000907,
  0x0000a908,
  0x0000ae09,
  0x0000930a,
  0x0000180b,
  0x0000910c,
  0x00001a0e,
  0x00004312,
  0x00005213,
  0x00000414,
  0x0000b115,
  0x00009716,
  0x00008317,
  0x00008b18,
  0x00007519,
  0x0000241a,
  0x0000121b,
  0x0004171e,
  0x0006081f,
  0x00003400,
  0x00009f01,
  0x00004b02,
  0x00004a03,
  0x00002e04,
  0x00004705,
  0x00002a06,
  0x00000b07,
  0x00006c08,
  0x00009909,
  0x0000480b,
  0x0000a30d,
  0x0000090f,
  0x00000210,
  0x0000a811,
  0x00009e12,
  0x00000114,
  0x00003115,
  0x00005916,
  0x00003f17,
  0x0000b318,
  0x00000a19,
  0x00004b1a,
  0x0000a11b,
  0x0004b11f,
  0x00061320
};
user_params xsd_fec_enc_docsis_medium_params = {
  0x00001734, // N
  0x000013b0, // K
  0x000000b4, // P
  0x00000005, // NLayers
  0x00000083, // NQC
  0x00000084, // NMQC
  0x00000042, // NM
  0x00000001, // NormType
  0x00000000, // NoPacking
  0x00000000, // SpecialQC
  0x00000000, // NoFinalParity
  0x00000000, // MaxSchedule
  &xsd_fec_enc_docsis_medium_sc_table[0],
  &xsd_fec_enc_docsis_medium_la_table[0],
  &xsd_fec_enc_docsis_medium_qc_table[0]
};
