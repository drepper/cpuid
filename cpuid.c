/* Compile with

   c99 -Os -Wall -o cpuid cpuid.c

*/
#include <cpuid.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>


typedef struct {
  unsigned bitmask;
  const char *name;
} bit_name_map;


#define NARRAY(arr) (sizeof(arr) / sizeof(arr[0]))


static const char clevel[][2] = {
  [1] = "d",
  [2] = "i",
  [3] = "",
  [4] = "?",
  [5] = "?",
  [6] = "?",
  [7] = "?",
  [8] = "?",
  [9] = "?",
  [10] = "?",
  [11] = "?",
  [12] = "?",
  [13] = "?",
  [14] = "?",
  [15] = "?",
  [16] = "?",
  [17] = "?",
  [18] = "?",
  [19] = "?",
  [20] = "?",
  [21] = "?",
  [22] = "?",
  [23] = "?",
  [24] = "?",
  [25] = "?",
  [26] = "?",
  [27] = "?",
  [28] = "?",
  [29] = "?",
  [30] = "?",
  [31] = "?",
};


static void
print_mask(const char *intro, unsigned mask, const bit_name_map *table,
	   size_t ntable)
{
  printf("%s = %08x (", intro, mask);
  size_t n = 0;
  bool first = true;
  while (mask != 0) {
    if (mask & table[n].bitmask) {
      printf("%s%s", first ? "" : " ", table[n].name);
      mask ^= table[n].bitmask;
      first = false;
    }
    if (++n == ntable && mask != 0) {
      printf("%sremainder=%x", first ? "" : " ", mask);
      break;
    }
  }
  puts(")");
};


