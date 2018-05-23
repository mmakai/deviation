#ifndef _BUGS3_RXID_RADIOID_MAP_H_
#define _BUGS3_RXID_RADIOID_MAP_H_

#include <stdint.h>
#include <stdbool.h>

//uint8_t most_popular_0(uint8_t i) {
//  return 0x5 << (i % 2);
//}

//uint8_t most_popular_1_cycle[] = {0x2c, 0x94, 0x3b, 0x79, 0x65, 0xb7, 0xd6, 0x8d};

//uint8_t most_popular_1(uint8_t i) {
//  uint8_t ii = i % 16;
//  uint8_t iii = most_popular_1_cycle[ii / 2];
//  return ii % 2 ? iii & 0x0f : iii >> 4;
//}

// Hex digit 1 is periodic with length 2, and hex digit 2 is periodic
// with length 16. However, storing the byte of those 2 digits
// instead of manipulating bits results simpler code and smaller binary.
uint8_t most_popular_01_cycle[] = {
  0x52, 0xac, 0x59, 0xa4, 0x53, 0xab, 0x57, 0xa9,
  0x56, 0xa5, 0x5b, 0xa7, 0x5d, 0xa6, 0x58, 0xad
};

uint8_t most_popular_01(uint8_t i) {
  return most_popular_01_cycle[i % 16];
}

uint8_t most_popular_67_cycle[] = {
  0x34, 0xc5,
  0x6a, 0xb4,
  0x29, 0xd5,
  0x2c, 0xd3,
  0x91, 0xb3,
  0x6c, 0x49,
  0x52, 0x9c,
  0x4d, 0x65,
  0xc3, 0x4a,
  0x5b, 0xd6,
  0x92,

  0x6d,
  0x94, 0xa6,
  0x55, 0xcd,
  0x2b, 0x9a,
  0x36, 0x95,
  0x4b, 0xd4,
  0x35, 0x8d,
  0x96, 0xb2,
  0xa3
};

uint8_t most_popular_67(uint8_t i) {
  if (i == 0) {
    return 0xd2;
  } else if (i == 1) {
    return 0xda;
  } else if (i % 16 < 2) {
    uint8_t ii = 2 * (i / 16) + i % 16 - 2;
    if (ii % 2 == 0) {
      return most_popular_67_cycle[ii + 7];
    } else {
      return most_popular_67_cycle[ii];
    }
  } else {
    return most_popular_67_cycle[2 * (i / 16) + (i % 16 - 2) % 7];
  }
}

uint8_t most_popular_45(uint8_t i) {
  if (i == 0) {
    return 0xa3;
  } else if (i == 1) {
    return 0x86;
  } else {
    uint8_t ii = i;
    if (i % 8 == 1) {
      ii -= 8;
    } else {
      ii -= 1;
    }
    return most_popular_67(ii);
  }
}

uint8_t most_popular_23(uint8_t i) {
  if (i == 0) {
    return 0xb2;
  } else if (i == 1) {
    return 0xcb;
  } else {
    uint8_t ii = i;
    if (i % 8 == 1) {
      ii -= 8;
    } else {
      ii -= 1;
    }
    return most_popular_45(ii);
  }
}

uint32_t most_popular(uint8_t i) {
  uint32_t ii = i + !(i <= 127);
  //return (uint32_t) most_popular_0(ii) << 28 |
  //  (uint32_t) most_popular_1(ii) << 24 |
  //  (uint32_t) most_popular_23(ii) << 16 |
  //  (uint32_t) most_popular_45(ii) << 8 |
  //  most_popular_67(ii);
  return (uint32_t) most_popular_01(ii) << 24 |
    (uint32_t) most_popular_23(ii) << 16 |
    (uint32_t) most_popular_45(ii) << 8 |
    most_popular_67(ii);
}

uint32_t second_most_popular(uint8_t i) {
  if (i < 127) {
    return most_popular(i + 1);
  } else if (i > 128) {
    return most_popular(i - 1);
  } else {
    return 0x52d6926d;
  }
}

// The 22 irregular values do not match the above periodicities. They might be
// errors from the readout, but let us try them here as long as it is not
// proven.
uint16_t irregular_keys[] = {
  1131,
  1287,
  2842,
  4668,
  5311,
  11594,
  13122,
  13813,
  20655,
  22975,
  25007,
  25068,
  28252,
  33309,
  35364,
  35765,
  37731,
  40296,
  43668,
  46540,
  49868,
  65535
};

uint32_t irregular_values[] = {
  0x52d6926d,
  0xa586da34,
  0x5329d52c,
  0xa66c4952,
  0x536c4952,
  0x524a5bd6,
  0x534d65c3,
  0xa9d391b3,
  0x5249529c,
  0xa555cd2b,
  0xac9a3695,
  0x58d391b3,
  0xa791b36c,
  0x53926d94,
  0xa7926d94,
  0xa72cd391,
  0xa9b429d5,
  0x5629d52c,
  0xad2b9a36,
  0xa74d65c3,
  0x526d94a6,
  0xad96b2a3
};

uint32_t is_irregular(uint32_t i) {
  for (uint8_t j = 0; j < sizeof(irregular_keys) / sizeof(irregular_keys[0]); ++j) {
    if (irregular_keys[j] == i) {
      return irregular_values[j];
    }
  }
  return 0;
}

uint32_t rxid_to_radioid(uint16_t rxid) {
  uint8_t block = rxid / 256;
  uint8_t second_seq_size;
  bool use_most_popular;
  if (rxid < 32768) {
    second_seq_size = 128 - block;
    use_most_popular = rxid % 256 >= second_seq_size;
  } else {
    second_seq_size = block - 127;
    use_most_popular = 255 - rxid % 256 >= second_seq_size;
  }
  uint32_t v = is_irregular(rxid);
  if (!v) {
    if (use_most_popular) {
      v = most_popular(rxid % 255);
    } else {
      v = second_most_popular(rxid % 255);
    }
  }
  return v;
}

#endif //_BUGS3_RXID_RADIOID_MAP_H_
