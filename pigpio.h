/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#ifndef PIGPIO_H
#define PIGPIO_H

#include <stdint.h>
#include <pthread.h>

#define PIGPIO_VERSION 34

/*TEXT

pigpio is a C library for the Raspberry which allows control of the gpios.

*Features*

o PWM on any of gpios 0-31

o servo pulses on any of gpios 0-31

o callbacks when any of gpios 0-31 change state

o callbacks at timed intervals

o reading/writing all of the gpios in a bank as one operation

o individually setting gpio modes, reading and writing

o notifications when any of gpios 0-31 change state

o the construction of output waveforms with microsecond timing

o rudimentary permission control over gpios

o a simple interface to start and stop new threads

o I2C, SPI, and serial link wrappers

o creating and running scripts

*gpios*

ALL gpios are identified by their Broadcom number.

*Credits*

The PWM and servo pulses are timed using the DMA and PWM peripherals.

This use was inspired by Richard Hirst's servoblaster kernel module.

[https://github.com/richardghirst/PiBits/tree/master/ServoBlaster]

*Usage*

Include <pigpio.h> in your source files.

Assuming your source is in prog.c use the following command to build and
run the executable.

. .
gcc -o prog prog.c -lpigpio -lpthread -lrt
sudo ./prog
. .

For examples of usage see the C programs within the pigpio archive file.

*Notes*

All the functions which return an int return < 0 on error.

If the library is not initialised all but the [*gpioCfg**], [*gpioVersion*],
and [*gpioHardwareRevision*] functions will return PI_NOT_INITIALISED.

If the library is initialised the [*gpioCfg**] functions will
return PI_INITIALISED.

TEXT*/

/*OVERVIEW

ESSENTIAL

gpioInitialise             Initialise library
gpioTerminate              Stop library

BEGINNER

gpioSetMode                Set a gpio mode
gpioGetMode                Get a gpio mode

gpioSetPullUpDown          Set/clear gpio pull up/down resistor

gpioRead                   Read a gpio
gpioWrite                  Write a gpio

gpioPWM                    Start/stop PWM pulses on a gpio
gpioGetPWMdutycycle        Get dutycycle setting on a gpio

gpioServo                  Start/stop servo pulses on a gpio
gpioGetServoPulsewidth     Get pulsewidth setting on a gpio

gpioDelay                  Delay for a number of microseconds

gpioSetAlertFunc           Request a gpio level change callback

gpioSetTimerFunc           Request a regular timed callback

INTERMEDIATE

gpioTrigger                Send a trigger pulse to a gpio.

gpioSetWatchdog            Set a watchdog on a gpio.

gpioSetPWMrange            Configure PWM range for a gpio
gpioGetPWMrange            Get configured PWM range for a gpio

gpioSetPWMfrequency        Configure PWM frequency for a gpio
gpioGetPWMfrequency        Get configured PWM frequency for a gpio

gpioRead_Bits_0_31         Read all gpios in bank 1
gpioRead_Bits_32_53        Read all gpios in bank 2

gpioWrite_Bits_0_31_Clear  Clear selected gpios in bank 1
gpioWrite_Bits_32_53_Clear Clear selected gpios in bank 2

gpioWrite_Bits_0_31_Set    Set selected gpios in bank 1
gpioWrite_Bits_32_53_Set   Set selected gpios in bank 2

gpioStartThread            Start a new thread
gpioStopThread             Stop a previously started thread

ADVANCED

gpioGetPWMrealRange        Get underlying PWM range for a gpio

gpioSetAlertFuncEx         Request a gpio change callback, extended

gpioSetSignalFunc          Request a signal callback
gpioSetSignalFuncEx        Request a signal callback, extended

gpioSetGetSamplesFunc      Requests a gpio samples callback
gpioSetGetSamplesFuncEx    Requests a gpio samples callback, extended

gpioSetTimerFuncEx         Request a regular timed callback, extended

gpioNotifyOpen             Request a notification handle
gpioNotifyBegin            Start notifications for selected gpios
gpioNotifyPause            Pause notifications
gpioNotifyClose            Close a notification

gpioSerialReadOpen         Opens a gpio for bit bang serial reads
gpioSerialRead             Reads bit bang serial data from a gpio
gpioSerialReadClose        Closes a gpio for bit bang serial reads

gpioHardwareClock          Start hardware clock on supported gpios
gpioHardwarePWM            Start hardware PWM on supported gpios

SCRIPTS

gpioStoreScript            Store a script
gpioRunScript              Run a stored script
gpioScriptStatus           Get script status and parameters
gpioStopScript             Stop a running script
gpioDeleteScript           Delete a stored script

WAVES

gpioWaveClear              Deletes all waveforms

gpioWaveAddNew             Starts a new waveform
gpioWaveAddGeneric         Adds a series of pulses to the waveform
gpioWaveAddSerial          Adds serial data to the waveform

gpioWaveCreate             Creates a waveform from added data
gpioWaveDelete             Deletes one or more waveforms

gpioWaveTxSend             Transmits a waveform

gpioWaveChain              Transmits a chain of waveforms

gpioWaveTxBusy             Checks to see if the waveform has ended
gpioWaveTxStop             Aborts the current waveform

gpioWaveGetMicros          Length in microseconds of the current waveform
gpioWaveGetHighMicros      Length of longest waveform so far
gpioWaveGetMaxMicros       Absolute maximum allowed micros

gpioWaveGetPulses          Length in pulses of the current waveform
gpioWaveGetHighPulses      Length of longest waveform so far
gpioWaveGetMaxPulses       Absolute maximum allowed pulses

gpioWaveGetCbs             Length in control blocks of the current waveform
gpioWaveGetHighCbs         Length of longest waveform so far
gpioWaveGetMaxCbs          Absolute maximum allowed control blocks

gpioWaveTxStart            Creates/transmits a waveform (DEPRECATED)

I2C

i2cOpen                    Opens an I2C device
i2cClose                   Closes an I2C device

i2cWriteQuick              SMBus write quick
i2cWriteByte               SMBus write byte
i2cReadByte                SMBus read byte
i2cWriteByteData           SMBus write byte data
i2cWriteWordData           SMBus write word data
i2cReadByteData            SMBus read byte data
i2cReadWordData            SMBus read word data
i2cProcessCall             SMBus process call
i2cWriteBlockData          SMBus write block data
i2cReadBlockData           SMBus read block data
i2cBlockProcessCall        SMBus block process call

i2cWriteI2CBlockData       SMBus write I2C block data
i2cReadI2CBlockData        SMBus read I2C block data

i2cReadDevice              Reads the raw I2C device
i2cWriteDevice             Writes the raw I2C device

i2cSwitchCombined          Sets or clears the combined flag

i2cSegments                Performs multiple I2C transactions

i2cZip                     Performs multiple I2C transactions

bbI2COpen                  Opens gpios for bit banging I2C
bbI2CClose                 Closes gpios for bit banging I2C
bbI2CZip                   Performs multiple bit banged I2C transactions

SPI

spiOpen                    Opens a SPI device
spiClose                   Closes a SPI device

spiRead                    Reads bytes from a SPI device
spiWrite                   Writes bytes to a SPI device
spiXfer                    Transfers bytes with a SPI device

SERIAL

serOpen                    Opens a serial device (/dev/tty*)
serClose                   Closes a serial device

serWriteByte               Writes a byte to a serial device
serReadByte                Reads a byte from a serial device
serWrite                   Writes bytes to a serial device
serRead                    Reads bytes from a serial device

serDataAvailable           Returns number of bytes ready to be read

CONFIGURATION

gpioCfgBufferSize          Configure the gpio sample buffer size
gpioCfgClock               Configure the gpio sample rate
gpioCfgDMAchannel          Configure the DMA channel (DEPRECATED)
gpioCfgDMAchannels         Configure the DMA channels
gpioCfgPermissions         Configure the gpio access permissions
gpioCfgInterfaces          Configure user interfaces
gpioCfgInternals           Configure miscellaneous internals
gpioCfgSocketPort          Configure socket port
gpioCfgMemAlloc            Configure DMA memory allocation mode

CUSTOM

gpioCustom1                User custom function 1
gpioCustom2                User custom function 2

UTILITIES

gpioTick                   Get current tick (microseconds)

gpioHardwareRevision       Get hardware revision
gpioVersion                Get the pigpio version

getBitInBytes              Get the value of a bit
putBitInBytes              Set the value of a bit

gpioTime                   Get current time
gpioSleep                  Sleep for specified time

time_sleep                 Sleeps for a float number of seconds
time_time                  Float number of seconds since the epoch

EXPERT

rawWaveAddSPI              Not intended for general use
rawWaveAddGeneric          Not intended for general use
rawWaveCB                  Not intended for general use
rawWaveCBAdr               Not intended for general use
rawWaveGetOut              Not intended for general use
rawWaveSetOut              Not intended for general use
rawWaveGetIn               Not intended for general use
rawWaveSetIn               Not intended for general use
rawWaveInfo                Not intended for general use
rawDumpWave                Not intended for general use
rawDumpScript              Not intended for general use

OVERVIEW*/

#define PI_INPFIFO "/dev/pigpio"
#define PI_OUTFIFO "/dev/pigout"
#define PI_ERRFIFO "/dev/pigerr"

#define PI_ENVPORT "PIGPIO_PORT"
#define PI_ENVADDR "PIGPIO_ADDR"

#define PI_LOCKFILE "/var/run/pigpio.pid"

#define PI_I2C_COMBINED "/sys/module/i2c_bcm2708/parameters/combined"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   uint16_t func;
   uint16_t size;
} gpioHeader_t;

typedef struct
{
   size_t size;
   void *ptr;
   uint32_t data;
} gpioExtent_t;

typedef struct
{
   uint32_t tick;
   uint32_t level;
} gpioSample_t;

typedef struct
{
   uint16_t seqno;
   uint16_t flags;
   uint32_t tick;
   uint32_t level;
} gpioReport_t;

typedef struct
{
   uint32_t gpioOn;
   uint32_t gpioOff;
   uint32_t usDelay;
} gpioPulse_t;

#define WAVE_FLAG_READ  1
#define WAVE_FLAG_TICK  2

typedef struct
{
   uint32_t gpioOn;
   uint32_t gpioOff;
   uint32_t usDelay;
   uint32_t flags;
} rawWave_t;

typedef struct
{
   uint16_t botCB;  /* first CB used by wave  */
   uint16_t topCB;  /* last CB used by wave   */
   uint16_t botOOL; /* last OOL used by wave  */
   uint16_t topOOL; /* first OOL used by wave */
} rawWaveInfo_t;

typedef struct
{
   int clk;     /* gpio for clock           */
   int mosi;    /* gpio for MOSI            */
   int miso;    /* gpio for MISO            */
   int ss_pol;  /* slave select off state   */
   int ss_us;   /* delay after slave select */
   int clk_pol; /* clock off state          */
   int clk_pha; /* clock phase              */
   int clk_us;  /* clock micros             */
} rawSPI_t;

typedef struct { /* linux/arch/arm/mach-bcm2708/include/mach/dma.h */
   uint32_t info;
   uint32_t src;
   uint32_t dst;
   uint32_t length;
   uint32_t stride;
   uint32_t next;
   uint32_t pad[2];
} rawCbs_t;

typedef struct
{
   uint16_t addr;  /* slave address       */
   uint16_t flags;
   uint16_t len;   /* msg length          */
   uint8_t  *buf;  /* pointer to msg data */
} pi_i2c_msg_t;

typedef void (*gpioAlertFunc_t)    (int      gpio,
                                    int      level,
                                    uint32_t tick);

typedef void (*gpioAlertFuncEx_t)  (int      gpio,
                                    int      level,
                                    uint32_t tick,
                                    void    *userdata);

typedef void (*gpioTimerFunc_t)    (void);

typedef void (*gpioTimerFuncEx_t)  (void *userdata);

typedef void (*gpioSignalFunc_t)   (int signum);

typedef void (*gpioSignalFuncEx_t) (int    signum,
                                    void  *userdata);

typedef void (*gpioGetSamplesFunc_t)   (const gpioSample_t *samples,
                                        int                 numSamples);

typedef void (*gpioGetSamplesFuncEx_t) (const gpioSample_t *samples,
                                        int                 numSamples,
                                        void               *userdata);

typedef void *(gpioThreadFunc_t) (void *);


/* gpio: 0-53 */

#define PI_MIN_GPIO       0
#define PI_MAX_GPIO      53

/* user_gpio: 0-31 */

#define PI_MAX_USER_GPIO 31

/* level: 0-1 */

#define PI_OFF   0
#define PI_ON    1

#define PI_CLEAR 0
#define PI_SET   1

#define PI_LOW   0
#define PI_HIGH  1

/* level: only reported for gpio time-out, see gpioSetWatchdog */

#define PI_TIMEOUT 2

/* mode: 0-7 */

#define PI_INPUT  0
#define PI_OUTPUT 1
#define PI_ALT0   4
#define PI_ALT1   5
#define PI_ALT2   6
#define PI_ALT3   7
#define PI_ALT4   3
#define PI_ALT5   2

/* pud: 0-2 */

#define PI_PUD_OFF  0
#define PI_PUD_DOWN 1
#define PI_PUD_UP   2

/* dutycycle: 0-range */

#define PI_DEFAULT_DUTYCYCLE_RANGE   255

/* range: 25-40000 */

#define PI_MIN_DUTYCYCLE_RANGE        25
#define PI_MAX_DUTYCYCLE_RANGE     40000

/* pulsewidth: 0, 500-2500 */

#define PI_SERVO_OFF 0
#define PI_MIN_SERVO_PULSEWIDTH 500
#define PI_MAX_SERVO_PULSEWIDTH 2500

/* hardware PWM */

#define PI_HW_PWM_MIN_FREQ 1
#define PI_HW_PWM_MAX_FREQ 125000000
#define PI_HW_PWM_RANGE 1000000

/* hardware clock */

#define PI_HW_CLK_MIN_FREQ 4689
#define PI_HW_CLK_MAX_FREQ 250000000

#define PI_NOTIFY_SLOTS  32

#define PI_NTFY_FLAGS_ALIVE    (1 <<6)
#define PI_NTFY_FLAGS_WDOG     (1 <<5)
#define PI_NTFY_FLAGS_BIT(x) (((x)<<0)&31)

#define PI_WAVE_BLOCKS     4
#define PI_WAVE_MAX_PULSES (PI_WAVE_BLOCKS * 3000)
#define PI_WAVE_MAX_CHARS  (PI_WAVE_BLOCKS *  300)

#define PI_BB_I2C_MIN_BAUD     50
#define PI_BB_I2C_MAX_BAUD 500000

#define PI_BB_SER_MIN_BAUD     50
#define PI_BB_SER_MAX_BAUD 250000

#define PI_WAVE_MIN_BAUD      50
#define PI_WAVE_MAX_BAUD 1000000

//#define PI_SPI_MIN_BAUD     32000
//#define PI_SPI_MAX_BAUD 125000000

#define PI_SPI_MIN_BAUD     1
#define PI_SPI_MAX_BAUD 500000000

#define PI_MIN_WAVE_DATABITS 1
#define PI_MAX_WAVE_DATABITS 32

#define PI_MIN_WAVE_HALFSTOPBITS 2
#define PI_MAX_WAVE_HALFSTOPBITS 8

#define PI_WAVE_MAX_MICROS (30 * 60 * 1000000) /* half an hour */

#define PI_MAX_WAVES       250
//#define PI_MAX_WAVE_CYCLES 537824
#define PI_MAX_WAVE_CYCLES 16777216
#define PI_WAVE_COUNTERS   5

/* wave tx mode */

#define PI_WAVE_MODE_ONE_SHOT 0
#define PI_WAVE_MODE_REPEAT   1

/* I2C, SPI, SER */

#define PI_I2C_SLOTS 32
#define PI_SPI_SLOTS 16
#define PI_SER_SLOTS 8

#define PI_NUM_I2C_BUS 2
#define PI_MAX_I2C_ADDR 0x7F

