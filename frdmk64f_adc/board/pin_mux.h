/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
    kPIN_MUX_DirectionInput = 0U,        /* Input direction */
    kPIN_MUX_DirectionOutput = 1U,       /* Output direction */
    kPIN_MUX_DirectionInputOrOutput = 2U /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

#define SOPT5_UART0TXSRC_UART_TX 0x00u /*!<@brief UART 0 transmit data source select: UART0_TX pin */

/*! @name PORTB16 (number 62), U7[4]/UART0_RX
  @{ */
#define BOARD_INITPINS_DEBUG_UART_RX_PERIPHERAL UART0       /*!<@brief Device name: UART0 */
#define BOARD_INITPINS_DEBUG_UART_RX_SIGNAL RX              /*!<@brief UART0 signal: RX */
#define BOARD_INITPINS_DEBUG_UART_RX_PORT PORTB             /*!<@brief PORT device name: PORTB */
#define BOARD_INITPINS_DEBUG_UART_RX_PIN 16U                /*!<@brief PORTB pin index: 16 */
#define BOARD_INITPINS_DEBUG_UART_RX_PIN_NAME UART0_RX      /*!<@brief Pin name */
#define BOARD_INITPINS_DEBUG_UART_RX_LABEL "U7[4]/UART0_RX" /*!<@brief Label */
#define BOARD_INITPINS_DEBUG_UART_RX_NAME "DEBUG_UART_RX"   /*!<@brief Identifier name */
                                                            /* @} */

/*! @name PORTB17 (number 63), U10[1]/UART0_TX
  @{ */
#define BOARD_INITPINS_DEBUG_UART_TX_PERIPHERAL UART0        /*!<@brief Device name: UART0 */
#define BOARD_INITPINS_DEBUG_UART_TX_SIGNAL TX               /*!<@brief UART0 signal: TX */
#define BOARD_INITPINS_DEBUG_UART_TX_PORT PORTB              /*!<@brief PORT device name: PORTB */
#define BOARD_INITPINS_DEBUG_UART_TX_PIN 17U                 /*!<@brief PORTB pin index: 17 */
#define BOARD_INITPINS_DEBUG_UART_TX_PIN_NAME UART0_TX       /*!<@brief Pin name */
#define BOARD_INITPINS_DEBUG_UART_TX_LABEL "U10[1]/UART0_TX" /*!<@brief Label */
#define BOARD_INITPINS_DEBUG_UART_TX_NAME "DEBUG_UART_TX"    /*!<@brief Identifier name */
                                                             /* @} */

/*! @name PORTE26 (number 33), J2[1]/D12[4]/LEDRGB_GREEN
  @{ */
#define BOARD_INITPINS_LED_GREEN_PERIPHERAL GPIOE                  /*!<@brief Device name: GPIOE */
#define BOARD_INITPINS_LED_GREEN_SIGNAL GPIO                       /*!<@brief GPIOE signal: GPIO */
#define BOARD_INITPINS_LED_GREEN_GPIO GPIOE                        /*!<@brief GPIO device name: GPIOE */
#define BOARD_INITPINS_LED_GREEN_GPIO_PIN 26U                      /*!<@brief PORTE pin index: 26 */
#define BOARD_INITPINS_LED_GREEN_PORT PORTE                        /*!<@brief PORT device name: PORTE */
#define BOARD_INITPINS_LED_GREEN_PIN 26U                           /*!<@brief PORTE pin index: 26 */
#define BOARD_INITPINS_LED_GREEN_CHANNEL 26                        /*!<@brief GPIOE GPIO channel: 26 */
#define BOARD_INITPINS_LED_GREEN_PIN_NAME PTE26                    /*!<@brief Pin name */
#define BOARD_INITPINS_LED_GREEN_LABEL "J2[1]/D12[4]/LEDRGB_GREEN" /*!<@brief Label */
#define BOARD_INITPINS_LED_GREEN_NAME "LED_GREEN"                  /*!<@brief Identifier name */
                                                                   /* @} */

