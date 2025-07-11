/**
 * @file algo_crc.hpp
 * @author Fish_Joe (2328339747@qq.com)
 * @brief 定义CRC校验算法类
 * @version 1.0
 * @date 2024-11-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef ALGO_CRC_HPP
#define ALGO_CRC_HPP

#include "Configuration.hpp"

namespace my_engineer
{

/**
 * @brief CRC Validator Class
 * @details
 * @note This Class Only Provide Static Functions
 */
class CCrcValidator {
public:

  /**
   * @brief Deleted Constructor
   */
  CCrcValidator() = delete;

  /**
   * @brief Verify CRC8 Checksum Value for Data Buffer
   * @param buffer[in] Data Buffer Vector
   * @param crc[in] CRC8 Checksum Value
   *
   * @returns RP_OK - Verify Succeeded \n
   *          RP_ERROR - Verify Failed
   */
  static EAppStatus Crc8Verify(const DataBuffer<uint8_t> &buffer, uint8_t crc);

  /**
   * @brief Verify CRC8 Value Checksum for Data Buffer
   * @param buffer[in] Data Buffer Pointer
   * @param crc[in] CRC8 Checksum Value
   * @param len[in] Bit Length of Data Buffer
   *
   * @returns RP_OK - Verify Succeeded \n
   *          RP_ERROR - Verify Failed
   */
  static EAppStatus Crc8Verify(const uint8_t *buffer, uint8_t crc, size_t len);

  /**
   * @brief Verify CRC16 Checksum Value for Data Buffer
   * @param buffer[in] Data Buffer
   * @param crc[in] CRC16 Checksum Value
   *
   * @returns RP_OK - Verify Succeeded \n
   *          RP_ERROR - Verify Failed
   */
  static EAppStatus Crc16Verify(const DataBuffer<uint8_t> &buffer, uint16_t crc);

  /**
   * @brief Verify CRC16 Checksum Value for Data Buffer
   * @param buffer[in] Data Buffer Pointer
   * @param crc[in] CRC8 Checksum Value
   * @param len[in] Bit Length of Data Buffer
   *
   * @returns RP_OK - Verify Succeeded \n
   *          RP_ERROR - Verify Failed
   */
  static EAppStatus Crc16Verify(const uint8_t *buffer, uint16_t crc, size_t len);

  /**
   * @brief Calculate CRC8 Checksum Value from Data Buffer
   * @param buffer[in] Data Buffer
   *
   * @return CRC8 Checksum Value
   */
  static uint8_t Crc8Calculate(const DataBuffer<uint8_t> &buffer);

  /**
   * @brief Calculate CRC8 Checksum Value from Data Buffer
   * @param buffer[in] Data Buffer Pointer
   * @param len[in] Bit Length of Data Buffer
   *
   * @return CRC8 Checksum Value
   */
  static uint8_t Crc8Calculate(const uint8_t *buffer, size_t len);

  /**
   * @brief Calculate CRC16 Checksum Value from Data Buffer
   * @param buffer[in] Data Buffer
   *
   * @return CRC16 Checksum Value
   */
  static uint16_t Crc16Calculate(const DataBuffer<uint8_t> &buffer);

  /**
   * @brief Calculate CRC16 Checksum Value from Data Buffer
   * @param buffer[in] Data Buffer Pointer
   * @param len[in] Bit Length of Data Buffer
   *
   * @return CRC16 Checksum Value
   */
  static uint16_t Crc16Calculate(const uint8_t *buffer, size_t len);

private:
};

} // namespace my_engineer

#endif // ALGO_CRC_HPP