#define PI_NUM_AUX_SPI_CHANNEL 3
#define PI_NUM_STD_SPI_CHANNEL 2

#define PI_MAX_I2C_DEVICE_COUNT (1<<16)
#define PI_MAX_SPI_DEVICE_COUNT (1<<16)

/* max pi_i2c_msg_t per transaction */

#define  PI_I2C_RDRW_IOCTL_MAX_MSGS 42

/* flags for i2cTransaction, pi_i2c_msg_t */

#define PI_I2C_M_WR           0x0000 /* write data */
#define PI_I2C_M_RD           0x0001 /* read data */
#define PI_I2C_M_TEN          0x0010 /* ten bit chip address */
#define PI_I2C_M_RECV_LEN     0x0400 /* length will be first received byte */
#define PI_I2C_M_NO_RD_ACK    0x0800 /* if I2C_FUNC_PROTOCOL_MANGLING */
#define PI_I2C_M_IGNORE_NAK   0x1000 /* if I2C_FUNC_PROTOCOL_MANGLING */
#define PI_I2C_M_REV_DIR_ADDR 0x2000 /* if I2C_FUNC_PROTOCOL_MANGLING */
#define PI_I2C_M_NOSTART      0x4000 /* if I2C_FUNC_PROTOCOL_MANGLING */

/* bbI2CZip and i2cZip commands */

#define PI_I2C_END          0
#define PI_I2C_ESC          1
#define PI_I2C_START        2
#define PI_I2C_COMBINED_ON  2
#define PI_I2C_STOP         3
#define PI_I2C_COMBINED_OFF 3
#define PI_I2C_ADDR         4
#define PI_I2C_FLAGS        5
#define PI_I2C_READ         6
#define PI_I2C_WRITE        7

/* SPI */

#define PI_SPI_FLAGS_BITLEN(x) ((x&63)<<16)
#define PI_SPI_FLAGS_RX_LSB(x)  ((x&1)<<15)
#define PI_SPI_FLAGS_TX_LSB(x)  ((x&1)<<14)
#define PI_SPI_FLAGS_3WREN(x)  ((x&15)<<10)
#define PI_SPI_FLAGS_3WIRE(x)   ((x&1)<<9)
#define PI_SPI_FLAGS_AUX_SPI(x) ((x&1)<<8)
#define PI_SPI_FLAGS_RESVD(x)   ((x&7)<<5)
#define PI_SPI_FLAGS_CSPOLS(x)  ((x&7)<<2)
#define PI_SPI_FLAGS_MODE(x)    ((x&3))

/* Longest busy delay */

#define PI_MAX_BUSY_DELAY 100

/* timeout: 0-60000 */

#define PI_MIN_WDOG_TIMEOUT 0
#define PI_MAX_WDOG_TIMEOUT 60000

/* timer: 0-9 */

#define PI_MIN_TIMER 0
#define PI_MAX_TIMER 9

/* millis: 10-60000 */

#define PI_MIN_MS 10
#define PI_MAX_MS 60000

#define PI_MAX_SCRIPTS       32

#define PI_MAX_SCRIPT_TAGS   50
#define PI_MAX_SCRIPT_VARS  150
#define PI_MAX_SCRIPT_PARAMS 10

/* script status */

#define PI_SCRIPT_INITING 0
#define PI_SCRIPT_HALTED  1
#define PI_SCRIPT_RUNNING 2
#define PI_SCRIPT_WAITING 3
#define PI_SCRIPT_FAILED  4

/* signum: 0-63 */

#define PI_MIN_SIGNUM 0
#define PI_MAX_SIGNUM 63

/* timetype: 0-1 */

#define PI_TIME_RELATIVE 0
#define PI_TIME_ABSOLUTE 1

#define PI_MAX_MICS_DELAY 1000000 /* 1 second */
#define PI_MAX_MILS_DELAY 60000   /* 60 seconds */

/* cfgMillis */

#define PI_BUF_MILLIS_MIN 100
#define PI_BUF_MILLIS_MAX 10000

/* cfgMicros: 1, 2, 4, 5, 8, or 10 */

/* cfgPeripheral: 0-1 */

#define PI_CLOCK_PWM 0
#define PI_CLOCK_PCM 1

/* DMA channel: 0-14 */

#define PI_MIN_DMA_CHANNEL 0
#define PI_MAX_DMA_CHANNEL 14

#define PI_MAX_PRIMARY_CHANNEL   14
#define PI_MAX_SECONDARY_CHANNEL  6

/* port */

#define PI_MIN_SOCKET_PORT 1024
#define PI_MAX_SOCKET_PORT 32000


/* ifFlags: */

#define PI_DISABLE_FIFO_IF 1
#define PI_DISABLE_SOCK_IF 2

/* memAllocMode */

#define PI_MEM_ALLOC_AUTO    0
#define PI_MEM_ALLOC_PAGEMAP 1
#define PI_MEM_ALLOC_MAILBOX 2

/*F*/
int gpioInitialise(void);
/*D
Initialises the library.

Call before using the other library functions.

Returns the pigpio version number if OK, otherwise PI_INIT_FAILED.

The only exception is the optional [*gpioCfg**] functions, see later.

...
if (gpioInitialise() < 0)
{
   // pigpio initialisation failed.
}
else
{
   // pigpio initialised okay.
}
...
D*/


/*F*/
void gpioTerminate(void);
/*D
Terminates the library.

Returns nothing.

Call before program exit.

This function resets the used DMA channels, releases memory, and
terminates any running threads.

...
gpioTerminate();
...
D*/


/*F*/
int gpioSetMode(unsigned gpio, unsigned mode);
/*D
Sets the gpio mode, typically input or output.

. .
gpio: 0-53
mode: 0-7
. .

Returns 0 if OK, otherwise PI_BAD_GPIO or PI_BAD_MODE.

Arduino style: pinMode.

...
gpioSetMode(17, PI_INPUT);  // Set gpio17 as input.

gpioSetMode(18, PI_OUTPUT); // Set gpio18 as output.

gpioSetMode(22,PI_ALT0);    // Set gpio22 to alternative mode 0.
...
D*/


/*F*/
int gpioGetMode(unsigned gpio);
/*D
Gets the gpio mode.

. .
gpio: 0-53
. .

Returns the gpio mode if OK, otherwise PI_BAD_GPIO.

...
if (gpioGetMode(17) != PI_ALT0)
{
   gpioSetMode(17, PI_ALT0);  // set gpio17 to ALT0
}
...
D*/


/*F*/
int gpioSetPullUpDown(unsigned gpio, unsigned pud);
/*D
Sets or clears resistor pull ups or downs on the gpio.

. .
gpio: 0-53
 pud: 0-2
. .

Returns 0 if OK, otherwise PI_BAD_GPIO or PI_BAD_PUD.

...
gpioSetPullUpDown(17, PI_PUD_UP);   // Sets a pull-up.

gpioSetPullUpDown(18, PI_PUD_DOWN); // Sets a pull-down.

gpioSetPullUpDown(23, PI_PUD_OFF);  // Clear any pull-ups/downs.
...
D*/


/*F*/
int gpioRead (unsigned gpio);
/*D
Reads the gpio level, on or off.

. .
gpio: 0-53
. .

Returns the gpio level if OK, otherwise PI_BAD_GPIO.

Arduino style: digitalRead.

...
printf("gpio24 is level %d\n", gpioRead(24));
...
D*/


/*F*/
int gpioWrite(unsigned gpio, unsigned level);
/*D
Sets the gpio level, on or off.

. .
 gpio: 0-53
level: 0,1
. .

Returns 0 if OK, otherwise PI_BAD_GPIO or PI_BAD_LEVEL.

If PWM or servo pulses are active on the gpio they are switched off.

Arduino style: digitalWrite

...
gpioWrite(24, 1); // Set gpio24 high.
...
D*/


/*F*/
int gpioPWM(unsigned user_gpio, unsigned dutycycle);
/*D
Starts PWM on the gpio, dutycycle between 0 (off) and range (fully on).
Range defaults to 255.

. .
user_gpio: 0-31
dutycycle: 0-range
. .

Returns 0 if OK, otherwise PI_BAD_USER_GPIO or PI_BAD_DUTYCYCLE.

Arduino style: analogWrite

This and the servo functionality use the DMA and PWM or PCM peripherals
to control and schedule the pulse lengths and dutycycles.

The [*gpioSetPWMrange*] function may be used to change the default
range of 255.

...
gpioPWM(17, 255); // Sets gpio17 full on.

gpioPWM(18, 128); // Sets gpio18 half on.

gpioPWM(23, 0);   // Sets gpio23 full off.
...
D*/


/*F*/
int gpioGetPWMdutycycle(unsigned user_gpio);
/*D
Returns the PWM dutycycle setting for the gpio.

. .
user_gpio: 0-31
. .

Returns between 0 (off) and range (fully on) if OK, otherwise
PI_BAD_USER_GPIO or PI_NOT_PWM_GPIO.

For normal PWM the dutycycle will be out of the defined range
for the gpio (see [*gpioGetPWMrange*]).

If a hardware clock is active on the gpio the reported dutycycle
will be 500000 (500k) out of 1000000 (1M).

If hardware PWM is active on the gpio the reported dutycycle
will be out of a 1000000 (1M).

Normal PWM range defaults to 255.
D*/


/*F*/
int gpioSetPWMrange(unsigned user_gpio, unsigned range);
/*D
Selects the dutycycle range to be used for the gpio.  Subsequent calls
to gpioPWM will use a dutycycle between 0 (off) and range (fully on).

. .
user_gpio: 0-31
    range: 25-40000
. .

Returns the real range for the given gpio's frequency if OK,
otherwise PI_BAD_USER_GPIO or PI_BAD_DUTYRANGE.

If PWM is currently active on the gpio its dutycycle will be scaled
to reflect the new range.

The real range, the number of steps between fully off and fully
on for each frequency, is given in the following table.

. .
  25,   50,  100,  125,  200,  250,  400,   500,   625,
 800, 1000, 1250, 2000, 2500, 4000, 5000, 10000, 20000
. .

The real value set by [*gpioPWM*] is (dutycycle * real range) / range.

...
gpioSetPWMrange(24, 2000); // Now 2000 is fully on
                           //     1000 is half on
                           //      500 is quarter on, etc.
...
D*/


/*F*/
int gpioGetPWMrange(unsigned user_gpio);
/*D
Returns the dutycycle range used for the gpio if OK, otherwise
PI_BAD_USER_GPIO.

. .
user_gpio: 0-31
. .

If a hardware clock or hardware PWM is active on the gpio
the reported range will be 1000000 (1M).

...
r = gpioGetPWMrange(23);
...
D*/


/*F*/
int gpioGetPWMrealRange(unsigned user_gpio);
/*D
Returns the real range used for the gpio if OK, otherwise
PI_BAD_USER_GPIO.

. .
user_gpio: 0-31
. .

If a hardware clock is active on the gpio the reported real
range will be 1000000 (1M).

If hardware PWM is active on the gpio the reported real range
will be approximately 250M divided by the set PWM frequency.

...
rr = gpioGetPWMrealRange(17);
...
D*/


/*F*/
int gpioSetPWMfrequency(unsigned user_gpio, unsigned frequency);
/*D
Sets the frequency in hertz to be used for the gpio.

. .
user_gpio: 0-31
frequency: >=0
. .

Returns the numerically closest frequency if OK, otherwise
PI_BAD_USER_GPIO.

The selectable frequencies depend upon the sample rate which
may be 1, 2, 4, 5, 8, or 10 microseconds (default 5).

Each gpio can be independently set to one of 18 different PWM
frequencies.

If PWM is currently active on the gpio it will be
switched off and then back on at the new frequency.

The frequencies for each sample rate are:

. .
                       Hertz

       1: 40000 20000 10000 8000 5000 4000 2500 2000 1600
           1250  1000   800  500  400  250  200  100   50

       2: 20000 10000  5000 4000 2500 2000 1250 1000  800
            625   500   400  250  200  125  100   50   25

       4: 10000  5000  2500 2000 1250 1000  625  500  400
            313   250   200  125  100   63   50   25   13
sample
 rate
 (us)  5:  8000  4000  2000 1600 1000  800  500  400  320
            250   200   160  100   80   50   40   20   10

       8:  5000  2500  1250 1000  625  500  313  250  200
            156   125   100   63   50   31   25   13    6

      10:  4000  2000  1000  800  500  400  250  200  160
            125   100    80   50   40   25   20   10    5
. .

...
gpioSetPWMfrequency(23, 0); // Set gpio23 to lowest frequency.

gpioSetPWMfrequency(24, 500); // Set gpio24 to 500Hz.

gpioSetPWMfrequency(25, 100000); // Set gpio25 to highest frequency.
...
D*/


/*F*/
int gpioGetPWMfrequency(unsigned user_gpio);
/*D
Returns the frequency (in hertz) used for the gpio if OK, otherwise
PI_BAD_USER_GPIO.

. .
user_gpio: 0-31
. .

For normal PWM the frequency will be that defined for the gpio by
[*gpioSetPWMfrequency*].

If a hardware clock is active on the gpio the reported frequency
will be that set by [*gpioHardwareClock*].

If hardware PWM is active on the gpio the reported frequency
will be that set by [*gpioHardwarePWM*].

...
f = gpioGetPWMfrequency(23); // Get frequency used for gpio23.
...
D*/


/*F*/
int gpioServo(unsigned user_gpio, unsigned pulsewidth);
/*D
Starts servo pulses on the gpio, 0 (off), 500 (most anti-clockwise) to
2500 (most clockwise).

. .
 user_gpio: 0-31
pulsewidth: 0, 500-2500
. .

Returns 0 if OK, otherwise PI_BAD_USER_GPIO or PI_BAD_PULSEWIDTH.

The range supported by servos varies and should probably be determined
by experiment.  A value of 1500 should always be safe and represents
the mid-point of rotation.  You can DAMAGE a servo if you command it
to move beyond its limits.

The following causes an on pulse of 1500 microseconds duration to be
transmitted on gpio 17 at a rate of 50 times per second. This will
command a servo connected to gpio 17 to rotate to its mid-point.

...
gpioServo(17, 1000); // Move servo to safe position anti-clockwise.

gpioServo(23, 1500); // Move servo to centre position.

gpioServo(25, 2000); // Move servo to safe position clockwise.
...

OTHER UPDATE RATES:

This function updates servos at 50Hz.  If you wish to use a different
update frequency you will have to use the PWM functions.

. .
PWM Hz    50   100  200  400  500
1E6/Hz 20000 10000 5000 2500 2000
. .

Firstly set the desired PWM frequency using [*gpioSetPWMfrequency*].

Then set the PWM range using [*gpioSetPWMrange*] to 1E6/frequency.
Doing this allows you to use units of microseconds when setting
the servo pulsewidth.

E.g. If you want to update a servo connected to gpio25 at 400Hz

. .
gpioSetPWMfrequency(25, 400);

gpioSetPWMrange(25, 2500);
. .

Thereafter use the PWM command to move the servo,
e.g. gpioPWM(25, 1500) will set a 1500 us pulse.
D*/


/*F*/
int gpioGetServoPulsewidth(unsigned user_gpio);
/*D
Returns the servo pulsewidth setting for the gpio.

. .
user_gpio: 0-31
. .

Returns , 0 (off), 500 (most anti-clockwise) to 2500 (most clockwise)
if OK, otherwise PI_BAD_USER_GPIO or PI_NOT_SERVO_GPIO.
D*/


