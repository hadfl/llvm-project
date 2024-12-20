static void CONSTRUCTOR_ATTRIBUTE init_have_lse_atomics(void) {
  uint32_t hwcap = 0;
  uint_t res = getisax(&hwcap, 1);
  __aarch64_have_lse_atomics = res != 0 && (hwcap & AV_AARCH64_LSE) != 0;
}