/*! @name PORTB21 (number 67), D12[3]/LEDRGB_BLUE
  @{ */
#define BOARD_INITPINS_LED_BLUE_PERIPHERAL GPIOB           /*!<@brief Device name: GPIOB */
#define BOARD_INITPINS_LED_BLUE_SIGNAL GPIO                /*!<@brief GPIOB signal: GPIO */
#define BOARD_INITPINS_LED_BLUE_GPIO GPIOB                 /*!<@brief GPIO device name: GPIOB */
#define BOARD_INITPINS_LED_BLUE_GPIO_PIN 21U               /*!<@brief PORTB pin index: 21 */
#define BOARD_INITPINS_LED_BLUE_PORT PORTB                 /*!<@brief PORT device name: PORTB */
#define BOARD_INITPINS_LED_BLUE_PIN 21U                    /*!<@brief PORTB pin index: 21 */
#define BOARD_INITPINS_LED_BLUE_CHANNEL 21                 /*!<@brief GPIOB GPIO channel: 21 */
#define BOARD_INITPINS_LED_BLUE_PIN_NAME PTB21             /*!<@brief Pin name */
#define BOARD_INITPINS_LED_BLUE_LABEL "D12[3]/LEDRGB_BLUE" /*!<@brief Label */
#define BOARD_INITPINS_LED_BLUE_NAME "LED_BLUE"            /*!<@brief Identifier name */
                                                           /* @} */

/*! @name PORTB22 (number 68), D12[1]/LEDRGB_RED
  @{ */
#define BOARD_INITPINS_LED_RED_PERIPHERAL GPIOB          /*!<@brief Device name: GPIOB */
#define BOARD_INITPINS_LED_RED_SIGNAL GPIO               /*!<@brief GPIOB signal: GPIO */
#define BOARD_INITPINS_LED_RED_GPIO GPIOB                /*!<@brief GPIO device name: GPIOB */
#define BOARD_INITPINS_LED_RED_GPIO_PIN 22U              /*!<@brief PORTB pin index: 22 */
#define BOARD_INITPINS_LED_RED_PORT PORTB                /*!<@brief PORT device name: PORTB */
#define BOARD_INITPINS_LED_RED_PIN 22U                   /*!<@brief PORTB pin index: 22 */
#define BOARD_INITPINS_LED_RED_CHANNEL 22                /*!<@brief GPIOB GPIO channel: 22 */
#define BOARD_INITPINS_LED_RED_PIN_NAME PTB22            /*!<@brief Pin name */
#define BOARD_INITPINS_LED_RED_LABEL "D12[1]/LEDRGB_RED" /*!<@brief Label */
#define BOARD_INITPINS_LED_RED_NAME "LED_RED"            /*!<@brief Identifier name */
                                                         /* @} */

/*! @name PORTB2 (number 55), J4[2]
  @{ */
#define BOARD_INITPINS_ADC0_SE12_PERIPHERAL ADC0    /*!<@brief Device name: ADC0 */
#define BOARD_INITPINS_ADC0_SE12_SIGNAL SE          /*!<@brief ADC0 signal: SE */
#define BOARD_INITPINS_ADC0_SE12_PORT PORTB         /*!<@brief PORT device name: PORTB */
#define BOARD_INITPINS_ADC0_SE12_PIN 2U             /*!<@brief PORTB pin index: 2 */
#define BOARD_INITPINS_ADC0_SE12_CHANNEL 12         /*!<@brief ADC0 SE channel: 12 */
#define BOARD_INITPINS_ADC0_SE12_PIN_NAME ADC0_SE12 /*!<@brief Pin name */
#define BOARD_INITPINS_ADC0_SE12_LABEL "J4[2]"      /*!<@brief Label */
#define BOARD_INITPINS_ADC0_SE12_NAME "ADC0_SE12"   /*!<@brief Identifier name */
                                                    /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/