/*F*/
int gpioSetAlertFunc(unsigned user_gpio, gpioAlertFunc_t f);
/*D
Registers a function to be called (a callback) when the specified
gpio changes state.

. .
user_gpio: 0-31
        f: the callback function
. .

Returns 0 if OK, otherwise PI_BAD_USER_GPIO.

One function may be registered per gpio.

The function is passed the gpio, the new level, and the tick.

The alert may be cancelled by passing NULL as the function.

The gpios are sampled at a rate set when the library is started.

If a value isn't specifically set the default of 5 us is used.

The number of samples per second is given in the following table.

. .
              samples
              per sec

         1  1,000,000
         2    500,000
sample   4    250,000
rate     5    200,000
(us)     8    125,000
        10    100,000
. .

Level changes shorter than the sample rate may be missed.

The thread which calls the alert functions is triggered nominally
1000 times per second.  The active alert functions will be called
once per level change since the last time the thread was activated.
i.e. The active alert functions will get all level changes but there
will be a latency.

The tick value is the time stamp of the sample in microseconds, see
[*gpioTick*] for more details.

...
void aFunction(int gpio, int level, uint32_t tick)
{
   printf("gpio %d became %d at %d\n", gpio, level, tick);
}

// call aFunction whenever gpio 4 changes state

gpioSetAlertFunc(4, aFunction);
...
D*/


/*F*/
int gpioSetAlertFuncEx(
   unsigned user_gpio, gpioAlertFuncEx_t f, void *userdata);
/*D
Registers a function to be called (a callback) when the specified
gpio changes state.

. .
user_gpio: 0-31
        f: the callback function
 userdata: pointer to arbitrary user data
. .

Returns 0 if OK, otherwise PI_BAD_USER_GPIO.

One function may be registered per gpio.

The function is passed the gpio, the new level, the tick, and
the userdata pointer.

Only one of [*gpioSetAlertFunc*] or [*gpioSetAlertFuncEx*] can be
registered per gpio.

See [*gpioSetAlertFunc*] for further details.
D*/


/*F*/
int gpioNotifyOpen(void);
/*D
This function requests a free notification handle.

Returns a handle greater than or equal to zero if OK,
otherwise PI_NO_HANDLE.

A notification is a method for being notified of gpio state changes
via a pipe or socket.

Pipe notifications for handle x will be available at the pipe
named /dev/pigpiox (where x is the handle number).  E.g. if the
function returns 15 then the notifications must be read
from /dev/pigpio15.

Socket notifications are returned to the socket which requested the
handle.

...
h = gpioNotifyOpen();

if (h >= 0)
{
   sprintf(str, "/dev/pigpio%d", h);

   fd = open(str, "r");

   if (fd >= 0)
   {
      // Okay.
   }
   else
   {
      // Error.
   }
}
else
{
   // Error.
}
...
D*/


/*F*/
int gpioNotifyBegin(unsigned handle, uint32_t bits);
/*D
This function starts notifications on a previously opened handle.

. .
handle: >=0, as returned by [*gpioNotifyOpen*]
  bits: a bit mask indicating the gpios of interest
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE.

The notification sends state changes for each gpio whose corresponding
bit in bits is set.

Each notification occupies 12 bytes in the fifo and has the
following structure.

. .
typedef struct
{
   uint16_t seqno;
   uint16_t flags;
   uint32_t tick;
   uint32_t level;
} gpioReport_t;
. .

seqno starts at 0 each time the handle is opened and then increments
by one for each report.

flags, if bit 5 is set then bits 0-4 of the flags indicate a gpio
which has had a watchdog timeout.

tick is the number of microseconds since system boot.

level indicates the level of each gpio.

...
// Start notifications for gpios 1, 4, 6, 7, 10.

//                         1
//                         0  76 4  1
// (1234 = 0x04D2 = 0b0000010011010010)

gpioNotifyBegin(h, 1234);
...
D*/


/*F*/
int gpioNotifyPause(unsigned handle);
/*D
This function pauses notifications on a previously opened handle.

. .
handle: >=0, as returned by [*gpioNotifyOpen*]
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE.

Notifications for the handle are suspended until [*gpioNotifyBegin*]
is called again.

...
gpioNotifyPause(h);
...
D*/


/*F*/
int gpioNotifyClose(unsigned handle);
/*D
This function stops notifications on a previously opened handle
and releases the handle for reuse.

. .
handle: >=0, as returned by [*gpioNotifyOpen*]
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE.

...
gpioNotifyClose(h);
...
D*/


/*F*/
int gpioWaveClear(void);
/*D
This function clears all waveforms and any data added by calls to the
[*gpioWaveAdd**] functions.

Returns 0 if OK.

...
gpioWaveClear();
...
D*/


/*F*/
int gpioWaveAddNew(void);
/*D
This function starts a new empty waveform.

You wouldn't normally need to call this function as it is automatically
called after a waveform is created with the [*gpioWaveCreate*] function.

Returns 0 if OK.

...
gpioWaveAddNew();
...
D*/


/*F*/
int gpioWaveAddGeneric(unsigned numPulses, gpioPulse_t *pulses);
/*D
This function adds a number of pulses to the current waveform.

. .
numPulses: the number of pulses
   pulses: an array of pulses
. .

Returns the new total number of pulses in the current waveform if OK,
otherwise PI_TOO_MANY_PULSES.

The pulses are interleaved in time order within the existing waveform
(if any).

Merging allows the waveform to be built in parts, that is the settings
for gpio#1 can be added, and then gpio#2 etc.

If the added waveform is intended to start after or within the existing
waveform then the first pulse should consist of a delay.

...
// Construct and send a 30 microsecond square wave.

gpioSetMode(gpio, PI_OUTPUT);

pulse[0].gpioOn = (1<<gpio);
pulse[0].gpioOff = 0;
pulse[0].usDelay = 15;

pulse[1].gpioOn = 0;
pulse[1].gpioOff = (1<<gpio);
pulse[1].usDelay = 15;

gpioWaveAddNew();

gpioWaveAddGeneric(2, pulse);

wave_id = gpioWaveCreate();

if (wave_id >= 0)
{
   gpioWaveTxSend(wave_id, PI_WAVE_MODE_REPEAT);

   // Transmit for 30 seconds.

   sleep(30);

   gpioWaveTxStop();
}
else
{
   // Wave create failed.
}
...
D*/


/*F*/
int gpioWaveAddSerial
   (unsigned user_gpio,
    unsigned baud,
    unsigned data_bits,
    unsigned stop_bits,
    unsigned offset,
    unsigned numBytes,
    char     *str);
/*D
This function adds a waveform representing serial data to the
existing waveform (if any).  The serial data starts offset
microseconds from the start of the waveform.

. .
user_gpio: 0-31
     baud: 50-1000000
data_bits: 1-32
stop_bits: 2-8
   offset: 0-
 numBytes: 1-
      str: an array of chars (which may contain nulls)
. .

Returns the new total number of pulses in the current waveform if OK,
otherwise PI_BAD_USER_GPIO, PI_BAD_WAVE_BAUD, PI_BAD_DATABITS,
PI_BAD_STOPBITS, PI_TOO_MANY_CHARS, PI_BAD_SER_OFFSET,
or PI_TOO_MANY_PULSES.

NOTES:

The serial data is formatted as one start bit, data_bits data bits, and
stop_bits/2 stop bits.

It is legal to add serial data streams with different baud rates to
the same waveform.

numBytes is the number of bytes of data in str.

The bytes required for each character depend upon data_bits.

For data_bits 1-8 there will be one byte per character. 
For data_bits 9-16 there will be two bytes per character. 
For data_bits 17-32 there will be four bytes per character.

...
#define MSG_LEN 8

int i;
char *str;
char data[MSG_LEN];

str = "Hello world!";

gpioWaveAddSerial(4, 9600, 8, 2, 0, strlen(str), str);

for (i=0; i<MSG_LEN; i++) data[i] = i;

// Data added is offset 1 second from the waveform start.
gpioWaveAddSerial(4, 9600, 8, 2, 1000000, MSG_LEN, data);
...
D*/


/*F*/
int gpioWaveCreate(void);
/*D
This function creates a waveform from the data provided by the prior
calls to the [*gpioWaveAdd**] functions.  Upon success a positive wave id
is returned.

The data provided by the [*gpioWaveAdd**] functions is consumed by this
function.

As many waveforms may be created as there is space available.  The
wave id is passed to [*gpioWaveTxSend*] to specify the waveform to transmit.

Normal usage would be

Step 1. [*gpioWaveClear*] to clear all waveforms and added data.

Step 2. [*gpioWaveAdd**] calls to supply the waveform data.

Step 3. [*gpioWaveCreate*] to create the waveform and get a unique id

Repeat steps 2 and 3 as needed.

Step 4. [*gpioWaveTxSend*] with the id of the waveform to transmit.

A waveform comprises one of more pulses.  Each pulse consists of a
[*gpioPulse_t*] structure.

. .
typedef struct
{
   uint32_t gpioOn;
   uint32_t gpioOff;
   uint32_t usDelay;
} gpioPulse_t;
. .

The fields specify

1) the gpios to be switched on at the start of the pulse. 
2) the gpios to be switched off at the start of the pulse. 
3) the delay in microseconds before the next pulse.

Any or all the fields can be zero.  It doesn't make any sense to
set all the fields to zero (the pulse will be ignored).

When a waveform is started each pulse is executed in order with the
specified delay between the pulse and the next.

Returns the new waveform id if OK, otherwise PI_EMPTY_WAVEFORM,
PI_NO_WAVEFORM_ID, PI_TOO_MANY_CBS, or PI_TOO_MANY_OOL.
D*/


/*F*/
int gpioWaveDelete(unsigned wave_id);
/*D
This function deletes all created waveforms with ids greater than or
equal to wave_id.

. .
wave_id: >=0, as returned by [*gpioWaveCreate*]
. .

Wave ids are allocated in order, 0, 1, 2, etc.

Returns 0 if OK, otherwise PI_BAD_WAVE_ID.
D*/


/*F*/
int gpioWaveTxStart(unsigned wave_mode); /* DEPRECATED */
/*D
This function creates and then transmits a waveform.  The mode
determines whether the waveform is sent once or cycles endlessly.

NOTE: Any hardware PWM started by [*gpioHardwarePWM*] will be cancelled.

. .
wave_mode: 0 (PI_WAVE_MODE_ONE_SHOT), 1 (PI_WAVE_MODE_REPEAT)
. .

This function is deprecated and should no longer be used.  Use
[*gpioWaveCreate*] and [*gpioWaveTxSend*] instead.

Returns the number of DMA control blocks in the waveform if OK,
otherwise PI_BAD_WAVE_MODE.
D*/


/*F*/
int gpioWaveTxSend(unsigned wave_id, unsigned wave_mode);
/*D
This function transmits the waveform with id wave_id.  The mode
determines whether the waveform is sent once or cycles endlessly.

NOTE: Any hardware PWM started by [*gpioHardwarePWM*] will be cancelled.

. .
  wave_id: >=0, as returned by [*gpioWaveCreate*]
wave_mode: 0 (PI_WAVE_MODE_ONE_SHOT), 1 (PI_WAVE_MODE_REPEAT)
. .

Returns the number of DMA control blocks in the waveform if OK,
otherwise PI_BAD_WAVE_ID, or PI_BAD_WAVE_MODE.
D*/


/*F*/
int gpioWaveChain(char *buf, unsigned bufSize);
/*D
This function transmits a chain of waveforms.

NOTE: Any hardware PWM started by [*gpioHardwarePWM*] will be cancelled.

The waves to be transmitted are specified by the contents of buf
which contains an ordered list of wave_ids and optional command
codes and related data.

. .
    buf: pointer to the wave_ids and optional command codes
bufSize: the number of bytes in buf
. .

Returns 0 if OK, otherwise PI_BAD_REPEAT_CNT, PI_BAD_REPEAT_WID,
PI_BAD_CHAIN_CMD, PI_TOO_MANY_COUNTS, or PI_BAD_WAVE_ID.

Each wave is transmitted in the order specified.  A wave may only
occur once per chain.  Waves may be transmitted multiple times by
using the repeat command.  The repeat command specifies a wave id
and a count.  The wave id must occur earlier in the chain.  All the
waves between wave id and the repeat command are transmitted count
times.

Repeat commands may not be nested.  The minimum repeat count is 2.
A maximum of 5 repeat commands is supported per chain.

The following command codes are supported:

Name    @ Cmd & Data       @ Meaning
Repeat  @ 255 wid C0 C1 C2 @ Repeat from wid count times
count = C0 + C1*256 + C2*65536

...
The following examples assume that waves with ids 0 to 12 exist.

// 0 255 0 57 0 0 (repeat 0 57 times)
status = gpioWaveChain((char []){0, 255, 0, 57, 0, 0}, 6);

// 0 1 255 0 0 2 0 (repeat 0+1 512 times)
status = gpioWaveChain((char []){0, 1, 255, 0, 0, 2, 0}, 7);

// 0 1 255 1 0 0 1 (transmit 0, repeat 1 65536 times)
status = gpioWaveChain((char []){0, 1, 255, 1, 0, 0, 1}, 7);

// 0 1 2 3 255 2 13 0 0 (transmit 0+1, repeat 2+3 13 times)
status = gpioWaveChain(
   (char []){0, 1, 2, 3, 255, 2, 13, 0, 0}, 9);

// The following repeats 5 65793 times, transmits 6,
// repeats 7+8 514 times, transmits 12,
// repeats 9+11+10 197121 times.
// 5 255 5 1 1 1 6 7 8 255 7 2 2 0 12 9 11 10 255 9 1 2 3
char chain[] = {
   5,             255, 5, 1, 1, 1,
   6, 7, 8,       255, 7, 2, 2, 0,
   12, 9, 11, 10, 255, 9, 1, 2, 3};

status = gpioWaveChain(chain, sizeof(chain));
...
D*/


/*F*/
int gpioWaveTxBusy(void);
/*D
This function checks to see if a waveform is currently being
transmitted.

Returns 1 if a waveform is currently being transmitted, otherwise 0.
D*/


/*F*/
int gpioWaveTxStop(void);
/*D
This function aborts the transmission of the current waveform.

Returns 0 if OK.

This function is intended to stop a waveform started in repeat mode.
D*/


/*F*/
int gpioWaveGetMicros(void);
/*D
This function returns the length in microseconds of the current
waveform.
D*/


/*F*/
int gpioWaveGetHighMicros(void);
/*D
This function returns the length in microseconds of the longest waveform
created since [*gpioInitialise*] was called.
D*/


/*F*/
int gpioWaveGetMaxMicros(void);
/*D
This function returns the maximum possible size of a waveform in
microseconds.
D*/


/*F*/
int gpioWaveGetPulses(void);
/*D
This function returns the length in pulses of the current waveform.
D*/


/*F*/
int gpioWaveGetHighPulses(void);
/*D
This function returns the length in pulses of the longest waveform
created since [*gpioInitialise*] was called.
D*/


/*F*/
int gpioWaveGetMaxPulses(void);
/*D
This function returns the maximum possible size of a waveform in pulses.
D*/


/*F*/
int gpioWaveGetCbs(void);
/*D
This function returns the length in DMA control blocks of the current
waveform.
D*/


/*F*/
int gpioWaveGetHighCbs(void);
/*D
This function returns the length in DMA control blocks of the longest
waveform created since [*gpioInitialise*] was called.
D*/


/*F*/
int gpioWaveGetMaxCbs(void);
/*D
This function returns the maximum possible size of a waveform in DMA
control blocks.
D*/


/*F*/
int gpioSerialReadOpen(unsigned user_gpio, unsigned baud, unsigned data_bits);
/*D
This function opens a gpio for bit bang reading of serial data.

. .
user_gpio: 0-31
     baud: 50-250000
data_bits: 1-32
. .

Returns 0 if OK, otherwise PI_BAD_USER_GPIO, PI_BAD_WAVE_BAUD,
PI_BAD_DATABITS, or PI_GPIO_IN_USE.

The serial data is returned in a cyclic buffer and is read using
[*gpioSerialRead*].

It is the caller's responsibility to read data from the cyclic buffer
in a timely fashion.
D*/


