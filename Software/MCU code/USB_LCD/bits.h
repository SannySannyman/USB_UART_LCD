#define set_bit(reg,value) reg |= (_BV(value))
#define set_bit2(reg,value1,value2) reg |= (_BV(value1) | _BV(value2))
#define set_bit3(reg,value1,value2,value3) reg |= (_BV(value1) | _BV(value2) | _BV(value3))
#define set_bit4(reg,value1,value2,value3,value4) reg |= (_BV(value1) | _BV(value2) | _BV(value3) | _BV(value4))
#define set_bit5(reg,value1,value2,value3,value4,value5) reg |= (_BV(value1) | _BV(value2) | _BV(value3) | _BV(value4) | _BV(value5))

#define unset_bit(reg,value) reg &= ~(_BV(value))
#define unset_bit2(reg,value1,value2) reg &= ~(_BV(value1) | _BV(value2))
#define unset_bit3(reg,value1,value2,value3) reg &= ~(_BV(value1) | _BV(value2) | _BV(value3))
#define unset_bit4(reg,value1,value2,value3,value4) reg &= ~(_BV(value1) | _BV(value2) | _BV(value3) | _BV(value4))
#define unset_bit5(reg,value1,value2,value3,value4,value5) reg &= ~(_BV(value1) | _BV(value2) | _BV(value3) | _BV(value4) | _BV(value5))

#define invert_bit(reg,value) reg ^= (_BV(value))
#define invert_bit2(reg,value1,value2) reg ^= (_BV(value1) | _BV(value2))
#define invert_bit3(reg,value1,value2,value3) reg ^= (_BV(value1) | _BV(value2) | _BV(value3))
#define invert_bit4(reg,value1,value2,value3,value4) reg ^= (_BV(value1) | _BV(value2) | _BV(value3) | _BV(value4))
#define invert_bit5(reg,value1,value2,value3,value4,value5) reg ^= (_BV(value1) | _BV(value2) | _BV(value3) | _BV(value4) | _BV(value5))

#define check_bit(reg,value) ((reg&(1<<value))!= 0)