static void
handler_0x01(void)
{
  union {
    unsigned eax;
    struct {
      unsigned stepping_ID : 4;
      unsigned model : 4;
      unsigned family_ID : 4;
      unsigned processor_type : 2;
      unsigned : 2;
      unsigned extended_model_ID : 4;
      unsigned extended_family_ID : 8;
      unsigned : 4;
    };
  } u;
  union {
    unsigned ebx;
    struct {
      unsigned brand_index : 8;
      unsigned clflush_line_size : 8;
      unsigned logical_processors : 8;
      unsigned initial_APID_ID : 8;
    };
  } v;
  unsigned ecx, edx;
  __cpuid(1, u.eax, v.ebx, ecx, edx);

  printf("stepping ID           = %u\n"
	 "...model              = %u\n"
	 "...family ID          = %u\n"
	 "processor type        = %u\n"
	 "...extended model ID  = %u\n"
	 "...extended family ID = %u\n"
	 "real model            = %u\n"
	 "real family ID        = %u\n",
	 u.stepping_ID, u.model, u.family_ID, u.processor_type,
	 u.extended_model_ID, u.extended_family_ID,
	 u.model | (u.extended_model_ID << 4),
	 u.family_ID | (u.extended_family_ID << 4));

  printf("brand index                       = %u\n"
	 "CLFLUSH line size                 = %u (%u bytes)\n"
	 "maximum number logical processors = %u\n"
	 "initial APID ID                   = %u\n",
	 v.brand_index, v.clflush_line_size,
	 v.clflush_line_size * 8,
	 v.logical_processors, v.initial_APID_ID);

  static const bit_name_map features_edx[] = {
#define F(n, s) \
    { 1u << n, #s }
    F(0, FPU),
    F(1, VME),
    F(2, DE),
    F(3, PSE),
    F(4, TSC),
    F(5, MSR),
    F(6, PAE),
    F(7, MCE),
    F(8, CX8),
    F(9, APIC),
    F(11, SEP),
    F(12, MTRR),
    F(13, PGE),
    F(14, MCA),
    F(15, CMOV),
    F(16, PAT),
    F(17, PSE-36),
    F(18, PSN),
    F(19, CLFSH),
    F(21, DS),
    F(22, ACPI),
    F(23, MMX),
    F(24, FXSR),
    F(25, SSE),
    F(26, SSE2),
    F(27, SS),
    F(28, HTT),
    F(29, TM),
    F(31, PBE)
  };
  print_mask("edx", edx, features_edx, NARRAY(features_edx));

  static const bit_name_map features_ecx[] = {
    F(0, SSE3),
    F(1, PCMULQDQ),
    F(2, DTES64),
    F(3, MONITOR),
    F(4, DS-CPL),
    F(5, VMX),
    F(6, SMX),
    F(7, EST),
    F(8, TM2),
    F(9, SSSE3),
    F(10, CNXT-ID),
    F(11, SDBG),
    F(12, FMA),
    F(13, CMPXCHG16B),
    F(14, xTPR),
    F(15, PDCM),
    F(17, PCID),
    F(18, DCA),
    F(19, SSE4.1),
    F(20, SSE4.2),
    F(21, x2APID),
    F(22, MOVBE),
    F(23, POPCNT),
    F(24, TSC-deadline),
    F(25, AES),
    F(26, XSAVE),
    F(27, OSXSAVE),
    F(28, AVX),
    F(29, F16C),
    F(30, RDRAND)
  };
  print_mask("ecx", ecx, features_ecx, NARRAY(features_ecx));
}
#undef F


static void
handler_0x02(void)
{
  unsigned r[4];
  unsigned cnt = 0;
  unsigned max = 1;

  static const char *cachedescr[] = {
#define F(n, str) [n] = #str
    F(0x01, ITLB;4kPage;4way;32),
    F(0x02, ITLB;4MPage;4way;2),
    F(0x03, DTLB;4kPage;4way;64),
    F(0x04, DTLB;4MPage;4way;8),
    F(0x05, DTLB1;4MPage;4way;32),
    F(0x06, L1i;8k;4way;32byte),
    F(0x08, L1i;16k;4way;32byte),
    F(0x0a, L1d;8k;2way;32byte),
    F(0x0b, ITLB;4MPage;4way;4),
    F(0x0c, L1d;16k;4way;32byte),
    F(0x0d, L1d;16k;4way;64byte),
    F(0x0e, L1d;24k;6way;64byte),
    F(0x21, L2;256k;8way;64byte),
    F(0x22, L3;512k;4way;64byte;2lines),
    F(0x23, L3;1M;8way;64byte;2lines),
    F(0x25, L3;2M;8way;64byte;2lines),
    F(0x29, L3;4M;8way;64byte;2lines),
    F(0x2c, L1d;32k;8way;64byte),
    F(0x30, L1i;32k;8way;64byte),
    F(0x40, NoL2orL3),
    F(0x41, L2;128k;4way;32byte),
    F(0x42, L2;256k;4way;32byte),
    F(0x43, L2;512k;4way;32byte),
    F(0x44, L2;1M;4way;32byte),
    F(0x45, L2;2M;4way;32byte),
    F(0x46, L3;4M;4way;64byte),
    F(0x47, L3;8M;8way;64byte),
    F(0x48, L2;3M;12way;64byte),
    F(0x49, L3;4M;16way;64byte),
    F(0x4a, L3;6M;12way;64byte),
    F(0x4b, L3;8M;16way;64byte),
    F(0x4c, L3;12M;12way;64byte),
    F(0x4d, L3;16M;16way;64byte),
    F(0x4e, L2;6M;24way;64byte),
    F(0x4f, ITLB;4kPage;32),
    F(0x50, ITLB;4K+2M+4MPage;64),
    F(0x51, ITLB;4K+2M+4MPage;128),
    F(0x52, ITLB;4K+2M+4MPage;256),
    F(0x55, ITLB;2MPage+4MPage;full;7),
    F(0x56, DTLB0;4MPage;4way;16),
    F(0x57, DTLB0;4kPage;4way;16),
    F(0x59, DTLB0;4kPage;full;16),
    F(0x5a, DTLB0;2MPage+4MPage;4way;32),
    F(0x5b, DTLB;4k+4MPage;64),
    F(0x5c, DTLB;4k+4MPage;128),
    F(0x5d, DTLB;4k+4MPage;256),
    F(0x60, L1d;16k;8way;64byte),
    F(0x63, DTLB;1GPage;4way;4),
    F(0x66, L1d;8k;4way;64byte),
    F(0x67, L1d;16k;4way;64byte),
    F(0x68, L1d;32k;4way;64byte),
    F(0x70, Trace;12kµop;8way),
    F(0x71, Trace;16kµop;8way),
    F(0x72, Trace;32kµop;8way),
    F(0x76, ITLB;2MPage+4MPage;full;8),
    F(0x78, L2;1M;4way;64byte),
    F(0x79, L2;128k;8way;64byte;2lines),
    F(0x7a, L2;256k;8way;64byte;2lines),
    F(0x7b, L2;512k;8way;64byte;2lines),
    F(0x7c, L2;1M;8way;64byte;2lines),
    F(0x7d, L2;2M;8way;64byte),
    F(0x7f, L2;512k;2way;64byte),
    F(0x80, L2;512k;8way;64byte),
    F(0x81, L2;128k;8way;32byte),
    F(0x82, L2;256k;8way;32byte),
    F(0x83, L2;512k;8way;32byte),
    F(0x84, L2;1M;8way;32byte),
    F(0x85, L2;2M;8way;32byte),
    F(0x86, L2;512k;4way;64byte),
    F(0x87, L2;1M;8way;64byte),
    F(0x88, L3;2M;4way;64byte),
    F(0x89, L3;4M;4way;64byte),
    F(0x8a, L3;8M;4way;64byte),
    F(0x8d, L3;3M;12way;128byte),
    F(0x90, ITLB;4kPage...256MPage;full;64),
    F(0x96, DTLB1;4kPage...256MPage;full;32),
    F(0x9b, DTLB2;4kPage...256MPage;full;96),
    F(0xa0, DTLB;4kPage;full;32),
    F(0xb0, ITLB;4kPage;4way;128),
    F(0xb1, ITLB;2MPage;4way;8;4MPage;4way;4),
    F(0xb2, ITLB;4kPage;4way;64),
    F(0xb3, DTLB;4kPage;4way;128),
    F(0xb4, DTLB1;4kPage;4way;256),
    F(0xb5, ITLB;4kPage;8way;64),
    F(0xb6, ITLB;4kPage;8way;128),
    F(0xba, DTLB1;4kPage;4way;64),
    F(0xc0, DTLB;4kPage+4MPage;4way;8),
    F(0xc1, TLB2;4kPage+2MPage;8way;1024),
    F(0xc2, DTLB;2MPage+4MPage;4way;16),
    F(0xca, SharedL2TLB;4kPage;4way;512),
    F(0xd0, L3;512k;4way;64byte),
    F(0xd1, L3;1M;4way;64byte),
    F(0xd2, L3;2M;4way;64byte),
    F(0xd6, L3;1M;8way;64byte),
    F(0xd7, L3;2M;8way;64byte),
    F(0xd8, L3;4M;8way;64byte),
    F(0xdc, L3;1.5M;12way;64byte),
    F(0xdd, L3;3M;12way;64byte),
    F(0xde, L3;6M;12way;64byte),
    F(0xe2, L3;2M;16way;64byte),
    F(0xe3, L3;4M;16way;64byte),
    F(0xe4, L3;8M;16way;64byte),
    F(0xea, L3;12M;24way;64byte),
    F(0xeb, L3;18M;24way;64byte),
    F(0xec, L3;24M;24way;64byte),
    F(0xf0, 64prefetch),
    F(0xf1, 128prefetch),
    F(0xff, Leaf4)
  };
#undef F

  while (cnt++ < max) {
    __cpuid (2, r[0], r[1], r[2], r[3]);
    if (cnt == 1)
      max = r[0] & 0xff;
    for (size_t i = 0; i < 4; ++i)
      if (r[i] == 0)
        printf("e%cx NULL descriptors\n", (int)('a' + i));
      else if ((r[i] & 0x80000000) != 0)
        printf("e%cx invalid descriptors\n", (int)('a' + i));
      else {
        printf("e%cx = %08x (", (int)('a' + i), r[i]);
        bool any = false;
        unsigned m = 0xff000000;
        do
	  if (cnt == 1 && i == 0 && m == 0xff)
	    printf("%scount=%u", any ? " " : "", max);
	  else if (r[i] & m) {
	    unsigned idx = (r[i] & m) >> __builtin_ctz(m);

	    if (idx < NARRAY(cachedescr) && cachedescr[idx])
	      printf("%s%s", any ? " " : "", cachedescr[idx]);
	    else
	      printf("%s???", any ? " " : "");
	    any = true;
	  }
        while ((m >>= 8) != 0);
	puts(")");
      }
  }
}


static void
handler_0x03(void)
{
  unsigned eax, ebx, ecx, edx;
  __cpuid(3, eax, ebx, ecx, edx);
  printf("psn = ????????%08x%08x\n", edx, ecx);
}


static void
handler_0x04(void)
{
  unsigned level = 0;
  while (1) {
    unsigned eax, ebx, ecx, edx;
    __cpuid_count(4, level, eax, ebx, ecx, edx);
    if ((eax & 0x1f) == 0)
      break;
    if (level > 0)
      printf("subleaf %u:\n", level);
    printf("eax = %08x (L%u%s%s%s #log=%u #core=%u)\n",
 	   eax,
	   (eax >> 5) & 7,
	   clevel[eax & 31],
	   eax & 256 ? " self" : "",
	   eax & 512 ? " full" : "",
	   1 + ((eax >> 14) & 0xfff),
	   1 + (eax >> 26));
    printf("ebx = %08x (line=%u part=%u ways=%u)\n",
	   ebx,
	   1 + (ebx & 0xfff),
	   1 + ((ebx >> 12) & 0x3ff),
	   1 + (ebx >> 22));
    printf("ecx = %08x (#set=%u)\n",
	   ecx, 1 + ecx);
    printf("edx = %08x (%s %s %s)\n",
	   edx,
	   edx & 1 ? "+invd" : "-invd",
	   edx & 2 ? "incl" : "excl",
	   edx & 4 ? "direct" : "complex");
    
    ++level;
  }
}


static void
handler_0x05(void)
{
  unsigned eax, ebx, ecx, edx;
  __cpuid(5, eax, ebx, ecx, edx);
  printf("eax = %08x (smallest mwait line size=%u)\n",
	 eax, eax & 0xffff);
  printf("ebx = %08x (largest mwait line size=%u)\n",
	 ebx, ebx & 0xffff);
  printf("ecx = %08x (%s%s%s)\n",
	 ecx,
	 ecx & 1 ? "enum" : "",
	 (ecx & 3) == 3 ? " " : "",
	 ecx & 2 ? "intr" : "");
  printf("edx = %08x (#C0=%u #C1=%u #C2=%u #C3=%u #C4=%u)\n",
	 edx,
	 edx & 0xf,
	 (edx >> 4) & 0xf,
	 (edx >> 8) & 0xf,
	 (edx >> 12) & 0xf,
	 (edx >> 16) & 0xf);
}


static void
handler_0x06(void)
{
  unsigned eax, ebx, ecx, edx;
  __cpuid(5, eax, ebx, ecx, edx);
  printf("eax = %08x (%s%s%s%s%s%s%s%s%s%s%s)\n",
	 eax,
	 eax & 1 ? "tempsens" : "",
	 (eax & 1) && (eax & 2) ? " " : "",
	 eax & 2 ? "boost" : "",
	 (eax & 3) && (eax & 4) ? " " : "",
	 eax & 4 ? "ARAT" : "",
	 (eax & 7) && (eax & 16) ? " " : "",
	 eax & 16 ? "PLN" : "",
	 (eax & 23) && (eax & 32) ? " " : "",
	 eax & 32 ? "ECMD" : "",
	 (eax & 55) && (eax & 64) ? " " : "",
	 eax & 64 ? "PTM" : "");
  printf("ebx = %08x (#thresholds=%u)\n",
	 ebx,
	 ebx & 0xf);
  printf("ecx = %08x (%s%s%s)\n",
	 ecx,
	 ecx & 1 ? "feedback" : "",
	 (ecx & 1) && (ecx & 8) ? " " : "",
	 ecx & 8 ? "bias" : "");
}


static unsigned ebx_7;

static void
handler_0x07(void)
{
  unsigned n = 0;
  unsigned max = 1;
  do {
    unsigned eax, ebx, ecx, edx;
    __cpuid_count(7, n, eax, ebx, ecx, edx);
    switch (n) {
    case 0:
      printf("eax = %08x (max subleaf=%u)\n", eax, eax);
      max = eax;
      static const bit_name_map features_ebx[] = {
#define F(n, s) \
        { 1u << n, #s }
	F(0, FSGSBASE),
	F(1, TSC_ADJUST),
        F(2, SGX),
	F(3, BMI1),
	F(4, HLE),
	F(5, AVX2),
        F(6, FDP_EXCPTN_ONLY),
	F(7, SMEP),
	F(8, BMI2),
	F(9, ERMS),
	F(10, INVPCID),
	F(11, RTM),
	F(12, RDTM),
	F(13, DFDC),
	F(14, MPX),
        F(15, RDTA),
	F(16, AVX512F),
	F(17, AVX512DQ),
	F(18, RDSEED),
	F(19, ADX),
	F(20, SMAP),
        F(21, AVX512_IFMA),
        F(23, CLFLUSHOPT),
        F(24, CLWB),
	F(25, Trace),
	F(26, AVX512PF),
	F(27, AVX512ER),
	F(28, AVX512CD),
	F(29, SHA),
        F(30, AVX512BW),
        F(31, AVX512VL),
      };
      ebx_7 = ebx;
      print_mask("ebx", ebx, features_ebx,
		 NARRAY(features_ebx));
      static const bit_name_map features_ecx[] = {
        F(0, PREFETCHWT1),
        F(1, AVX512_VBMI),
        F(2, UMIP),
        F(3, PKU),
        F(4, OSPKE),
        F(14, AVX512_VPOPCNTDQ),
        F(22, RDPID),
        F(30, SGX_LC),
      };
      print_mask("ecx", ecx, features_ecx,
		 NARRAY(features_ecx));
      printf("  NAWAU=%u\n", (ecx & 0b1111100000000000000000) >> 17);
      static const bit_name_map features_edx[] = {
        F(2, AVX512_4VNNIW),
        F(3, AVX512_4FMAPS),
        F(26, IBRS),
        F(27, STIBP),
        F(28, L1D_FLUSH),
        F(29, IA32_ARCH_CAPABILITIES),
        F(31, SSBD)
      };
      print_mask("edx", edx, features_edx,
		 NARRAY(features_edx));
#undef F
      break;
    default:
      printf("unknown subleaf %u\n", n);
      break;
    }
  } while (n++ < max);
}


static void
handler_0x09(void)
{
  unsigned eax, ebx, ecx, edx;
  __cpuid(9, eax, ebx, ecx, edx);
  printf("eax = %08x (DCA_CAP=%u)\n",
	 eax, eax);
}


static void
handler_0x0a(void)
{
  unsigned eax, ebx, ecx, edx;
  __cpuid(0xa, eax, ebx, ecx, edx);
  printf("eax = %08x (version=%u #cntr=%u width=%u ebxlen=%u)\n",
	 eax,
	 eax & 0xff,
	 (eax >> 8) & 0xff,
	 (eax >> 16) & 0xff,
	 (eax >> 24) & 0xff);
  static const bit_name_map features_ebx[] = {
#define F(n, s) \
        { 1u << n, #s }
    F(0, -CYC),
    F(1, -RETIRED),
    F(2, -REFCYC),
    F(3, -LLCREF),
    F(4, -LLCMIS),
    F(5, -BRRET),
    F(6, -BRMIS),
  };
#undef F
  print_mask("ebx", ebx, features_ebx,
	     MIN((eax >> 24) & 0xff, NARRAY(features_ebx)));
  if ((eax & 0xff) > 1)
    printf("edx = %08x (#fixed=%u widthfixed=%u)\n",
	   edx,
	   edx & 0x1f,
	   (edx >> 5) & 0xff);
}


static void
handler_0x0b(void)
{
  unsigned eax, ebx, ecx, edx;
  unsigned n = 0;
  while (1) {
    __cpuid_count(0xb, n, eax, ebx, ecx, edx);
    if (((ecx >> 8) & 0xff) == 0)
      break;
    if (n > 0)
      printf("subleaf %u:\n", n);

    printf("eax = %08x (shift=%u)\n",
	   eax, eax & 0xf);
    printf("ebx = %08x (#proc=%u)\n",
	   ebx, ebx & 0xffff);
    printf("ecx = %08x (level=%u type=%u)\n",
	   ecx,
	   ecx & 0xff,
	   (ecx >> 8) & 0xff);
    printf("edx = %08x (x2APIC=%u)\n",
	   edx, edx);

    ++n;
  }
}


static void
handler_0x0d(void)
{
  unsigned eax, ebx, ecx, edx;
  __cpuid_count(0xd, 0, eax, ebx, ecx, edx);
  static const bit_name_map features_eax[] = {
#define F(n, s) \
    { 1u << n, #s }
    F(0, x87),
    F(1, SSE),
    F(2, YMM_HI128),
    F(3, BNDCSR),
    F(4, BNDREGS),
    F(5, OPMASK),
    F(6, ZMM_HI256),
    F(7, HI16_ZMM),
  };
#undef F
  print_mask("eax", eax, features_eax, NARRAY(features_eax));
  printf("ebx = %08x (max size enabled=%u)\n",
	 ebx, ebx);
  printf("ecx = %08x (max size all=%u)\n",
	 ecx, ecx);
  static const bit_name_map features_edx[] = {
#define F(n, s) \
    { 1u << n, #s }
    F(30, LWP),
  };
#undef F
  print_mask("edx", edx, features_edx, NARRAY(features_edx));

  unsigned mask = eax;

  printf("subleaf %u:\n", 1);
  __cpuid_count(0xd, 1, eax, ebx, ecx, edx);
  printf("eax = %08x (%s)\n",
	 eax, 
	 eax & 1 ? "XSAVEOPT" : "");

  unsigned i = 2;
  while (i < 32-__builtin_clz(mask)) {
    printf("subleaf %u: %s\n", i, features_eax[i].name);
    __cpuid_count(0xd, i++, eax, ebx, ecx, edx);
    printf("eax = %08x (size=%u)\n", eax, eax);
    printf("ebx = %08x (offset=%u)\n", ebx, ebx);
  }

  __cpuid_count(0xd, 62, eax, ebx, ecx, edx);
  if (eax != 0 || ebx != 0 || ecx != 0 || edx != 0) {
    printf("subleaf %u:\n", 62);
    printf("eax = %08x (lwpsize=%u)\n", eax, eax);
    printf("ebx = %08x (lwpoffset=%u)\n", ebx, ebx);
  }
}


static void
handler_0x0f(void)
{
  unsigned eax, ebx, ecx, edx;
  __cpuid_count(0xf, 0, eax, ebx, ecx, edx);
  printf("ebx = %08x (range RMID=%u)\n", ebx, ebx);
  static const bit_name_map features_edx_0[] = {
#define F(n, s) \
        { 1u << n, #s }
    F(1, L3QoS),
  };
#undef F
  print_mask("edx", edx, features_edx_0, NARRAY(features_edx_0));

  printf("Leaf 15: L3 Cache QoS Capability Enumeration\n");
  __cpuid_count(0xf, 1, eax, ebx, ecx, edx);
  printf("ebx = %08x (convfactor=%u)\n", ebx, ebx);
  printf("ecx = %08x (range RMID=%u)\n", ecx, ecx);
  static const bit_name_map features_edx_1[] = {
#define F(n, s) \
        { 1u << n, #s }
    F(1, L3occup),
  };
#undef F
  print_mask("edx", edx, features_edx_1, NARRAY(features_edx_1));
}


static void
handler_0x10(void)
{
  unsigned eax, ebx, ecx, edx;
  unsigned avail;
  __cpuid_count(0x10, 0, eax, avail, ecx, edx);
  printf("eax = %08x (", eax);
  printf("ebx = %08x (", avail);
  bool first = true;
  if (avail & 2) {
    printf("L3 Cache Allocation");
    first = false;
  }
  if (avail & 4) {
    if (! first)
      printf(", ");
    printf("L2 Cache Allocation");
    first = false;
  }
  if (avail & 8) {
    if (! first)
      printf(", ");
    printf("Memory Bandwidth Allocation");
    first = false;
  }
  printf(")\n");
  printf("ecx = %08x (", ecx);
  printf("edx = %08x (", edx);
  for (unsigned i = 1; i <= 3; ++i)
    if (avail & (1 << i)) {
      printf("subleaf %u:\n", i);
      __cpuid_count(0x10, i, eax, avail, ecx, edx);
      if (i == 3)
        printf("eax = %08x (maximum MBA throttling=%u)\n", eax, (eax & 0b11111111111) + 1);
      else
        printf("eax = %08x (len capability mask=%u)\n", eax, (eax & 0b11111) + 1);
      printf("ebx = %08x\n", ebx);
      if (i == 3)
        printf("ecx = %08x (%s)\n", ecx, (ecx & 4) ? "delay value linear" : "");
      else
        printf("ecx = %08x (%s)\n", ecx, (ecx & 4) ? "code&data prioritization" : "");
      printf("edx = %08x (highest COS=%u)\n", edx, edx & 0xffff);
    }
}


static void
handler_0x12(void)
{
  if ((ebx_7 & 4) == 0)
    return;
  unsigned eax, ebx, ecx, edx;
  __cpuid_count(0x12, 0, eax, ebx, ecx, edx);
  static const bit_name_map features_eax_0[] = {
#define F(n, s) \
    { 1u << n, #s }
    F(0, SGX1),
    F(1, SGX2),
    F(5, ENCLV),
    F(6, ENCLS),
  };
  print_mask("eax", eax, features_eax_0, NARRAY(features_eax_0));
  printf("ebx = %08x (MISCSELECT)\n", ebx);
  printf("ecx = %08x\n", ecx);
  printf("edx = %08x (MaxEnclSizeNot64=%llu MaxEnclSize64=%llu\n", edx, 1llu<<(edx & 0xff), 1llu<<((edx >> 8) & 0xff));

  printf("subleaf 1:\n");
  __cpuid_count(0x12, 1, eax, ebx, ecx, edx);
  printf("eax = %08x (SEC.ATTRIBUTES[31:0])\n", eax);
  printf("ebx = %08x (SEC.ATTRIBUTES[63:32])\n", eax);
  printf("ecx = %08x (SEC.ATTRIBUTES[95:64])\n", eax);
  printf("edx = %08x (SEC.ATTRIBUTES[127:96])\n", eax);

  printf("******************* decoding of subleaf 2 and later missing\n");
}


static void
handler_0x14(void)
{
  unsigned eax, ebx, ecx, edx;
  __cpuid_count(0x14, 0, eax, ebx, ecx, edx);
  printf("eax = %08x (max leaf=%u)\n", eax, eax);
  static const bit_name_map features_ebx[] = {
#define F(n, s) \
        { 1u << n, #s }
    F(0, RTIT_CTLCR3Filter),
  };
#undef F
  print_mask("ebx", ebx, features_ebx, NARRAY(features_ebx));
  static const bit_name_map features_ecx[] = {
#define F(n, s) \
        { 1u << n, #s }
    F(0, RTIT_CTLToPA),
    F(1, ToPAmax),
    F(31, LIPCSbase)
  };
#undef F
  print_mask("ecx", ecx, features_ecx, NARRAY(features_ecx));
}


static void
handler_0x15(void)
{
  unsigned eax, ebx, ecx, edx;
  __cpuid(0x15, eax, ebx, ecx, edx);
  printf("eax = %08x (denominator TSC/core crystal clock=%u)\n", eax, eax);
  printf("ebx = %08x (enumerator TSC/core crystal clock=%u)\n", ebx, ebx);
  printf("ecx = %08x (core crystal clock=%u Hz)\n", ecx, ecx);
  printf("edx = %08x\n", edx);
}


static void
handler_0x16(void)
{
  unsigned eax, ebx, ecx, edx;
  __cpuid(0x16, eax, ebx, ecx, edx);
  printf("eax = %08x (Base Frequency=%u MHz)\n", eax, eax & 0xffff);
  printf("ebx = %08x (Max Frequency=%u MHz)\n", ebx, ebx & 0xffff);
  printf("ecx = %08x (Bus (Reference) Frequency=%u MHz)\n", ecx, ecx & 0xffff);
  printf("edx = %08x\n", edx);
}


static struct {
  void (*handler)(void);
  const char *name;
} leaves[] =
{
  { NULL, NULL },
  { handler_0x01, "Basic CPU Information" },
  { handler_0x02, "Basic CPU Information" },
  { handler_0x03, "Basic CPU Information" },
  { handler_0x04, "Deterministic Cache Parameter" },
  { handler_0x05, "MONITOR/MWAIT" },
  { handler_0x06, "Thermal and Power Management" },
  { handler_0x07, "Structured Extended Feature Flags Enumeration" },
  { NULL, NULL },
  { handler_0x09, "Direct Cache Access Information" },
  { handler_0x0a, "Architectural Performance Monitoring" },
  { handler_0x0b, "Extended Topology Enumeration" },
  { NULL, NULL },
  { handler_0x0d, "Processor Extended State Enumeration Main" },
  { NULL, NULL },
  { handler_0x0f, "Quality of Service Resource Type Enumeration" },
  { handler_0x10, "Intel Resource Director Technology (Intel RDT) Allocation Enumeration" },
  { NULL, NULL },
  { handler_0x12, "Intel SGX Enumeration" },
  { NULL, NULL },
  { handler_0x14, "Intel Processor Trace Enumeration Main" },
  { handler_0x15, "Time Stamp Counter and Nominal Core Crystal Clock Information" },
  { handler_0x16, "Processor Frequency Information" },
};


static const char *
amdassoc(unsigned v, char *buf)
{
  switch (v) {
  case 0:
    abort();
  case 1:
    return "direct";
  case 0xff:
    return "full";
  default:
    sprintf(buf, "%uway", v);
    return buf;
  }
}


static void
extended_function(void)
{
  unsigned eax, ebx, ecx, edx;
  __cpuid(0x80000000, eax, ebx, ecx, edx);
  printf("\nLeaf 0x80000000: Extended Function CPUID Information\n"
	 "eax = %08x (maxleaf=%x)\n",
	 eax, eax);
  unsigned max = eax;

  if (max < 0x80000001)
    return;

  __cpuid(0x80000001, eax, ebx, ecx, edx);
  printf("\nLeaf 0x80000001: Extended Function CPUID Information\n");
  static const bit_name_map features_ecx_1[] = {
#define F(n, s) \
        { 1u << n, #s }
    F(0, LAHFSAHF),
    F(1, CMPLEGACY),
    F(2, SVM),
    F(3, EXTAPIC),
    F(4, ALTMOVCR8),
    F(5, LZCNT),
    F(6, SSE4A),
    F(7, MISALIGNSSE),
    F(8, PREFETCHW),
    F(9, OSVW),
    F(10, IBS),
    F(11, XOP),
    F(12, SKINIT),
    F(13, WDT),
    F(15, LWP),
    F(16, FMA4),
    F(19, NODEID),
    F(21, TBM),
    F(22, TOPOLOGY),
  };
#undef F
  print_mask("ecx", ecx, features_ecx_1, NARRAY(features_ecx_1));
  static const bit_name_map features_edx_1[] = {
#define F(n, s) \
        { 1u << n, #s }
    F(0, X87),
    F(1, VME),
    F(2, DE),
    F(3, PSE),
    F(4, TSC),
    F(5, MSR),
    F(6, PAE),
    F(7, MCE),
    F(8, CMPXCHG8B),
    F(9, APIC),
    F(11, SYSCALL),
    F(12, MTRR),
    F(13, PGE),
    F(14, MCA),
    F(15, CMOV),
    F(16, PAT),
    F(17, PSE36),
    F(20, NX),
    F(22, MMXEXT),
    F(23, MMX),
    F(24, FXSR),
    F(25, FFXSR),
    F(26, 1GBPAGE),
    F(27, RDTSCP),
    F(29, EM64),
    F(30, 3DNOWEXT),
    F(31, 3DNOW)
  };
#undef F
  print_mask("edx", edx, features_edx_1, NARRAY(features_edx_1));

  if (max < 0x80000004)
    return;

  printf("\nLeaf 0x80000002-0x80000004: Extended Function CPUID Information\n");
  union {
    unsigned r[12];
    char buf[96];
  } u;
  __cpuid(0x80000002, u.r[0], u.r[1], u.r[2], u.r[3]);
  __cpuid(0x80000003, u.r[4], u.r[5], u.r[6], u.r[7]);
  __cpuid(0x80000004, u.r[8], u.r[9], u.r[10], u.r[11]);
  printf("brand = %.96s\n", u.buf);

  if (max < 0x80000005)
    return;

  __cpuid(0x80000005, eax, ebx, ecx, edx);
  if (eax != 0 || ebx != 0 || ecx != 0 || edx != 0) {
    printf("\nLeaf 0x80000005: L1 Cache and TLB Identifiers\n");
    char buf1[8];
    char buf2[8];
    printf("eax = %08x (L1ITLB2M+4MSize=%u L1ITLB2M+4MAssoc=%s L1DTLB2M+4MSize=%u L1DTLB2M+4MAssoc=%s)\n",
 	   eax,
	   eax & 0xff,
	   amdassoc((eax >> 8) & 0xff, buf1),
	   (eax >> 16) & 0xff,
	   amdassoc((eax >> 24) & 0xff, buf2));
    printf("ebx = %08x (L1ITLB4kSize=%u L1ITLB4kAssoc=%s L1DTLB4kSize=%u L1DTLB4kAssoc=%s)\n",
 	   ebx,
	   ebx & 0xff,
	   amdassoc((ebx >> 8) & 0xff, buf1),
	   (ebx >> 16) & 0xff,
	   amdassoc((ebx >> 24) & 0xff, buf2));
    printf("ecx = %08x (L1D line=%u #L1D lines=%u L1DAssoc=%s L1D size=%u)\n",
 	   ecx,
	   ecx & 0xff,
	   (ecx >> 8) & 0xff,
	   amdassoc((ecx >> 8) & 0xff, buf1),
	   (ecx >> 24) & 0xff);
    printf("edx = %08x (L1I line=%u #L1I lines=%u L1IAssoc=%s L1I size=%u)\n",
 	   edx,
	   edx & 0xff,
	   (edx >> 8) & 0xff,
	   amdassoc((edx >> 8) & 0xff, buf1),
	   (edx >> 24) & 0xff);
  }

  if (max < 0x80000006)
    return;

  printf("\nLeaf 0x80000006: Extended Function CPUID Information\n");
  __cpuid(0x80000006, eax, ebx, ecx, edx);
  static const char *l2assoc[16] = {
    [0] = "disable",
    [1] = "direct",
    [2] = "2way",
    [4] = "4way",
    [6] = "8way",
    [8] = "16way",
    [10] = "32way",
    [11] = "48way",
    [12] = "64way",
    [13] = "96way",
    [14] = "128way",
    [15] = "full"
  };
  printf("ecx = %08x (clsize=%u %s %uk)\n",
	 ecx,
	 ecx & 0xff,
	 l2assoc[(ecx >> 12) & 0xf],
	 ecx >> 16);

  if (max < 0x80000007)
    return;

  __cpuid(0x80000007, eax, ebx, ecx, edx);
  if (eax != 0 || ebx != 0 || ecx != 0 || edx != 0) {
    printf("\nLeaf 0x80000007: Advanced Power Management Information\n");
    static const bit_name_map features_edx[] = {
#define F(n, s) \
      { 1u << n, #s }
      F(0, TS),
      F(1, FID),
      F(2, VID),
      F(3, TTP),
      F(4, TM),
      F(6, 100MHZSTEPS),
      F(7, HWPSTATE),
      F(8, TSCINVARIANT),
      F(9, CPB),
      F(10, EFFFREQRO),
    };
#undef F
    print_mask("edx", edx, features_edx, NARRAY(features_edx));
  }

  if (max < 0x80000008)
    return;

  printf("\nLeaf 0x80000008: Extended Function CPUID Information\n");
  __cpuid(0x80000008, eax, ebx, ecx, edx);
  char gbbuf[16];
  if ((eax >> 16) & 0xff)
    sprintf(gbbuf, "nestbits=%u", (eax >> 16) & 0xff);
  else
    gbbuf[0] = '\0';
  printf("eax = %08x (physbits=%u virtbits=%u%s)\n",
	 eax,
	 eax & 0xff,
	 (eax >> 8) & 0xff,
	 gbbuf);
  if (ecx)
    printf("ecx = %08x (nc=%u APICCoreID bits=%u)\n",
	   ecx,
	   (ecx & 0xff) + 1,
	   1u << ((ecx >> 12) & 0xf));

  if (max < 0x8000000a)
    return;

  __cpuid(0x8000000a, eax, ebx, ecx, edx);
  if (eax != 0 || ebx != 0 || ecx != 0 || edx != 0) {
    printf("\nLeaf 0x8000000a: SVM Revision and Feature Identification\n");
    printf("eax = %08x (SVM rev=%u)\n",
	   eax,
	   eax & 0xff);
    printf("ebx = %08x (nasid=%u)\n", ebx, ebx);
    static const bit_name_map features_edx[] = {
#define F(n, s) \
      { 1u << n, #s }
      F(0, NP),
      F(1, LBRVIRT),
      F(2, SVML),
      F(3, NRIPS),
      F(4, TSCRATEMSR),
      F(6, FLUSHBYASID),
      F(7, DECODEASSITS),
      F(10, PAUSEFILTER),
      F(12, PAUSEFILTERTHRESHOLD),
    };
#undef F
    print_mask("edx", edx, features_edx, NARRAY(features_edx));
  }

  if (max < 0x80000019)
    return;

  __cpuid(0x80000019, eax, ebx, ecx, edx);
  if (eax != 0 || ebx != 0 || ecx != 0 || edx != 0) {
    printf("\nLeaf 0x80000019: TLB 1GB Page Identifiers\n");
    printf("eax = %08x (L1ITLB1GSize=%u L1ITLB1GAssoc=%s L1DTLB1GSize=%u L1DTLB1GAssoc=%s)\n",
	   eax,
	   eax & 0xfff,
	   l2assoc[(eax >> 12) & 0xf],
	   (eax >> 16) & 0xfff,
	   l2assoc[(eax >> 28) & 0xf]);
    printf("ebx = %08x (L2ITLB1GSize=%u L2ITLB1GAssoc=%s L2DTLB1GSize=%u L2DTLB1GAssoc=%s)\n",
	   ebx,
	   ebx & 0xfff,
	   l2assoc[(ebx >> 12) & 0xf],
	   (ebx >> 16) & 0xfff,
	   l2assoc[(ebx >> 28) & 0xf]);
  }

  if (max < 0x8000001a)
    return;

  __cpuid(0x8000001a, eax, ebx, ecx, edx);
  if (eax != 0 || ebx != 0 || ecx != 0 || edx != 0) {
    printf("\nLeaf 0x8000001a: Performance Optimization Identifiers\n");
    static const bit_name_map features_eax[] = {
#define F(n, s) \
      { 1u << n, #s }
      F(0, FP128),
      F(1, MOVU),
    };
#undef F
    print_mask("eax", eax, features_eax, NARRAY(features_eax));
  }

  if (max < 0x8000001b)
    return;

  __cpuid(0x8000001b, eax, ebx, ecx, edx);
  if (eax != 0 || ebx != 0 || ecx != 0 || edx != 0) {
    printf("\nLeaf 0x8000001b: Instruction Based Sampling Identifiers\n");
    static const bit_name_map features_eax[] = {
#define F(n, s) \
      { 1u << n, #s }
      F(0, IBSFFV),
      F(1, FETCHSAM),
      F(2, OPSAM),
      F(3, RDWROPCNT),
      F(4, OPCNT),
      F(5, BRNTRGT),
      F(6, OPCNTEXT),
      F(7, RIPINVALIDCHK),
    };
#undef F
    print_mask("eax", eax, features_eax, NARRAY(features_eax));
  }

  if (max < 0x8000001c)
    return;

  __cpuid(0x8000001c, eax, ebx, ecx, edx);
  if (eax != 0 || ebx != 0 || ecx != 0 || edx != 0) {
    printf("\nLeaf 0x8000001c: Lightweight Profiling Capabilities 0\n");
    static const bit_name_map features_eax[] = {
#define F(n, s) \
      { 1u << n, #s }
      F(0, LWPAVAIL),
      F(1, LWPVAL),
      F(2, LWPIRE),
      F(3, LWPBRE),
      F(4, LWPDME),
      F(5, LWPCNH),
      F(31, LWPINT),
      F(1, LWPVAL),
    };
#undef F
    print_mask("eax", eax, features_eax, NARRAY(features_eax));
    printf("ebx = %08x (lwpcbsize=%u lwpeventsize=%u lwpmaxevents=%u lwpeventoffsets=%u)\n",
	   ebx,
	   ebx & 0xff,
	   (ebx >> 8) & 0xff,
	   (ebx >> 16) & 0xff,
	   (ebx >> 24) & 0xff);
    printf("ecx = %08x (lwplatencymax=%u%s lwplatencyrnd=%u lwpversion=%u lwpminbuffersize=%u%s%s%s%s)\n",
	   ecx,
	   ecx & 0x1f,
	   ecx & 0x20 ? " LWPDATAADDRESS" : "",
	   (ecx >> 6) & 0x7,
	   (ecx >> 9) & 0x7f,
	   (ecx >> 16) & 0xff,
	   ecx & 0x10000000 ? " LWPBRANCHPREDICTION" : "",
	   ecx & 0x20000000 ? " LWPIPFILTERING" : "",
	   ecx & 0x40000000 ? " LWPCACHELEVELS" : "",
	   ecx & 0x80000000 ? " LWPCACHELATENCY" : "");
    static const bit_name_map features_edx[] = {
#define F(n, s) \
      { 1u << n, #s }
      F(0, LWPAVAIL),
      F(1, LWPVAL),
      F(2, LWPIRE),
      F(3, LWPBRE),
      F(4, LWPDME),
      F(5, LWPCNH),
      F(6, LWPRNH),
      F(31, LWPINT),
    };
#undef F
    print_mask("edx", edx, features_edx, NARRAY(features_edx));
  }

  if (max < 0x8000001d)
    return;

  printf("\nLeaf 0x8000001d: Cache Properties\n");
  unsigned cnt = 0;
  while (1) {
    __cpuid_count(0x8000001d, cnt, eax, ebx, ecx, edx);
    if ((eax & 0x1f) == 0)
      break;

    if (cnt > 0)
      printf("subleaf %u:\n", cnt);
    printf("eax = %08x (L%u%s%s%s #sharing=%u)\n",
	   eax,
	   (eax >> 5) & 0x7,
	   clevel[eax & 0x1f],
	   eax & 0x100 ? " self" : "",
	   eax & 0x200 ? " full" : "",
	   ((eax >> 14) & 0xfff) + 1);
    printf("ebx = %08x (size=%u #physparticions=%u #ways=%u)\n",
	   ebx,
	   (ebx & 0xfff) + 1,
	   ((ebx >> 12) & 0x3ff) + 1,
	   ((ebx >> 22) & 0x3ff) + 1);
    printf("ecx = %08x (#sets=%u)\n", ecx, ecx + 1);
    static const bit_name_map features_edx[] = {
#define F(n, s) \
      { 1u << n, #s }
      F(0, WBINVD),
      F(1, INCLUSIVE),
    };
#undef F
    print_mask("edx", edx, features_edx, NARRAY(features_edx));
    ++cnt;
  }

  if (max < 0x8000001e)
    return;

  __cpuid(0x8000001e, eax, ebx, ecx, edx);
  if (eax != 0 || ebx != 0 || ecx != 0 || edx != 0) {
    printf("\nLeaf 0x8000001e: Extended APIC ID\n");
    printf("eax = %08x (extendedAPICID=%u)\n",
	   eax, eax);
    printf("ebx = %08x (unitID=%u #coresperunit=%u)\n",
	   ebx,
	   ebx & 0xff,
	   ((ebx >> 8) & 0x3) + 1);
    printf("ecx = %08x (nodeID=%u #nodesperprocessor=%u)\n",
	   ecx,
	   ecx & 0xff,
	   ((ecx >> 8) & 0x3) + 1);
  }
}


int
main()
{
  union {
    struct {
      unsigned eax, ebx, edx, ecx;
    };
    char s[16];
  } u;

  // eax = 0
  __cpuid(0, u.eax, u.ebx, u.ecx, u.edx);
  unsigned maxid = u.eax;
  printf("maximum input number for Basic CPUID Information = %u\n", maxid);
  printf("%.12s\n", u.s + 4);

  for (unsigned i = 1; i <= maxid; ++i) {
    unsigned eax, ebx, ecx, edx;
    __cpuid_count(i, 0, eax, ebx, ecx, edx);
    if (eax != 0 || ebx != 0 || ecx != 0 || edx != 0) {
      if (i < NARRAY(leaves) && leaves[i].handler) {
        printf("\nLeaf %u: %s\n", i, leaves[i].name);
        leaves[i].handler();
      } else
        printf("\n*** leaf %u not handled\n", i);
    }
  }

  extended_function();

  return 0;
}