/*F*/
int gpioSerialRead(unsigned user_gpio, void *buf, size_t bufSize);
/*D
This function copies up to bufSize bytes of data read from the
bit bang serial cyclic buffer to the buffer starting at buf.

. .
user_gpio: 0-31, previously opened with [*gpioSerialReadOpen*]
      buf: an array to receive the read bytes
  bufSize: 0-
. .

Returns the number of bytes copied if OK, otherwise PI_BAD_USER_GPIO
or PI_NOT_SERIAL_GPIO.

The bytes returned for each character depend upon the number of
data bits [*data_bits*] specified in the [*gpioSerialReadOpen*] command.

For [*data_bits*] 1-8 there will be one byte per character. 
For [*data_bits*] 9-16 there will be two bytes per character. 
For [*data_bits*] 17-32 there will be four bytes per character.
D*/


/*F*/
int gpioSerialReadClose(unsigned user_gpio);
/*D
This function closes a gpio for bit bang reading of serial data.

. .
user_gpio: 0-31, previously opened with [*gpioSerialReadOpen*]
. .

Returns 0 if OK, otherwise PI_BAD_USER_GPIO, or PI_NOT_SERIAL_GPIO.
D*/

/*F*/
int i2cOpen(unsigned i2cBus, unsigned i2cAddr, unsigned i2cFlags);
/*D
This returns a handle for the device at the address on the I2C bus.

. .
  i2cBus: 0-1
 i2cAddr: 0x00-0x7F
i2cFlags: 0
. .

No flags are currently defined.  This parameter should be set to zero.

Returns a handle (>=0) if OK, otherwise PI_BAD_I2C_BUS, PI_BAD_I2C_ADDR,
PI_BAD_FLAGS, PI_NO_HANDLE, or PI_I2C_OPEN_FAILED.

For the SMBus commands the low level transactions are shown at the end
of the function description.  The following abbreviations are used.

. .
S     (1 bit) : Start bit
P     (1 bit) : Stop bit
Rd/Wr (1 bit) : Read/Write bit. Rd equals 1, Wr equals 0.
A, NA (1 bit) : Accept and not accept bit. 
Addr  (7 bits): I2C 7 bit address.
Comm  (8 bits): Command byte, a data byte which often selects a register.
Data  (8 bits): A data byte.
Count (8 bits): A data byte containing the length of a block operation.

[..]: Data sent by the device.
. .
D*/


/*F*/
int i2cClose(unsigned handle);
/*D
This closes the I2C device associated with the handle.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE.
D*/


/*F*/
int i2cWriteQuick(unsigned handle, unsigned bit);
/*D
This sends a single bit (in the Rd/Wr bit) to the device associated
with handle.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
   bit: 0-1, the value to write
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE, PI_BAD_PARAM, or
PI_I2C_WRITE_FAILED.

Quick command. SMBus 2.0 5.5.1
. .
S Addr Rd/Wr [A] P
. .
D*/


/*F*/
int i2cWriteByte(unsigned handle, unsigned bVal);
/*D
This sends a single byte to the device associated with handle.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
  bVal: 0-0xFF, the value to write
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE, PI_BAD_PARAM, or
PI_I2C_WRITE_FAILED.

Send byte. SMBus 2.0 5.5.2
. .
S Addr Wr [A] Data [A] P
. .
D*/


/*F*/
int i2cReadByte(unsigned handle);
/*D
This reads a single byte from the device associated with handle.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
. .

Returns the byte read (>=0) if OK, otherwise PI_BAD_HANDLE,
or PI_I2C_READ_FAILED.

Receive byte. SMBus 2.0 5.5.3
. .
S Addr Rd [A] [Data] NA P
. .
D*/


/*F*/
int i2cWriteByteData(unsigned handle, unsigned i2cReg, unsigned bVal);
/*D
This writes a single byte to the specified register of the device
associated with handle.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
i2cReg: 0-255, the register to write
  bVal: 0-0xFF, the value to write
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE, PI_BAD_PARAM, or
PI_I2C_WRITE_FAILED.

Write byte. SMBus 2.0 5.5.4
. .
S Addr Wr [A] Comm [A] Data [A] P
. .
D*/


/*F*/
int i2cWriteWordData(unsigned handle, unsigned i2cReg, unsigned wVal);
/*D
This writes a single 16 bit word to the specified register of the device
associated with handle.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
i2cReg: 0-255, the register to write
  wVal: 0-0xFFFF, the value to write
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE, PI_BAD_PARAM, or
PI_I2C_WRITE_FAILED.

Write word. SMBus 2.0 5.5.4
. .
S Addr Wr [A] Comm [A] DataLow [A] DataHigh [A] P
. .
D*/


/*F*/
int i2cReadByteData(unsigned handle, unsigned i2cReg);
/*D
This reads a single byte from the specified register of the device
associated with handle.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
i2cReg: 0-255, the register to read
. .

Returns the byte read (>=0) if OK, otherwise PI_BAD_HANDLE,
PI_BAD_PARAM, or PI_I2C_READ_FAILED.

Read byte. SMBus 2.0 5.5.5
. .
S Addr Wr [A] Comm [A] S Addr Rd [A] [Data] NA P
. .
D*/


/*F*/
int i2cReadWordData(unsigned handle, unsigned i2cReg);
/*D
This reads a single 16 bit word from the specified register of the device
associated with handle.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
i2cReg: 0-255, the register to read
. .

Returns the word read (>=0) if OK, otherwise PI_BAD_HANDLE,
PI_BAD_PARAM, or PI_I2C_READ_FAILED.

Read word. SMBus 2.0 5.5.5
. .
S Addr Wr [A] Comm [A] S Addr Rd [A] [DataLow] A [DataHigh] NA P
. .
D*/


/*F*/
int i2cProcessCall(unsigned handle, unsigned i2cReg, unsigned wVal);
/*D
This writes 16 bits of data to the specified register of the device
associated with handle and reads 16 bits of data in return.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
i2cReg: 0-255, the register to write/read
  wVal: 0-0xFFFF, the value to write
. .

Returns the word read (>=0) if OK, otherwise PI_BAD_HANDLE,
PI_BAD_PARAM, or PI_I2C_READ_FAILED.

Process call. SMBus 2.0 5.5.6
. .
S Addr Wr [A] Comm [A] DataLow [A] DataHigh [A]
   S Addr Rd [A] [DataLow] A [DataHigh] NA P
. .
D*/


/*F*/
int i2cWriteBlockData(
unsigned handle, unsigned i2cReg, char *buf, unsigned count);
/*D
This writes up to 32 bytes to the specified register of the device
associated with handle.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
i2cReg: 0-255, the register to write
   buf: an array with the data to send
 count: 1-32, the number of bytes to write
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE, PI_BAD_PARAM, or
PI_I2C_WRITE_FAILED.

Block write. SMBus 2.0 5.5.7
. .
S Addr Wr [A] Comm [A] Count [A] Data [A] Data [A] ... [A] Data [A] P
. .
D*/


/*F*/
int i2cReadBlockData(unsigned handle, unsigned i2cReg, char *buf);
/*D
This reads a block of up to 32 bytes from the specified register of
the device associated with handle.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
i2cReg: 0-255, the register to read
   buf: an array to receive the read data
. .

The amount of returned data is set by the device.

Returns the number of bytes read (>=0) if OK, otherwise PI_BAD_HANDLE,
PI_BAD_PARAM, or PI_I2C_READ_FAILED.

Block read. SMBus 2.0 5.5.7
. .
S Addr Wr [A] Comm [A]
   S Addr Rd [A] [Count] A [Data] A [Data] A ... A [Data] NA P
. .
D*/


/*F*/
int i2cBlockProcessCall(
unsigned handle, unsigned i2cReg, char *buf, unsigned count);
/*D
This writes data bytes to the specified register of the device
associated with handle and reads a device specified number
of bytes of data in return.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
i2cReg: 0-255, the register to write/read
   buf: an array with the data to send and to receive the read data
 count: 1-32, the number of bytes to write
. .

Returns the number of bytes read (>=0) if OK, otherwise PI_BAD_HANDLE,
PI_BAD_PARAM, or PI_I2C_READ_FAILED.

The SMBus 2.0 documentation states that a minimum of 1 byte may be
sent and a minimum of 1 byte may be received.  The total number of
bytes sent/received must be 32 or less.

Block write-block read. SMBus 2.0 5.5.8
. .
S Addr Wr [A] Comm [A] Count [A] Data [A] ...
   S Addr Rd [A] [Count] A [Data] ... A P
. .
D*/


/*F*/
int i2cReadI2CBlockData(
unsigned handle, unsigned i2cReg, char *buf, unsigned count);
/*D
This reads count bytes from the specified register of the device
associated with handle .  The count may be 1-32.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
i2cReg: 0-255, the register to read
   buf: an array to receive the read data
 count: 1-32, the number of bytes to read
. .

Returns the number of bytes read (>0) if OK, otherwise PI_BAD_HANDLE,
PI_BAD_PARAM, or PI_I2C_READ_FAILED.

. .
S Addr Wr [A] Comm [A]
   S Addr Rd [A] [Data] A [Data] A ... A [Data] NA P
. .
D*/


/*F*/
int i2cWriteI2CBlockData(
unsigned handle, unsigned i2cReg, char *buf, unsigned count);
/*D
This writes 1 to 32 bytes to the specified register of the device
associated with handle.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
i2cReg: 0-255, the register to write
   buf: the data to write
 count: 1-32, the number of bytes to write
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE, PI_BAD_PARAM, or
PI_I2C_WRITE_FAILED.

. .
S Addr Wr [A] Comm [A] Data [A] Data [A] ... [A] Data [A] P
. .
D*/

/*F*/
int i2cReadDevice(unsigned handle, char *buf, unsigned count);
/*D
This reads count bytes from the raw device into buf.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
   buf: an array to receive the read data bytes
 count: >0, the number of bytes to read
. .

Returns count (>0) if OK, otherwise PI_BAD_HANDLE, PI_BAD_PARAM, or
PI_I2C_READ_FAILED.
D*/


/*F*/
int i2cWriteDevice(unsigned handle, char *buf, unsigned count);
/*D
This writes count bytes from buf to the raw device.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
   buf: an array containing the data bytes to write
 count: >0, the number of bytes to write
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE, PI_BAD_PARAM, or
PI_I2C_WRITE_FAILED.
D*/

/*F*/
void i2cSwitchCombined(int setting);
/*D
This sets the I2C (i2c-bcm2708) module "use combined transactions"
parameter on or off.

. .
setting: 0 to set the parameter off, non-zero to set it on
. .


NOTE: when the flag is on a write followed by a read to the same
slave address will use a repeated start (rather than a stop/start).
D*/

/*F*/
int i2cSegments(unsigned handle, pi_i2c_msg_t *segs, unsigned numSegs);
/*D
This function executes multiple I2C segments in one transaction by
calling the I2C_RDWR ioctl.

. .
 handle: >=0, as returned by a call to [*i2cOpen*]
   segs: an array of I2C segments
numSegs: >0, the number of I2C segments
. .

Returns the number of segments if OK, otherwise PI_BAD_I2C_SEG.
D*/

/*F*/
int i2cZip(
   unsigned handle,
   char    *inBuf,
   unsigned inLen,
   char    *outBuf,
   unsigned outLen);
/*D
This function executes a sequence of I2C operations.  The
operations to be performed are specified by the contents of inBuf
which contains the concatenated command codes and associated data.

. .
handle: >=0, as returned by a call to [*i2cOpen*]
 inBuf: pointer to the concatenated I2C commands, see below
 inLen: size of command buffer
outBuf: pointer to buffer to hold returned data
outLen: size of output buffer
. .

Returns >= 0 if OK (the number of bytes read), otherwise
PI_BAD_HANDLE, PI_BAD_POINTER, PI_BAD_I2C_CMD, PI_BAD_I2C_RLEN.
PI_BAD_I2C_WLEN, or PI_BAD_I2C_SEG.

The following command codes are supported:

Name    @ Cmd & Data @ Meaning
End     @ 0          @ No more commands
Escape  @ 1          @ Next P is two bytes
On      @ 2          @ Switch combined flag on
Off     @ 3          @ Switch combined flag off
Address @ 4 P        @ Set I2C address to P
Flags   @ 5 lsb msb  @ Set I2C flags to lsb + (msb << 8)
Read    @ 6 P        @ Read P bytes of data
Write   @ 7 P ...    @ Write P bytes of data

The address, read, and write commands take a parameter P.
Normally P is one byte (0-255).  If the command is preceded by
the Escape command then P is two bytes (0-65535, least significant
byte first).

The address defaults to that associated with the handle.
The flags default to 0.  The address and flags maintain their
previous value until updated.

The returned I2C data is stored in consecutive locations of outBuf.

...
Set address 0x53, write 0x32, read 6 bytes
Set address 0x1E, write 0x03, read 6 bytes
Set address 0x68, write 0x1B, read 8 bytes
End

0x04 0x53   0x07 0x01 0x32   0x06 0x06
0x04 0x1E   0x07 0x01 0x03   0x06 0x06
0x04 0x68   0x07 0x01 0x1B   0x06 0x08
0x00
...
D*/

/*F*/
int bbI2COpen(unsigned SDA, unsigned SCL, unsigned baud);
/*D
This function selects a pair of gpios for bit banging I2C at a
specified baud rate.

Bit banging I2C allows for certain operations which are not possible
with the standard I2C driver.

o baud rates as low as 50 
o repeated starts 
o clock stretching 
o I2C on any pair of spare gpios

. .
 SDA: 0-31
 SCL: 0-31
baud: 50-500000
. .

Returns 0 if OK, otherwise PI_BAD_USER_GPIO, PI_BAD_I2C_BAUD, or
PI_GPIO_IN_USE.

NOTE:

The gpios used for SDA and SCL must have pull-ups to 3V3 connected.  As
a guide the hardware pull-ups on pins 3 and 5 are 1k8 in value.
D*/

/*F*/
int bbI2CClose(unsigned SDA);
/*D
This function stops bit banging I2C on a pair of gpios previously
opened with [*bbI2COpen*].

. .
SDA: 0-31, the SDA gpio used in a prior call to [*bbI2COpen*]
. .

Returns 0 if OK, otherwise PI_BAD_USER_GPIO, or PI_NOT_I2C_GPIO.
D*/

/*F*/
int bbI2CZip(
   unsigned SDA,
   char    *inBuf,
   unsigned inLen,
   char    *outBuf,
   unsigned outLen);
/*D
This function executes a sequence of bit banged I2C operations.  The
operations to be performed are specified by the contents of inBuf
which contains the concatenated command codes and associated data.

. .
   SDA: 0-31 (as used in a prior call to [*bbI2COpen*])
 inBuf: pointer to the concatenated I2C commands, see below
 inLen: size of command buffer
outBuf: pointer to buffer to hold returned data
outLen: size of output buffer
. .

Returns >= 0 if OK (the number of bytes read), otherwise
PI_BAD_USER_GPIO, PI_NOT_I2C_GPIO, PI_BAD_POINTER,
PI_BAD_I2C_CMD, PI_BAD_I2C_RLEN, PI_BAD_I2C_WLEN,
PI_I2C_READ_FAILED, or PI_I2C_WRITE_FAILED.

The following command codes are supported:

Name    @ Cmd & Data   @ Meaning
End     @ 0            @ No more commands
Escape  @ 1            @ Next P is two bytes
Start   @ 2            @ Start condition
Stop    @ 3            @ Stop condition
Address @ 4 P          @ Set I2C address to P
Flags   @ 5 lsb msb    @ Set I2C flags to lsb + (msb << 8)
Read    @ 6 P          @ Read P bytes of data
Write   @ 7 P ...      @ Write P bytes of data

The address, read, and write commands take a parameter P.
Normally P is one byte (0-255).  If the command is preceded by
the Escape command then P is two bytes (0-65535, least significant
byte first).

The address and flags default to 0.  The address and flags maintain
their previous value until updated.

No flags are currently defined.

The returned I2C data is stored in consecutive locations of outBuf.

...
Set address 0x53
start, write 0x32, (re)start, read 6 bytes, stop
Set address 0x1E
start, write 0x03, (re)start, read 6 bytes, stop
Set address 0x68
start, write 0x1B, (re)start, read 8 bytes, stop
End

0x04 0x53
0x02 0x07 0x01 0x32   0x02 0x06 0x06 0x03

0x04 0x1E
0x02 0x07 0x01 0x03   0x02 0x06 0x06 0x03

0x04 0x68
0x02 0x07 0x01 0x1B   0x02 0x06 0x08 0x03

0x00
...
D*/

