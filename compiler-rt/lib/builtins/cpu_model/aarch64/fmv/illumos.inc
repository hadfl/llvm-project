#include <sys/auxv.h>
#define HAVE_SYS_AUXV_H

static void __init_cpu_features_constructor(unsigned long hwcap,
                                            const __ifunc_arg_t *arg) {

  uint64_t features = 0;
  uint64_t hwcap2 = arg->_hwcap2;

  typedef struct {
    uint32_t feature_name;
    enum CPUFeatures feature;
  } feature_check_t;

  static const feature_check_t hwcapmap[] = {
    {AV_AARCH64_FP, FEAT_FP},
    {AV_AARCH64_ADVSIMD, FEAT_SIMD},
    {AV_AARCH64_SVE, FEAT_SVE},
    {AV_AARCH64_CRC32, FEAT_CRC},
    {AV_AARCH64_SB, FEAT_SB},
//    {AV_AARCH64_SSBS, RESERVED_FEAT_SSBS},
//    {AV_AARCH64_DGH, RESERVED_FEAT_DGH},
//    {AV_AARCH64_AES, RESERVED_FEAT_AES},
    {AV_AARCH64_PMULL, FEAT_PMULL},
//    {AV_AARCH64_SHA1, RESERVED_FEAT_SHA1},
    {AV_AARCH64_SHA256, FEAT_SHA2},
    {AV_AARCH64_SHA512, FEAT_SHA2},
    {AV_AARCH64_SHA3, FEAT_SHA3},
//    {AV_AARCH64_SM3,
//    {AV_AARCH64_SM4,
    {AV_AARCH64_LSE, FEAT_LSE},
    {AV_AARCH64_RDM, FEAT_RDM},
    {AV_AARCH64_FP16, FEAT_FP16},
    {AV_AARCH64_DOTPROD, FEAT_DOTPROD},
//    {AV_AARCH64_FHM,
//    {AV_AARCH64_DCPOP,
    {AV_AARCH64_F32MM, FEAT_SVE_F32MM},
    {AV_AARCH64_F64MM, FEAT_SVE_F64MM},
//    {AV_AARCH64_DCPODP,
    {AV_AARCH64_BF16, FEAT_BF16},
    {AV_AARCH64_I8MM, FEAT_I8MM},
    {AV_AARCH64_FCMA, FEAT_FCMA},
    {AV_AARCH64_JSCVT, FEAT_JSCVT},
    {AV_AARCH64_LRCPC, FEAT_RCPC},
//    {AV_AARCH64_PACA,
//    {AV_AARCH64_PACG,
    {AV_AARCH64_DIT, FEAT_DIT}
  };

  static const feature_check_t hwcap2map[] = {
    {AV_AARCH64_2_FLAGM, FEAT_FLAGM},
    {AV_AARCH64_2_ILRCPC, FEAT_RCPC},
//    {AV_AARCH64_2_LSE2,
    {AV_AARCH64_2_FLAGM2, FEAT_FLAGM2},
    {AV_AARCH64_2_FRINTTS, FEAT_FRINTTS},
    {AV_AARCH64_2_BTI, FEAT_BTI},
    {AV_AARCH64_2_RNG, FEAT_RNG},
    {AV_AARCH64_2_MTE, FEAT_MEMTAG2},
//    {AV_AARCH64_2_MTE3, RESERVED_FEAT_MEMTAG3},
//    {AV_AARCH64_2_ECV,
//    {AV_AARCH64_2_AFP,
//    {AV_AARCH64_2_RPRES, RESERVED_FEAT_RPRES},
//    {AV_AARCH64_2_LD64B,
//    {AV_AARCH64_2_ST64BV,
//    {AV_AARCH64_2_ST64BV0,
    {AV_AARCH64_2_WFXT, FEAT_WFXT},
    {AV_AARCH64_2_MOPS, FEAT_MOPS},
//    {AV_AARCH64_2_HBC,
//    {AV_AARCH64_2_CMOW,
    {AV_AARCH64_2_SVE2, FEAT_SVE2},
//    {AV_AARCH64_2_SVE2_AES, RESERVED_FEAT_SVE_AES},
    {AV_AARCH64_2_SVE2_BITPERM, FEAT_SVE_BITPERM},
    {AV_AARCH64_2_SVE2_PMULL128, FEAT_SVE_PMULL128},
    {AV_AARCH64_2_SVE2_SHA3, FEAT_SVE_SHA3},
    {AV_AARCH64_2_SVE2_SM4, FEAT_SVE_SM4},
//    {AV_AARCH64_2_TME,
    {AV_AARCH64_2_SME, FEAT_SME},
//    {AV_AARCH64_2_SME_FA64,
//    {AV_AARCH64_2_EBF16, RESERVED_FEAT_EBF16},
    {AV_AARCH64_2_SME_F64F64, FEAT_SME_F64},
    {AV_AARCH64_2_SME_I16I64, FEAT_SME_I64}
  };

  for (size_t I = 0, E = sizeof(hwcapmap) / sizeof(hwcapmap[0]); I != E; ++I)
    if (hwcap & hwcapmap[I].feature_name)
      features |= (1ULL << hwcapmap[I].feature);

  for (size_t I = 0, E = sizeof(hwcap2map) / sizeof(hwcap2map[0]); I != E; ++I)
    if (hwcap2 & hwcap2map[I].feature_name)
      features |= (1ULL << hwcap2map[I].feature);

  __atomic_store_n(&__aarch64_cpu_features.features, features, __ATOMIC_RELAXED);
}

void __init_cpu_features_resolver(unsigned long hwcap,
                                  const __ifunc_arg_t *arg) {
  if (__atomic_load_n(&__aarch64_cpu_features.features, __ATOMIC_RELAXED))
    return;
  __init_cpu_features_constructor(hwcap, arg);
}

void CONSTRUCTOR_ATTRIBUTE __init_cpu_features(void) {
  uint32_t hwcap[2] = {0};
  // CPU features already initialized.
  if (__atomic_load_n(&__aarch64_cpu_features.features, __ATOMIC_RELAXED))
    return;

  uint_t res = getisax(hwcap, 2);
  if (res == 0)
    return;

  __ifunc_arg_t arg;
  arg._size = sizeof(__ifunc_arg_t);
  arg._hwcap = hwcap[0],
  arg._hwcap2 = hwcap[1];
  __init_cpu_features_constructor(hwcap[0], &arg);
}
