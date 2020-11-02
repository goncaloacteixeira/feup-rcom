/**
 * \addtogroup APPLICATIONLAYER
 * @{
 */

/** 
 * @brief Information Frame Struct
 * This struct stores the bytes from an information frame
 */
typedef struct {
  unsigned char address;    /**< @brief The address byte */
  unsigned char control;    /**< @brief The control byte */
  unsigned char bcc1;       /**< @brief The BCC1 byte */
  unsigned char *data;      /**< @brief The data array */
  int data_size;            /**< @brief The data array size in bytes */
  unsigned char bcc2;       /**< @brief The BCC2 byte */

  unsigned char *raw_bytes; /**< @brief The array containing unprocessed bytes */
} information_frame_t;

/**
 * @brief Data Packet Struct
 * This struct stores the bytes from a data packet
 */
typedef struct {
  unsigned char control;    /**< @brief The control byte - [DATA] */
  unsigned char sequence;   /**< @brief The sequence byte - index on global data */
  int data_field_size;      /**< @brief The size of the data array - [1..PACKET_SIZE] */
  unsigned char data[2*PACKET_SIZE]; /**< @brief The data array */

  unsigned char *raw_bytes; /**< @brief The array containing unprocessed bytes */
  int raw_bytes_size;       /**< @brief The size of the raw_bytes array */
} data_packet_t;

/**
 * @brief Control Packet Struct
 * This struct stores the bytes from a control packet
 */
typedef struct {
  unsigned char control;      /**< @brief The control byte - [START; STOP] */
  unsigned char *file_size;   /**< @brief File size in bytes stored in an array */
  unsigned char *file_name;   /**< @brief String for the filename */
  unsigned int filesize_size; /**< @brief Size of the file_size array */

  unsigned char *raw_bytes;   /**< @brief The array containing unprocessed bytes */ 
  int raw_bytes_size;         /**< @brief The size of the raw_bytes array */
} control_packet_t;

/**
 * @brief File information Struct
 * This struct stores the file's information
 */
typedef struct {
  unsigned char* data;  /**< @brief Data array, containing all the bytes from the file */
  unsigned char* name;  /**< @brief Name of the file */
  unsigned long size;   /**< @brief Size of the file in bytes */
} file_t;

/** @} */