/*F*/
int spiOpen(unsigned spiChan, unsigned baud, unsigned spiFlags);
/*D
This function returns a handle for the SPI device on the channel.
Data will be transferred at baud bits per second.  The flags may
be used to modify the default behaviour of 4-wire operation, mode 0,
active low chip select.

An auxiliary SPI device is available on the A+/B+/Pi2 and may be
selected by setting the A bit in the flags.  The auxiliary
device has 3 chip selects and a selectable word size in bits.

. .
 spiChan: 0-1 (0-2 for A+/B+/Pi2 auxiliary device)
    baud: 32K-125M (values above 30M are unlikely to work)
spiFlags: see below
. .

Returns a handle (>=0) if OK, otherwise PI_BAD_SPI_CHANNEL,
PI_BAD_SPI_SPEED, PI_BAD_FLAGS, PI_NO_AUX_SPI, or PI_SPI_OPEN_FAILED.

spiFlags consists of the least significant 22 bits.

. .
21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 b  b  b  b  b  b  R  T  n  n  n  n  W  A u2 u1 u0 p2 p1 p0  m  m
. .

mm defines the SPI mode.

Warning: modes 1 and 3 do not appear to work on the auxiliary device.

. .
Mode POL PHA
 0    0   0
 1    0   1
 2    1   0
 3    1   1
. .

px is 0 if CEx is active low (default) and 1 for active high.

ux is 0 if the CEx gpio is reserved for SPI (default) and 1 otherwise.

A is 0 for the standard SPI device, 1 for the auxiliary SPI.  The
auxiliary device is only present on the A+/B+/Pi2.

W is 0 if the device is not 3-wire, 1 if the device is 3-wire.  Standard
SPI device only.

nnnn defines the number of bytes (0-15) to write before switching
the MOSI line to MISO to read data.  This field is ignored
if W is not set.  Standard SPI device only.

T is 1 if the least significant bit is transmitted on MOSI first, the
default (0) shifts the most significant bit out first.  Auxiliary SPI
device only.

R is 1 if the least significant bit is received on MISO first, the
default (0) receives the most significant bit first.  Auxiliary SPI
device only.

bbbbbb defines the word size in bits (0-32).  The default (0)
sets 8 bits per word.  Auxiliary SPI device only.

The other bits in flags should be set to zero.
D*/

/*F*/
int spiClose(unsigned handle);
/*D
This functions closes the SPI device identified by the handle.

. .
handle: >=0, as returned by a call to [*spiOpen*]
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE.
D*/


/*F*/
int spiRead(unsigned handle, char *buf, unsigned count);
/*D
This function reads count bytes of data from the SPI
device associated with the handle.

. .
handle: >=0, as returned by a call to [*spiOpen*]
   buf: an array to receive the read data bytes
 count: the number of bytes to read
. .

Returns the number of bytes transferred if OK, otherwise
PI_BAD_HANDLE, PI_BAD_SPI_COUNT, or PI_SPI_XFER_FAILED.
D*/


/*F*/
int spiWrite(unsigned handle, char *buf, unsigned count);
/*D
This function writes count bytes of data from buf to the SPI
device associated with the handle.

. .
handle: >=0, as returned by a call to [*spiOpen*]
   buf: the data bytes to write
 count: the number of bytes to write
. .

Returns the number of bytes transferred if OK, otherwise
PI_BAD_HANDLE, PI_BAD_SPI_COUNT, or PI_SPI_XFER_FAILED.
D*/

/*F*/
int spiXfer(unsigned handle, char *txBuf, char *rxBuf, unsigned count);
/*D
This function transfers count bytes of data from txBuf to the SPI
device associated with the handle.  Simultaneously count bytes of
data are read from the device and placed in rxBuf.

. .
handle: >=0, as returned by a call to [*spiOpen*]
 txBuf: the data bytes to write
 rxBuf: the received data bytes
 count: the number of bytes to transfer
. .

Returns the number of bytes transferred if OK, otherwise
PI_BAD_HANDLE, PI_BAD_SPI_COUNT, or PI_SPI_XFER_FAILED.
D*/


/*F*/
int serOpen(char *sertty, unsigned baud, unsigned serFlags);
/*D
This function opens a serial device at a specified baud rate
with specified flags.

. .
  sertty: the serial device to open, /dev/tty*
    baud: the baud rate in bits per second, see below
serFlags: 0
. .

Returns a handle (>=0) if OK, otherwise PI_NO_HANDLE, or
PI_SER_OPEN_FAILED.

The baud rate must be one of 50, 75, 110, 134, 150,
200, 300, 600, 1200, 1800, 2400, 4800, 9500, 19200,
38400, 57600, 115200, or 230400.

No flags are currently defined.  This parameter should be set to zero.
D*/


/*F*/
int serClose(unsigned handle);
/*D
This function closes the serial device associated with handle.

. .
handle: >=0, as returned by a call to [*serOpen*]
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE.
D*/

/*F*/
int serWriteByte(unsigned handle, unsigned bVal);
/*D
This function writes bVal to the serial port associated with handle.

. .
handle: >=0, as returned by a call to [*serOpen*]
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE, PI_BAD_PARAM, or
PI_SER_WRITE_FAILED.
D*/

/*F*/
int serReadByte(unsigned handle);
/*D
This function reads a byte from the serial port associated with handle.

. .
handle: >=0, as returned by a call to [*serOpen*]
. .

Returns the read byte (>=0) if OK, otherwise PI_BAD_HANDLE,
PI_SER_READ_NO_DATA, or PI_SER_READ_FAILED.
D*/

/*F*/
int serWrite(unsigned handle, char *buf, unsigned count);
/*D
This function writes count bytes from buf to the the serial port
associated with handle.

. .
handle: >=0, as returned by a call to [*serOpen*]
   buf: the array of bytes to write
 count: the number of bytes to write
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE, PI_BAD_PARAM, or
PI_SER_WRITE_FAILED.
D*/


/*F*/
int serRead(unsigned handle, char *buf, unsigned count);
/*D
This function reads up count bytes from the the serial port
associated with handle and writes them to buf.

. .
handle: >=0, as returned by a call to serial_open
   buf: an array to receive the read data
 count: the maximum number of bytes to read
. .

Returns the number of bytes read (>0) if OK, otherwise PI_BAD_HANDLE,
PI_BAD_PARAM, PI_SER_READ_NO_DATA, or PI_SER_WRITE_FAILED.
D*/


/*F*/
int serDataAvailable(unsigned handle);
/*D
This function returns the number of bytes available
to be read from the device associated with handle.

. .
handle: >=0, as returned by a call to [*serOpen*]
. .

Returns the number of bytes of data available (>=0) if OK,
otherwise PI_BAD_HANDLE.
D*/


/*F*/
int gpioTrigger(unsigned user_gpio, unsigned pulseLen, unsigned level);
/*D
This function sends a trigger pulse to a gpio.  The gpio is set to
level for pulseLen microseconds and then reset to not level.

. .
user_gpio: 0-31
 pulseLen: 1-100
    level: 0,1
. .

Returns 0 if OK, otherwise PI_BAD_USER_GPIO, PI_BAD_LEVEL,
or PI_BAD_PULSELEN.
D*/


/*F*/
int gpioSetWatchdog(unsigned user_gpio, unsigned timeout);
/*D
Sets a watchdog for a gpio.

. .
user_gpio: 0-31
  timeout: 0-60000
. .

Returns 0 if OK, otherwise PI_BAD_USER_GPIO or PI_BAD_WDOG_TIMEOUT.

The watchdog is nominally in milliseconds.

One watchdog may be registered per gpio.

The watchdog may be cancelled by setting timeout to 0.

If no level change has been detected for the gpio for timeout
milliseconds:-

1) any registered alert function for the gpio is called with
   the level set to PI_TIMEOUT. 
2) any notification for the gpio has a report written to the
   fifo with the flags set to indicate a watchdog timeout.

...
void aFunction(int gpio, int level, uint32_t tick)
{
   printf("gpio %d became %d at %d\n", gpio, level, tick);
}

// call aFunction whenever gpio 4 changes state
gpioSetAlertFunc(4, aFunction);

//  or approximately every 5 millis
gpioSetWatchdog(4, 5);
...
D*/


/*F*/
int gpioSetGetSamplesFunc(gpioGetSamplesFunc_t f, uint32_t bits);
/*D
Registers a function to be called (a callback) every millisecond
with the latest gpio samples.

. .
   f: the function to call
bits: the gpios of interest
. .

Returns 0 if OK.

The function is passed a pointer to the samples (an array of
[*gpioSample_t*]),  and the number of samples.

Only one function can be registered.

The callback may be cancelled by passing NULL as the function.

The samples returned will be the union of bits, plus any active alerts,
plus any active notifications.

e.g.  if there are alerts for gpios 7, 8, and 9, notifications for gpios
8, 10, 23, 24, and bits is (1<<23)|(1<<17) then samples for gpios
7, 8, 9, 10, 17, 23, and 24 will be reported.
D*/


/*F*/
int gpioSetGetSamplesFuncEx(
   gpioGetSamplesFuncEx_t f, uint32_t bits, void *userdata);
/*D
Registers a function to be called (a callback) every millisecond
with the latest gpio samples.

. .
       f: the function to call
    bits: the gpios of interest
userdata: a pointer to arbitrary user data
. .

Returns 0 if OK.

The function is passed a pointer to the samples (an array of
[*gpioSample_t*]), the number of samples, and the userdata pointer.

Only one of [*gpioGetSamplesFunc*] or [*gpioGetSamplesFuncEx*] can be
registered.

See [*gpioSetGetSamplesFunc*] for further details.
D*/


/*F*/
int gpioSetTimerFunc(unsigned timer, unsigned millis, gpioTimerFunc_t f);
/*D
Registers a function to be called (a callback) every millis milliseconds.

. .
 timer: 0-9
millis: 10-60000
     f: the function to call
. .

Returns 0 if OK, otherwise PI_BAD_TIMER, PI_BAD_MS, or PI_TIMER_FAILED.

10 timers are supported numbered 0 to 9.

One function may be registered per timer.

The timer may be cancelled by passing NULL as the function.

...
void bFunction(void)
{
   printf("two seconds have elapsed\n");
}

// call bFunction every 2000 milliseconds
gpioSetTimerFunc(0, 2000, bFunction);
...
D*/


/*F*/
int gpioSetTimerFuncEx(
   unsigned timer, unsigned millis, gpioTimerFuncEx_t f, void *userdata);
/*D
Registers a function to be called (a callback) every millis milliseconds.

. .
   timer: 0-9.
  millis: 10-60000
       f: the function to call
userdata: a pointer to arbitrary user data
. .

Returns 0 if OK, otherwise PI_BAD_TIMER, PI_BAD_MS, or PI_TIMER_FAILED.

The function is passed the userdata pointer.

Only one of [*gpioSetTimerFunc*] or [*gpioSetTimerFuncEx*] can be
registered per timer.

See [*gpioSetTimerFunc*] for further details.
D*/


/*F*/
pthread_t *gpioStartThread(gpioThreadFunc_t f, void *arg);
/*D
Starts a new thread of execution with f as the main routine.

. .
  f: the main function for the new thread
arg: a pointer to arbitrary user data
. .

Returns a pointer to pthread_t if OK, otherwise NULL.

The function is passed the single argument arg.

The thread can be cancelled by passing the pointer to pthread_t to
[*gpioStopThread*].

...
#include <stdio.h>
#include <pigpio.h>

void *myfunc(void *arg)
{
   while (1)
   {
      printf("%s\n", arg);
      sleep(1);
   }
}

int main(int argc, char *argv[])
{
   pthread_t *p1, *p2, *p3;

   if (gpioInitialise() < 0) return 1;

   p1 = gpioStartThread(myfunc, "thread 1"); sleep(3);

   p2 = gpioStartThread(myfunc, "thread 2"); sleep(3);

   p3 = gpioStartThread(myfunc, "thread 3"); sleep(3);

   gpioStopThread(p3); sleep(3);

   gpioStopThread(p2); sleep(3);

   gpioStopThread(p1); sleep(3);

   gpioTerminate();
}
...
D*/


/*F*/
void gpioStopThread(pthread_t *pth);
/*D
Cancels the thread pointed at by pth.

. .
pth: a thread pointer returned by [*gpioStartThread*]
. .

No value is returned.

The thread to be stopped should have been started with [*gpioStartThread*].
D*/


/*F*/
int gpioStoreScript(char *script);
/*D
This function stores a null terminated script for later execution.

. .
script: the text of the script
. .

The function returns a script id if the script is valid,
otherwise PI_BAD_SCRIPT.
D*/


/*F*/
int gpioRunScript(unsigned script_id, unsigned numPar, uint32_t *param);
/*D
This function runs a stored script.

. .
script_id: >=0, as returned by [*gpioStoreScript*]
   numPar: 0-10, the number of parameters
    param: an array of parameters
. .

The function returns 0 if OK, otherwise PI_BAD_SCRIPT_ID, or
PI_TOO_MANY_PARAM.

param is an array of up to 10 parameters which may be referenced in
the script as p0 to p9.
D*/


/*F*/
int gpioScriptStatus(unsigned script_id, uint32_t *param);
/*D
This function returns the run status of a stored script as well as
the current values of parameters 0 to 9.

. .
script_id: >=0, as returned by [*gpioStoreScript*]
    param: an array to hold the returned 10 parameters
. .

The function returns greater than or equal to 0 if OK,
otherwise PI_BAD_SCRIPT_ID.

The run status may be

. .
PI_SCRIPT_INITING
PI_SCRIPT_HALTED
PI_SCRIPT_RUNNING
PI_SCRIPT_WAITING
PI_SCRIPT_FAILED
. .

The current value of script parameters 0 to 9 are returned in param.
D*/


/*F*/
int gpioStopScript(unsigned script_id);
/*D
This function stops a running script.

. .
script_id: >=0, as returned by [*gpioStoreScript*]
. .

The function returns 0 if OK, otherwise PI_BAD_SCRIPT_ID.
D*/


/*F*/
int gpioDeleteScript(unsigned script_id);
/*D
This function deletes a stored script.

. .
script_id: >=0, as returned by [*gpioStoreScript*]
. .

The function returns 0 if OK, otherwise PI_BAD_SCRIPT_ID.
D*/


/*F*/
int gpioSetSignalFunc(unsigned signum, gpioSignalFunc_t f);
/*D
Registers a function to be called (a callback) when a signal occurs.

. .
signum: 0-63
     f: the callback function
. .

Returns 0 if OK, otherwise PI_BAD_SIGNUM.

The function is passed the signal number.

One function may be registered per signal.

The callback may be cancelled by passing NULL.

By default all signals are treated as fatal and cause the library
to call gpioTerminate and then exit.
D*/


/*F*/
int gpioSetSignalFuncEx(
   unsigned signum, gpioSignalFuncEx_t f, void *userdata);
/*D
Registers a function to be called (a callback) when a signal occurs.

. .
  signum: 0-63
       f: the callback function
userdata: a pointer to arbitrary user data
. .

Returns 0 if OK, otherwise PI_BAD_SIGNUM.

The function is passed the signal number and the userdata pointer.

Only one of gpioSetSignalFunc or gpioSetSignalFuncEx can be
registered per signal.

See gpioSetSignalFunc for further details.
D*/


/*F*/
uint32_t gpioRead_Bits_0_31(void);
/*D
Returns the current level of gpios 0-31.
D*/


/*F*/
uint32_t gpioRead_Bits_32_53(void);
/*D
Returns the current level of gpios 32-53.
D*/


/*F*/
int gpioWrite_Bits_0_31_Clear(uint32_t bits);
/*D
Clears gpios 0-31 if the corresponding bit in bits is set.

. .
bits: a bit mask of gpios to clear
. .

Returns 0 if OK.

...
// To clear (set to 0) gpios 4, 7, and 15
gpioWrite_Bits_0_31_Clear( (1<<4) | (1<<7) | (1<<15) );
...
D*/


/*F*/
int gpioWrite_Bits_32_53_Clear(uint32_t bits);
/*D
Clears gpios 32-53 if the corresponding bit (0-21) in bits is set.

. .
bits: a bit mask of gpios to clear
. .

Returns 0 if OK.
D*/


/*F*/
int gpioWrite_Bits_0_31_Set(uint32_t bits);
/*D
Sets gpios 0-31 if the corresponding bit in bits is set.

. .
bits: a bit mask of gpios to set
. .

Returns 0 if OK.
D*/


/*F*/
int gpioWrite_Bits_32_53_Set(uint32_t bits);
/*D
Sets gpios 32-53 if the corresponding bit (0-21) in bits is set.

. .
bits: a bit mask of gpios to set
. .

Returns 0 if OK.

...
// To set (set to 1) gpios 32, 40, and 53
gpioWrite_Bits_32_53_Set((1<<(32-32)) | (1<<(40-32)) | (1<<(53-32)));
...
D*/

/*F*/
int gpioHardwareClock(unsigned gpio, unsigned clkfreq);
/*D
Starts a hardware clock on a gpio at the specified frequency.
Frequencies above 30MHz are unlikely to work.

. .
   gpio: see description
clkfreq: 0 (off) or 4689-250000000 (250M)
. .

Returns 0 if OK, otherwise PI_BAD_GPIO, PI_NOT_HCLK_GPIO,
PI_BAD_HCLK_FREQ,or PI_BAD_HCLK_PASS.

The same clock is available on multiple gpios.  The latest
frequency setting will be used by all gpios which share a clock.

The gpio must be one of the following.

. .
4   clock 0  All models
5   clock 1  A+/B+/Pi2 and compute module only (reserved for system use)
6   clock 2  A+/B+/Pi2 and compute module only
20  clock 0  A+/B+/Pi2 and compute module only
21  clock 1  All models but Rev.2 B (reserved for system use)

32  clock 0  Compute module only
34  clock 0  Compute module only
42  clock 1  Compute module only (reserved for system use)
43  clock 2  Compute module only
44  clock 1  Compute module only (reserved for system use)
. .

Access to clock 1 is protected by a password as its use will likely
crash the Pi.  The password is given by or'ing 0x5A000000 with the
gpio number.
D*/

/*F*/
int gpioHardwarePWM(unsigned gpio, unsigned PWMfreq, unsigned PWMduty);
/*D
Starts hardware PWM on a gpio at the specified frequency and dutycycle.
Frequencies above 30MHz are unlikely to work.

NOTE: Any waveform started by [*gpioWaveTxSend*], [*gpioWaveTxStart*],
or [*gpioWaveChain*] will be cancelled.

This function is only valid if the pigpio main clock is PCM.  The
main clock defaults to PCM but may be overridden by a call to
[*gpioCfgClock*].

. .
   gpio: see description
PWMfreq: 0 (off) or 1-125000000 (125M)
PWMduty: 0 (off) to 1000000 (1M)(fully on)
. .

Returns 0 if OK, otherwise PI_BAD_GPIO, PI_NOT_HPWM_GPIO,
PI_BAD_HPWM_DUTY, PI_BAD_HPWM_FREQ, or PI_HPWM_ILLEGAL.

The same PWM channel is available on multiple gpios.  The latest
frequency and dutycycle setting will be used by all gpios which
share a PWM channel.

The gpio must be one of the following.

. .
12  PWM channel 0  A+/B+/Pi2 and compute module only
13  PWM channel 1  A+/B+/Pi2 and compute module only
18  PWM channel 0  All models
19  PWM channel 1  A+/B+/Pi2 and compute module only

40  PWM channel 0  Compute module only
41  PWM channel 1  Compute module only
45  PWM channel 1  Compute module only
52  PWM channel 0  Compute module only
53  PWM channel 1  Compute module only
. .
D*/

/*F*/
int gpioTime(unsigned timetype, int *seconds, int *micros);
/*D
Updates the seconds and micros variables with the current time.

. .
timetype: 0 (relative), 1 (absolute)
 seconds: a pointer to an int to hold seconds
  micros: a pointer to an int to hold microseconds
. .

Returns 0 if OK, otherwise PI_BAD_TIMETYPE.

If timetype is PI_TIME_ABSOLUTE updates seconds and micros with the
number of seconds and microseconds since the epoch (1st January 1970).

If timetype is PI_TIME_RELATIVE updates seconds and micros with the
number of seconds and microseconds since the library was initialised.

...
int secs, mics;

// print the number of seconds since the library was started
gpioTime(PI_TIME_RELATIVE, &secs, &mics);
printf("library started %d.%03d seconds ago\n", secs, mics/1000);
...
D*/


/*F*/
int gpioSleep(unsigned timetype, int seconds, int micros);
/*D
Sleeps for the number of seconds and microseconds specified by seconds
and micros.

. .
timetype: 0 (relative), 1 (absolute)
 seconds: seconds to sleep
  micros: microseconds to sleep
. .

Returns 0 if OK, otherwise PI_BAD_TIMETYPE, PI_BAD_SECONDS,
or PI_BAD_MICROS.

If timetype is PI_TIME_ABSOLUTE the sleep ends when the number of seconds
and microseconds since the epoch (1st January 1970) has elapsed.  System
clock changes are taken into account.

If timetype is PI_TIME_RELATIVE the sleep is for the specified number
of seconds and microseconds.  System clock changes do not effect the
sleep length.

For short delays (say, 50 microseonds or less) use [*gpioDelay*].

...
gpioSleep(PI_TIME_RELATIVE, 2, 500000); // sleep for 2.5 seconds

gpioSleep(PI_TIME_RELATIVE, 0, 100000); // sleep for 0.1 seconds

gpioSleep(PI_TIME_RELATIVE, 60, 0);     // sleep for one minute
...
D*/


/*F*/
uint32_t gpioDelay(uint32_t micros);
/*D
Delays for at least the number of microseconds specified by micros.

. .
micros: the number of microseconds to sleep
. .

Returns the actual length of the delay in microseconds.

Delays of 100 microseconds or less use busy waits.
D*/


/*F*/
uint32_t gpioTick(void);
/*D
Returns the current system tick.

Tick is the number of microseconds since system boot.

As tick is an unsigned 32 bit quantity it wraps around after
2^32 microseconds, which is approximately 1 hour 12 minutes.

You don't need to worry about the wrap around as long as you
take a tick (uint32_t) from another tick, i.e. the following
code will always provide the correct difference.

...
uint32_t startTick, endTick;
int diffTick;

startTick = gpioTick();

// do some processing

endTick = gpioTick();

diffTick = endTick - startTick;

printf("some processing took %d microseconds\n", diffTick);
...
D*/


/*F*/
unsigned gpioHardwareRevision(void);
/*D
Returns the hardware revision.

If the hardware revision can not be found or is not a valid hexadecimal
number the function returns 0.

The hardware revision is the last few characters on the Revision line of
/proc/cpuinfo.

The revision number can be used to determine the assignment of gpios
to pins (see [*gpio*]).

There are at least three types of board.

Type 1 boards have hardware revision numbers of 2 and 3.

Type 2 boards have hardware revision numbers of 4, 5, 6, and 15.

Type 3 boards have hardware revision numbers of 16 or greater.

for "Revision       : 0002" the function returns 2. 
for "Revision       : 000f" the function returns 15. 
for "Revision       : 000g" the function returns 0.
D*/


/*F*/
unsigned gpioVersion(void);
/*D
Returns the pigpio version.
D*/


/*F*/
int gpioCfgBufferSize(unsigned cfgMillis);
/*D
Configures pigpio to buffer cfgMillis milliseconds of gpio samples.

. .
cfgMillis: 100-10000
. .

The default setting is 120 milliseconds.

The intention is to allow for bursts of data and protection against
other processes hogging cpu time.

I haven't seen a process locked out for more than 100 milliseconds.

Making the buffer bigger uses a LOT of memory at the more frequent
sampling rates as shown in the following table in MBs.

. .
                     buffer milliseconds
               120 250 500 1sec 2sec 4sec 8sec

         1      16  31  55  107  ---  ---  ---
         2      10  18  31   55  107  ---  ---
sample   4       8  12  18   31   55  107  ---
 rate    5       8  10  14   24   45   87  ---
 (us)    8       6   8  12   18   31   55  107
        10       6   8  10   14   24   45   87
. .
D*/


/*F*/
int gpioCfgClock(
   unsigned cfgMicros, unsigned cfgPeripheral, unsigned cfgSource);
/*D
Configures pigpio to use a particular sample rate timed by a specified
peripheral.

. .
    cfgMicros: 1, 2, 4, 5, 8, 10
cfgPeripheral: 0 (PWM), 1 (PCM)
    cfgSource: deprecated, value is ignored
. .

The timings are provided by the specified peripheral (PWM or PCM).

The default setting is 5 microseconds using the PCM peripheral.

The approximate CPU percentage used for each sample rate is:

. .
sample  cpu
 rate    %

  1     25
  2     16
  4     11
  5     10
  8     15
 10     14
. .

A sample rate of 5 microseconds seeems to be the sweet spot.
D*/


/*F*/
int gpioCfgDMAchannel(unsigned DMAchannel); /* DEPRECATED */
/*D
Configures pigpio to use the specified DMA channel.

. .
DMAchannel: 0-14
. .

The default setting is to use channel 14.
D*/


/*F*/
int gpioCfgDMAchannels(
   unsigned primaryChannel, unsigned secondaryChannel);
/*D
Configures pigpio to use the specified DMA channels.

. .
  primaryChannel: 0-14
secondaryChannel: 0-6
. .

The default setting is to use channel 14 for the primary channel and
channel 5 for the secondary channel.
D*/


/*F*/
int gpioCfgPermissions(uint64_t updateMask);
/*D
Configures pigpio to only allow updates (writes or mode changes) for the
gpios specified by the mask.

. .
updateMask: bit (1<<n) is set for each gpio n which may be updated
. .

The default setting depends upon the board revision (Type 1, 2, or 3).
The user gpios are added to the mask.  If the board revision is not
recognised then gpios 0-31 are allowed.

Unknown board @ PI_DEFAULT_UPDATE_MASK_R0 @ 0xFFFFFFFF 
Type 1 board  @ PI_DEFAULT_UPDATE_MASK_R1 @ 0x03E6CF93 
Type 2 board  @ PI_DEFAULT_UPDATE_MASK_R2 @ 0xFBC6CF9C
Type 3 board  @ PI_DEFAULT_UPDATE_MASK_R3 @ 0x0FFFFFFC
D*/


/*F*/
int gpioCfgSocketPort(unsigned port);
/*D
Configures pigpio to use the specified socket port.

. .
port: 1024-32000
. .

The default setting is to use port 8888.
D*/


/*F*/
int gpioCfgInterfaces(unsigned ifFlags);
/*D
Configures pigpio support of the fifo and socket interfaces.

. .
ifFlags: 0-3
. .

The default setting (0) is that both interfaces are enabled.

Or in PI_DISABLE_FIFO_IF to disable the pipe interface.
Or in PI_DISABLE_SOCK_IF to disable the socket interface.
D*/


/*F*/
int gpioCfgMemAlloc(unsigned memAllocMode);
/*D
Selects the method of DMA memory allocation.

. .
memAllocMode: 0-2
. .

There are two methods of DMA memory allocation.  The original method
uses the /proc/self/pagemap file to allocate bus memory.  The new
method uses the mailbox property interface to allocate bus memory.

Auto will use the mailbox method unless a larger than default buffer
size is requested with [*gpioCfgBufferSize*].
D*/

/*F*/
int gpioCfgInternals(unsigned cfgWhat, int cfgVal);
/*D
Used to tune internal settings.

. .
cfgWhat: see source code
 cfgVal: see source code
. .
D*/

/*F*/
int gpioCustom1(unsigned arg1, unsigned arg2, char *argx, unsigned argc);
/*D
This function is available for user customisation.

It returns a single integer value.

. .
arg1: >=0
arg2: >=0
argx: extra (byte) arguments
argc: number of extra arguments
. .

Returns >= 0 if OK, less than 0 indicates a user defined error.
D*/


/*F*/
int gpioCustom2(unsigned arg1, char *argx, unsigned argc,
                char *retBuf, unsigned retMax);
/*D
This function is available for user customisation.

It differs from gpioCustom1 in that it returns an array of bytes
rather than just an integer.

The returned value is an integer indicating the number of returned bytes.
. .
  arg1: >=0
  argx: extra (byte) arguments
  argc: number of extra arguments
retBuf: buffer for returned bytes
retMax: maximum number of bytes to return
. .

Returns >= 0 if OK, less than 0 indicates a user defined error.

The number of returned bytes must be retMax or less.
D*/


/*F*/
int rawWaveAddSPI(
   rawSPI_t *spi,
   unsigned offset,
   unsigned spiSS,
   char *buf,
   unsigned spiTxBits,
   unsigned spiBitFirst,
   unsigned spiBitLast,
   unsigned spiBits);
/*D
This function adds a waveform representing SPI data to the
existing waveform (if any).

. .
        spi: a pointer to a spi object
     offset: microseconds from the start of the waveform
      spiSS: the slave select gpio
        buf: the bits to transmit, most significant bit first
  spiTxBits: the number of bits to write
spiBitFirst: the first bit to read
 spiBitLast: the last bit to read
    spiBits: the number of bits to transfer
. .

Returns the new total number of pulses in the current waveform if OK,
otherwise PI_BAD_USER_GPIO, PI_BAD_SER_OFFSET, or PI_TOO_MANY_PULSES.

Not intended for general use.
D*/

/*F*/
int rawWaveAddGeneric(unsigned numPulses, rawWave_t *pulses);
/*D
This function adds a number of pulses to the current waveform.

. .
numPulses: the number of pulses
   pulses: the array containing the pulses
. .

Returns the new total number of pulses in the current waveform if OK,
otherwise PI_TOO_MANY_PULSES.

The advantage of this function over gpioWaveAddGeneric is that it
allows the setting of the flags field.

The pulses are interleaved in time order within the existing waveform
(if any).

Merging allows the waveform to be built in parts, that is the settings
for gpio#1 can be added, and then gpio#2 etc.

If the added waveform is intended to start after or within the existing
waveform then the first pulse should consist of a delay.

Not intended for general use.
D*/

/*F*/
unsigned rawWaveCB(void);
/*D
Returns the number of the cb being currently output.

Not intended for general use.
D*/

/*F*/
rawCbs_t *rawWaveCBAdr(int cbNum);
/*D
Return the Linux address of contol block cbNum.

. .
cbNum: the cb of interest
. .

Not intended for general use.
D*/

/*F*/
uint32_t rawWaveGetOut(int pos);
/*D
Gets the wave output parameter stored at pos.

. .
pos: the position of interest.
. .

Not intended for general use.
D*/


/*F*/
void rawWaveSetOut(int pos, uint32_t lVal);
/*D
Sets the wave output parameter stored at pos to value.

. .
 pos: the position of interest
lVal: the value to write
. .

Not intended for general use.
D*/

/*F*/
uint32_t rawWaveGetIn(int pos);
/*D
Gets the wave input value parameter stored at pos.

. .
pos: the position of interest
. .

Not intended for general use.
D*/


/*F*/
void rawWaveSetIn(int pos, uint32_t lVal);
/*D
Sets the wave input value stored at pos to value.

. .
 pos: the position of interest
lVal: the value to write
. .

Not intended for general use.
D*/

/*F*/
rawWaveInfo_t rawWaveInfo(int wave_id);
/*D
Gets details about the wave with id wave_id.

. .
wave_id: the wave of interest
. .

Not intended for general use.
D*/

/*F*/
int getBitInBytes(int bitPos, char *buf, int numBits);
/*D
Returns the value of the bit bitPos bits from the start of buf.  Returns
0 if bitPos is greater than or equal to numBits.

. .
 bitPos: bit index from the start of buf
    buf: array of bits
numBits: number of valid bits in buf
. .

D*/

/*F*/
void putBitInBytes(int bitPos, char *buf, int bit);
/*D
Sets the bit bitPos bits from the start of buf to bit.

. .
bitPos: bit index from the start of buf
   buf: array of bits
   bit: 0-1, value to set
. .

D*/

/*F*/
double time_time(void);
/*D
Return the current time in seconds since the Epoch.
D*/


/*F*/
void time_sleep(double seconds);
/*D
Delay execution for a given number of seconds

. .
seconds: the number of seconds to sleep
. .
D*/


/*F*/
void rawDumpWave(void);
/*D
Used to print a readable version of the current waveform to stderr.

Not intended for general use.
D*/


/*F*/
void rawDumpScript(unsigned script_id);
/*D
Used to print a readable version of a script to stderr.

. .
script_id: >=0, a script_id returned by [*gpioStoreScript*]
. .

Not intended for general use.
D*/


#ifdef __cplusplus
}
#endif

/*PARAMS

*arg::

A pointer to a void object passed to a thread started by gpioStartThread.

arg1::

An unsigned argument passed to a user customised function.  Its
meaning is defined by the customiser.

arg2::

An unsigned argument passed to a user customised function.  Its
meaning is defined by the customiser.

argc::
The count of bytes passed to a user customised function.

*argx::
A pointer to an array of bytes passed to a user customised function.
Its meaning and content is defined by the customiser.

baud::
The speed of serial communication (I2C, SPI, serial link, waves) in
bits per second.

bit::
A value of 0 or 1.

bitPos::
A bit position within a byte or word.  The least significant bit is
position 0.

bits::
A value used to select gpios.  If bit n of bits is set then gpio n is
selected.

A convenient way to set bit n is to or in (1<<n).

e.g. to select bits 5, 9, 23 you could use (1<<5) | (1<<9) | (1<<23).

*buf::

A buffer to hold data being sent or being received.

bufSize::

The size in bytes of a buffer.

bVal::0-255 (Hex 0x0-0xFF, Octal 0-0377)

An 8-bit byte value.

cbNum::

A number identifying a DMA contol block.

cfgMicros::

The gpio sample rate in microseconds.  The default is 5us, or 200 thousand
samples per second.

cfgMillis:: 100-10000

The size of the sample buffer in milliseconds.  Generally this should be
left at the default of 120ms.  If you expect intense bursts of signals it
might be necessary to increase the buffer size.

cfgPeripheral::

One of the PWM or PCM peripherals used to pace DMA transfers for timing
purposes.

cfgSource::

Deprecated.

cfgVal::

A number specifying the value of a configuration item.  See [*cfgWhat*].

cfgWhat::

A number specifying a configuration item.

562484977: print enhanced statistics at termination. 
984762879: set the initial debug level.

char::

A single character, an 8 bit quantity able to store 0-255.

clkfreq::4689-250M

The hardware clock frequency.

. .
#define PI_HW_CLK_MIN_FREQ 4689
#define PI_HW_CLK_MAX_FREQ 250000000
. .

count::

The number of bytes to be transferred in an I2C, SPI, or Serial
command.

data_bits::1-32

The number of data bits to be used when adding serial data to a
waveform.

. .
#define PI_MIN_WAVE_DATABITS 1
#define PI_MAX_WAVE_DATABITS 32
. .

DMAchannel::0-14
. .
PI_MIN_DMA_CHANNEL 0
PI_MAX_DMA_CHANNEL 14
. .

double::

A floating point number.

dutycycle::0-range

A number representing the ratio of on time to off time for PWM.

The number may vary between 0 and range (default 255) where
0 is off and range is fully on.

f::

A function.

frequency::0-

The number of times a gpio is swiched on and off per second.  This
can be set per gpio and may be as little as 5Hz or as much as
40KHz.  The gpio will be on for a proportion of the time as defined
by its dutycycle.

gpio::

A Broadcom numbered gpio, in the range 0-53.

There  are 54 General Purpose Input Outputs (gpios) named gpio0 through
gpio53.

They are split into two  banks.   Bank  1  consists  of  gpio0  through
gpio31.  Bank 2 consists of gpio32 through gpio53.

All the gpios which are safe for the user to read and write are in
bank 1.  Not all gpios in bank 1 are safe though.  Type 1 boards
have 17  safe gpios.  Type 2 boards have 21.  Type 3 boards have 26.

See [*gpioHardwareRevision*].

The user gpios are marked with an X in the following table.

. .
          0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
Type 1    X  X  -  -  X  -  -  X  X  X  X  X  -  -  X  X
Type 2    -  -  X  X  X  -  -  X  X  X  X  X  -  -  X  X
Type 3          X  X  X  X  X  X  X  X  X  X  X  X  X  X

         16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
Type 1    -  X  X  -  -  X  X  X  X  X  -  -  -  -  -  -
Type 2    -  X  X  -  -  -  X  X  X  X  -  X  X  X  X  X
Type 3    X  X  X  X  X  X  X  X  X  X  X  X  -  -  -  -
. .

gpioAlertFunc_t::
. .
typedef void (*gpioAlertFunc_t) (int gpio, int level, uint32_t tick);
. .

gpioAlertFuncEx_t::
. .
typedef void (*gpioAlertFuncEx_t)
   (int gpio, int level, uint32_t tick, void *userdata);
. .

gpioCfg*::

One of

[*gpioCfgBufferSize*] 
[*gpioCfgClock*] 
[*gpioCfgDMAchannel*] 
[*gpioCfgDMAchannels*] 
[*gpioCfgPermissions*] 
[*gpioCfgInterfaces*] 
[*gpioCfgInternals*] 
[*gpioCfgSocketPort*] 
[*gpioCfgMemAlloc*]

gpioGetSamplesFunc_t::
. .
typedef void (*gpioGetSamplesFunc_t)
   (const gpioSample_t *samples, int numSamples);
. .

gpioGetSamplesFuncEx_t::
. .
typedef void (*gpioGetSamplesFuncEx_t)
   (const gpioSample_t *samples, int numSamples, void *userdata);
. .

gpioPulse_t::
. .
typedef struct
{
   uint32_t gpioOn;
   uint32_t gpioOff;
   uint32_t usDelay;
} gpioPulse_t;
. .

gpioSample_t::
. .
typedef struct
{
   uint32_t tick;
   uint32_t level;
} gpioSample_t;
. .

gpioSignalFunc_t::
. .
typedef void (*gpioSignalFunc_t) (int signum);
. .

gpioSignalFuncEx_t::
. .
typedef void (*gpioSignalFuncEx_t) (int signum, void *userdata);
. .

gpioThreadFunc_t::
. .
typedef void *(gpioThreadFunc_t) (void *);
. .

gpioTimerFunc_t::
. .
typedef void (*gpioTimerFunc_t) (void);
. .

gpioTimerFuncEx_t::
. .
typedef void (*gpioTimerFuncEx_t) (void *userdata);
. .

gpioWaveAdd*::

One of

[*gpioWaveAddNew*] 
[*gpioWaveAddGeneric*] 
[*gpioWaveAddSerial*]

handle::0-

A number referencing an object opened by one of

[*i2cOpen*] 
[*gpioNotifyOpen*] 
[*serOpen*] 
[*spiOpen*]

i2cAddr::
The address of a device on the I2C bus.

i2cBus::0-1

An I2C bus, 0 or 1.

i2cFlags::0

Flags which modify an I2C open command.  None are currently defined.

i2cReg:: 0-255

A register of an I2C device.

ifFlags::0-3
. .
PI_DISABLE_FIFO_IF 1
PI_DISABLE_SOCK_IF 2
. .

*inBuf::
A buffer used to pass data to a function.

inLen::
The number of bytes of data in a buffer.

int::
A whole number, negative or positive.

level::
The level of a gpio.  Low or High.

. .
PI_OFF 0
PI_ON 1

PI_CLEAR 0
PI_SET 1

PI_LOW 0
PI_HIGH 1
. .

There is one exception.  If a watchdog expires on a gpio the level will be
reported as PI_TIMEOUT.  See [*gpioSetWatchdog*].

. .
PI_TIMEOUT 2
. .


lVal::0-4294967295 (Hex 0x0-0xFFFFFFFF, Octal 0-37777777777)

A 32-bit word value.

memAllocMode:: 0-2

The DMA memory allocation mode.

. .
PI_MEM_ALLOC_AUTO    0
PI_MEM_ALLOC_PAGEMAP 1
PI_MEM_ALLOC_MAILBOX 2
. .

*micros::

A value representing microseconds.

micros::

A value representing microseconds.

millis::

A value representing milliseconds.

mode::0-7

The operational mode of a gpio, normally INPUT or OUTPUT.

. .
PI_INPUT 0
PI_OUTPUT 1
PI_ALT0 4
PI_ALT1 5
PI_ALT2 6
PI_ALT3 7
PI_ALT4 3
PI_ALT5 2
. .

numBits::

The number of bits stored in a buffer.

numBytes::
The number of bytes used to store characters in a string.  Depending
on the number of bits per character there may be 1, 2, or 4 bytes
per character.

numPar:: 0-10
The number of parameters passed to a script.

numPulses::
The number of pulses to be added to a waveform.

numSegs::
The number of segments in a combined I2C transaction.

offset::
The associated data starts this number of microseconds from the start of
the waveform.

*outBuf::
A buffer used to return data from a function.

outLen::
The size in bytes of an output buffer.

*param::
An array of script parameters.

pi_i2c_msg_t::
. .
typedef struct
{
   uint16_t addr;  // slave address
   uint16_t flags;
   uint16_t len;   // msg length
   uint8_t  *buf;  // pointer to msg data
} pi_i2c_msg_t;
. .

port:: 1024-32000
The port used to bind to the pigpio socket.  Defaults to 8888.

pos::
The position of an item.

primaryChannel:: 0-14
The DMA channel used to time the sampling of gpios and to time servo and
PWM pulses.

*pth::

A thread identifier, returned by [*gpioStartThread*].

pthread_t::

A thread identifier.

pud::0-2

The setting of the pull up/down resistor for a gpio, which may be off,
pull-up, or pull-down.
. .
PI_PUD_OFF 0
PI_PUD_DOWN 1
PI_PUD_UP 2
. .

pulseLen::

1-100, the length of a trigger pulse in microseconds.

*pulses::

An array of pulses to be added to a waveform.

pulsewidth::0, 500-2500
. .
PI_SERVO_OFF 0
PI_MIN_SERVO_PULSEWIDTH 500
PI_MAX_SERVO_PULSEWIDTH 2500
. .

PWMduty::0-1000000 (1M)
The hardware PWM dutycycle.

. .
#define PI_HW_PWM_RANGE 1000000
. .

PWMfreq::5-250K
The hardware PWM frequency.

. .
#define PI_HW_PWM_MIN_FREQ 1
#define PI_HW_PWM_MAX_FREQ 125000000
. .

range::25-40000
. .
PI_MIN_DUTYCYCLE_RANGE 25
PI_MAX_DUTYCYCLE_RANGE 40000
. .

rawCbs_t::
. .
typedef struct // linux/arch/arm/mach-bcm2708/include/mach/dma.h
{
   unsigned long info;
   unsigned long src;
   unsigned long dst;
   unsigned long length;
   unsigned long stride;
   unsigned long next;
   unsigned long pad[2];
} rawCbs_t;
. .

rawSPI_t::
. .
typedef struct
{
   int clk;     // gpio for clock
   int mosi;    // gpio for MOSI
   int miso;    // gpio for MISO
   int ss_pol;  // slave select off state
   int ss_us;   // delay after slave select
   int clk_pol; // clock off state
   int clk_pha; // clock phase
   int clk_us;  // clock micros
} rawSPI_t;
. .

rawWave_t::
. .
typedef struct
{
   uint32_t gpioOn;
   uint32_t gpioOff;
   uint32_t usDelay;
   uint32_t flags;
} rawWave_t;
. .

rawWaveInfo_t::
. .
typedef struct
{
   uint16_t botCB;  // first CB used by wave
   uint16_t topCB;  // last CB used by wave
   uint16_t botOOL; // last OOL used by wave
   uint16_t topOOL; // first OOL used by wave
} rawWaveInfo_t;
. .

*retBuf::

A buffer to hold a number of bytes returned to a used customised function,

retMax::

The maximum number of bytes a user customised function should return.

*rxBuf::

A pointer to a buffer to receive data.

SCL::

The user gpio to use for the clock when bit banging I2C.

*script::

A pointer to the text of a script.

script_id::

An id of a stored script as returned by [*gpioStoreScript*].

SDA::

The user gpio to use for data when bit banging I2C.

secondaryChannel:: 0-6

The DMA channel used to time output waveforms.

*seconds::

A pointer to a uint32_t to store the second component of
a returned time.

seconds::

The number of seconds.

*segs::

An array of segments which make up a combined I2C transaction.

serFlags::
Flags which modify a serial open command.  None are currently defined.

*sertty::
The name of a serial tty device, e.g. /dev/ttyAMA0, /dev/ttyUSB0, /dev/tty1.

setting::
A value used to set a flag, 0 for false, non-zero for true.

signum::0-63
. .
PI_MIN_SIGNUM 0
PI_MAX_SIGNUM 63
. .

size_t::

A standard type used to indicate the size of an object in bytes.

*spi::
A pointer to a [*rawSPI_t*] structure.

spiBitFirst::
Gpio reads are made from spiBitFirst to spiBitLast.

spiBitLast::

Gpio reads are made from spiBitFirst to spiBitLast.

spiBits::
The number of bits to transfer in a raw SPI transaction.

spiChan::
A SPI channel, 0-2.

spiFlags::
See [*spiOpen*].

spiSS::
The SPI slave select gpio in a raw SPI transaction.

spiTxBits::
The number of bits to transfer dring a raw SPI transaction

stop_bits::2-8
The number of (half) stop bits to be used when adding serial data
to a waveform.

. .
#define PI_MIN_WAVE_HALFSTOPBITS 2
#define PI_MAX_WAVE_HALFSTOPBITS 8
. .

*str::
An array of characters.

timeout::
A gpio watchdog timeout in milliseconds.
. .
PI_MIN_WDOG_TIMEOUT 0
PI_MAX_WDOG_TIMEOUT 60000
. .

timer::
. .
PI_MIN_TIMER 0
PI_MAX_TIMER 9
. .

timetype::
. .
PI_TIME_RELATIVE 0
PI_TIME_ABSOLUTE 1
. .

*txBuf::

An array of bytes to transmit.

uint32_t::0-0-4,294,967,295 (Hex 0x0-0xFFFFFFFF)

A 32-bit unsigned value.

uint64_t::0-(2^64)-1

A 64-bit unsigned value.

unsigned::

A whole number >= 0.

updateMask::

A 64 bit mask indicating which gpios may be written to by the user.

If gpio#n may be written then bit (1<<n) is set.

user_gpio::

0-31, a Broadcom numbered gpio.

See [*gpio*].

*userdata::

A pointer to arbitrary user data.  This may be used to identify the instance.

void::

Denoting no parameter is required

wave_id::

A number representing a waveform created by [*gpioWaveCreate*].

wave_mode::

The mode of waveform transmission, whether it is sent once or cycles
repeatedly.

. .
PI_WAVE_MODE_ONE_SHOT 0
PI_WAVE_MODE_REPEAT   1
. .

wVal::0-65535 (Hex 0x0-0xFFFF, Octal 0-0177777)

A 16-bit word value.

PARAMS*/

/*DEF_S Socket Command Codes*/

#define PI_CMD_MODES  0
#define PI_CMD_MODEG  1
#define PI_CMD_PUD    2
#define PI_CMD_READ   3
#define PI_CMD_WRITE  4
#define PI_CMD_PWM    5
#define PI_CMD_PRS    6
#define PI_CMD_PFS    7
#define PI_CMD_SERVO  8
#define PI_CMD_WDOG   9
#define PI_CMD_BR1   10
#define PI_CMD_BR2   11
#define PI_CMD_BC1   12
#define PI_CMD_BC2   13
#define PI_CMD_BS1   14
#define PI_CMD_BS2   15
#define PI_CMD_TICK  16
#define PI_CMD_HWVER 17
#define PI_CMD_NO    18
#define PI_CMD_NB    19
#define PI_CMD_NP    20
#define PI_CMD_NC    21
#define PI_CMD_PRG   22
#define PI_CMD_PFG   23
#define PI_CMD_PRRG  24
#define PI_CMD_HELP  25
#define PI_CMD_PIGPV 26
#define PI_CMD_WVCLR 27
#define PI_CMD_WVAG  28
#define PI_CMD_WVAS  29
#define PI_CMD_WVGO  30
#define PI_CMD_WVGOR 31
#define PI_CMD_WVBSY 32
#define PI_CMD_WVHLT 33
#define PI_CMD_WVSM  34
#define PI_CMD_WVSP  35
#define PI_CMD_WVSC  36
#define PI_CMD_TRIG  37
#define PI_CMD_PROC  38
#define PI_CMD_PROCD 39
#define PI_CMD_PROCR 40
#define PI_CMD_PROCS 41
#define PI_CMD_SLRO  42
#define PI_CMD_SLR   43
#define PI_CMD_SLRC  44
#define PI_CMD_PROCP 45
#define PI_CMD_MICS  46
#define PI_CMD_MILS  47
#define PI_CMD_PARSE 48
#define PI_CMD_WVCRE 49
#define PI_CMD_WVDEL 50
#define PI_CMD_WVTX  51
#define PI_CMD_WVTXR 52
#define PI_CMD_WVNEW 53

#define PI_CMD_I2CO  54
#define PI_CMD_I2CC  55
#define PI_CMD_I2CRD 56
#define PI_CMD_I2CWD 57
#define PI_CMD_I2CWQ 58
#define PI_CMD_I2CRS 59
#define PI_CMD_I2CWS 60
#define PI_CMD_I2CRB 61
#define PI_CMD_I2CWB 62
#define PI_CMD_I2CRW 63
#define PI_CMD_I2CWW 64
#define PI_CMD_I2CRK 65
#define PI_CMD_I2CWK 66
#define PI_CMD_I2CRI 67
#define PI_CMD_I2CWI 68
#define PI_CMD_I2CPC 69
#define PI_CMD_I2CPK 70

#define PI_CMD_SPIO  71
#define PI_CMD_SPIC  72
#define PI_CMD_SPIR  73
#define PI_CMD_SPIW  74
#define PI_CMD_SPIX  75

#define PI_CMD_SERO  76
#define PI_CMD_SERC  77
#define PI_CMD_SERRB 78
#define PI_CMD_SERWB 79
#define PI_CMD_SERR  80
#define PI_CMD_SERW  81
#define PI_CMD_SERDA 82

#define PI_CMD_GDC   83
#define PI_CMD_GPW   84

#define PI_CMD_HC    85
#define PI_CMD_HP    86

#define PI_CMD_CF1   87
#define PI_CMD_CF2   88

#define PI_CMD_BI2CC 89
#define PI_CMD_BI2CO 90
#define PI_CMD_BI2CZ 91

#define PI_CMD_I2CZ  92

#define PI_CMD_WVCHA 93

#define PI_CMD_NOIB  99

/*DEF_E*/

/*
PI CMD_NOIB only works on the socket interface.
It returns a spare notification handle.  Notifications for
that handle will be sent to the socket (rather than a
/dev/pigpiox pipe).

The socket should be dedicated to receiving notifications
after this command is issued.
*/

/* pseudo commands */

#define PI_CMD_SCRIPT 800

#define PI_CMD_ADD   800
#define PI_CMD_AND   801
#define PI_CMD_CALL  802
#define PI_CMD_CMDR  803
#define PI_CMD_CMDW  804
#define PI_CMD_CMP   805
#define PI_CMD_DCR   806
#define PI_CMD_DCRA  807
#define PI_CMD_DIV   808
#define PI_CMD_HALT  809
#define PI_CMD_INR   810
#define PI_CMD_INRA  811
#define PI_CMD_JM    812
#define PI_CMD_JMP   813
#define PI_CMD_JNZ   814
#define PI_CMD_JP    815
#define PI_CMD_JZ    816
#define PI_CMD_TAG   817
#define PI_CMD_LD    818
#define PI_CMD_LDA   819
#define PI_CMD_LDAB  820
#define PI_CMD_MLT   821
#define PI_CMD_MOD   822
#define PI_CMD_NOP   823
#define PI_CMD_OR    824
#define PI_CMD_POP   825
#define PI_CMD_POPA  826
#define PI_CMD_PUSH  827
#define PI_CMD_PUSHA 828
#define PI_CMD_RET   829
#define PI_CMD_RL    830
#define PI_CMD_RLA   831
#define PI_CMD_RR    832
#define PI_CMD_RRA   833
#define PI_CMD_STA   834
#define PI_CMD_STAB  835
#define PI_CMD_SUB   836
#define PI_CMD_SYS   837
#define PI_CMD_WAIT  838
#define PI_CMD_X     839
#define PI_CMD_XA    840
#define PI_CMD_XOR   841

/*DEF_S Error Codes*/

#define PI_INIT_FAILED       -1 // gpioInitialise failed
#define PI_BAD_USER_GPIO     -2 // gpio not 0-31
#define PI_BAD_GPIO          -3 // gpio not 0-53
#define PI_BAD_MODE          -4 // mode not 0-7
#define PI_BAD_LEVEL         -5 // level not 0-1
#define PI_BAD_PUD           -6 // pud not 0-2
#define PI_BAD_PULSEWIDTH    -7 // pulsewidth not 0 or 500-2500
#define PI_BAD_DUTYCYCLE     -8 // dutycycle outside set range
#define PI_BAD_TIMER         -9 // timer not 0-9
#define PI_BAD_MS           -10 // ms not 10-60000
#define PI_BAD_TIMETYPE     -11 // timetype not 0-1
#define PI_BAD_SECONDS      -12 // seconds < 0
#define PI_BAD_MICROS       -13 // micros not 0-999999
#define PI_TIMER_FAILED     -14 // gpioSetTimerFunc failed
#define PI_BAD_WDOG_TIMEOUT -15 // timeout not 0-60000
#define PI_NO_ALERT_FUNC    -16 // DEPRECATED
#define PI_BAD_CLK_PERIPH   -17 // clock peripheral not 0-1
#define PI_BAD_CLK_SOURCE   -18 // DEPRECATED
#define PI_BAD_CLK_MICROS   -19 // clock micros not 1, 2, 4, 5, 8, or 10
#define PI_BAD_BUF_MILLIS   -20 // buf millis not 100-10000
#define PI_BAD_DUTYRANGE    -21 // dutycycle range not 25-40000
#define PI_BAD_DUTY_RANGE   -21 // DEPRECATED (use PI_BAD_DUTYRANGE)
#define PI_BAD_SIGNUM       -22 // signum not 0-63
#define PI_BAD_PATHNAME     -23 // can't open pathname
#define PI_NO_HANDLE        -24 // no handle available
#define PI_BAD_HANDLE       -25 // unknown handle
#define PI_BAD_IF_FLAGS     -26 // ifFlags > 3
#define PI_BAD_CHANNEL      -27 // DMA channel not 0-14
#define PI_BAD_PRIM_CHANNEL -27 // DMA primary channel not 0-14
#define PI_BAD_SOCKET_PORT  -28 // socket port not 1024-32000
#define PI_BAD_FIFO_COMMAND -29 // unrecognized fifo command
#define PI_BAD_SECO_CHANNEL -30 // DMA secondary channel not 0-6
#define PI_NOT_INITIALISED  -31 // function called before gpioInitialise
#define PI_INITIALISED      -32 // function called after gpioInitialise
#define PI_BAD_WAVE_MODE    -33 // waveform mode not 0-1
#define PI_BAD_CFG_INTERNAL -34 // bad parameter in gpioCfgInternals call
#define PI_BAD_WAVE_BAUD    -35 // baud rate not 50-250K(RX)/50-1M(TX)
#define PI_TOO_MANY_PULSES  -36 // waveform has too many pulses
#define PI_TOO_MANY_CHARS   -37 // waveform has too many chars
#define PI_NOT_SERIAL_GPIO  -38 // no bit bang serial read in progress on gpio
#define PI_BAD_SERIAL_STRUC -39 // bad (null) serial structure parameter
#define PI_BAD_SERIAL_BUF   -40 // bad (null) serial buf parameter
#define PI_NOT_PERMITTED    -41 // gpio operation not permitted
#define PI_SOME_PERMITTED   -42 // one or more gpios not permitted
#define PI_BAD_WVSC_COMMND  -43 // bad WVSC subcommand
#define PI_BAD_WVSM_COMMND  -44 // bad WVSM subcommand
#define PI_BAD_WVSP_COMMND  -45 // bad WVSP subcommand
#define PI_BAD_PULSELEN     -46 // trigger pulse length not 1-100
#define PI_BAD_SCRIPT       -47 // invalid script
#define PI_BAD_SCRIPT_ID    -48 // unknown script id
#define PI_BAD_SER_OFFSET   -49 // add serial data offset > 30 minutes
#define PI_GPIO_IN_USE      -50 // gpio already in use
#define PI_BAD_SERIAL_COUNT -51 // must read at least a byte at a time
#define PI_BAD_PARAM_NUM    -52 // script parameter id not 0-9
#define PI_DUP_TAG          -53 // script has duplicate tag
#define PI_TOO_MANY_TAGS    -54 // script has too many tags
#define PI_BAD_SCRIPT_CMD   -55 // illegal script command
#define PI_BAD_VAR_NUM      -56 // script variable id not 0-149
#define PI_NO_SCRIPT_ROOM   -57 // no more room for scripts
#define PI_NO_MEMORY        -58 // can't allocate temporary memory
#define PI_SOCK_READ_FAILED -59 // socket read failed
#define PI_SOCK_WRIT_FAILED -60 // socket write failed
#define PI_TOO_MANY_PARAM   -61 // too many script parameters (> 10)
#define PI_NOT_HALTED       -62 // script already running or failed
#define PI_BAD_TAG          -63 // script has unresolved tag
#define PI_BAD_MICS_DELAY   -64 // bad MICS delay (too large)
#define PI_BAD_MILS_DELAY   -65 // bad MILS delay (too large)
#define PI_BAD_WAVE_ID      -66 // non existent wave id
#define PI_TOO_MANY_CBS     -67 // No more CBs for waveform
#define PI_TOO_MANY_OOL     -68 // No more OOL for waveform
#define PI_EMPTY_WAVEFORM   -69 // attempt to create an empty waveform
#define PI_NO_WAVEFORM_ID   -70 // no more waveforms
#define PI_I2C_OPEN_FAILED  -71 // can't open I2C device
#define PI_SER_OPEN_FAILED  -72 // can't open serial device
#define PI_SPI_OPEN_FAILED  -73 // can't open SPI device
#define PI_BAD_I2C_BUS      -74 // bad I2C bus
#define PI_BAD_I2C_ADDR     -75 // bad I2C address
#define PI_BAD_SPI_CHANNEL  -76 // bad SPI channel
#define PI_BAD_FLAGS        -77 // bad i2c/spi/ser open flags
#define PI_BAD_SPI_SPEED    -78 // bad SPI speed
#define PI_BAD_SER_DEVICE   -79 // bad serial device name
#define PI_BAD_SER_SPEED    -80 // bad serial baud rate
#define PI_BAD_PARAM        -81 // bad i2c/spi/ser parameter
#define PI_I2C_WRITE_FAILED -82 // i2c write failed
#define PI_I2C_READ_FAILED  -83 // i2c read failed
#define PI_BAD_SPI_COUNT    -84 // bad SPI count
#define PI_SER_WRITE_FAILED -85 // ser write failed
#define PI_SER_READ_FAILED  -86 // ser read failed
#define PI_SER_READ_NO_DATA -87 // ser read no data available
#define PI_UNKNOWN_COMMAND  -88 // unknown command
#define PI_SPI_XFER_FAILED  -89 // spi xfer/read/write failed
#define PI_BAD_POINTER      -90 // bad (NULL) pointer
#define PI_NO_AUX_SPI       -91 // need a A+/B+/Pi2 for auxiliary SPI
#define PI_NOT_PWM_GPIO     -92 // gpio is not in use for PWM
#define PI_NOT_SERVO_GPIO   -93 // gpio is not in use for servo pulses
#define PI_NOT_HCLK_GPIO    -94 // gpio has no hardware clock
#define PI_NOT_HPWM_GPIO    -95 // gpio has no hardware PWM
#define PI_BAD_HPWM_FREQ    -96 // hardware PWM frequency not 1-125M
#define PI_BAD_HPWM_DUTY    -97 // hardware PWM dutycycle not 0-1M
#define PI_BAD_HCLK_FREQ    -98 // hardware clock frequency not 4689-250M
#define PI_BAD_HCLK_PASS    -99 // need password to use hardware clock 1
#define PI_HPWM_ILLEGAL    -100 // illegal, PWM in use for main clock
#define PI_BAD_DATABITS    -101 // serial data bits not 1-32
#define PI_BAD_STOPBITS    -102 // serial (half) stop bits not 2-8
#define PI_MSG_TOOBIG      -103 // socket/pipe message too big
#define PI_BAD_MALLOC_MODE -104 // bad memory allocation mode
#define PI_TOO_MANY_SEGS   -105 // too many I2C transaction segments
#define PI_BAD_I2C_SEG     -106 // an I2C transaction segment failed
#define PI_BAD_SMBUS_CMD   -107 // SMBus command not supported by driver
#define PI_NOT_I2C_GPIO    -108 // no bit bang I2C in progress on gpio
#define PI_BAD_I2C_WLEN    -109 // bad I2C write length
#define PI_BAD_I2C_RLEN    -110 // bad I2C read length
#define PI_BAD_I2C_CMD     -111 // bad I2C command
#define PI_BAD_I2C_BAUD    -112 // bad I2C baud rate, not 50-500k
#define PI_BAD_REPEAT_CNT  -113 // bad repeat count, not 2-max
#define PI_BAD_REPEAT_WID  -114 // bad repeat wave id
#define PI_TOO_MANY_COUNTS -115 // too many chain counters
#define PI_BAD_CHAIN_CMD   -116 // malformed chain command string
#define PI_REUSED_WID      -117 // wave already used in chain

#define PI_PIGIF_ERR_0    -2000
#define PI_PIGIF_ERR_99   -2099

#define PI_CUSTOM_ERR_0   -3000
#define PI_CUSTOM_ERR_999 -3999

/*DEF_E*/

/*DEF_S Defaults*/

#define PI_DEFAULT_BUFFER_MILLIS         120
#define PI_DEFAULT_CLK_MICROS            5
#define PI_DEFAULT_CLK_PERIPHERAL        PI_CLOCK_PCM
#define PI_DEFAULT_IF_FLAGS              0
#define PI_DEFAULT_DMA_CHANNEL           14
#define PI_DEFAULT_DMA_PRIMARY_CHANNEL   14
#define PI_DEFAULT_DMA_SECONDARY_CHANNEL 5
#define PI_DEFAULT_SOCKET_PORT           8888
#define PI_DEFAULT_SOCKET_PORT_STR       "8888"
#define PI_DEFAULT_SOCKET_ADDR_STR       "127.0.0.1"
#define PI_DEFAULT_UPDATE_MASK_R0        0xFFFFFFFF
#define PI_DEFAULT_UPDATE_MASK_R1        0x03E7CF93
#define PI_DEFAULT_UPDATE_MASK_R2        0xFBC7CF9C
#define PI_DEFAULT_UPDATE_MASK_R3        0x0080480FFFFFFCLL
#define PI_DEFAULT_UPDATE_MASK_COMPUTE   0x00FFFFFFFFFFFFLL
#define PI_DEFAULT_MEM_ALLOC_MODE        PI_MEM_ALLOC_AUTO

/*DEF_E*/

#endif

