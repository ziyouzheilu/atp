#ifndef CRC16_H
#define CRC16_H
unsigned short crc_ccitt(unsigned char *ptr,int len);

unsigned short CRC16_CCITT(unsigned char *puchMsg, unsigned int usDataLen);
unsigned short CRC16_CCITT_FALSE(unsigned char *puchMsg, unsigned int usDataLen);
unsigned short CRC16_XMODEM(unsigned char *puchMsg, unsigned int usDataLen);
unsigned short CRC16_X25(unsigned char *puchMsg, unsigned int usDataLen);
unsigned short CRC16_MODBUS(unsigned char *puchMsg, unsigned int usDataLen);
unsigned short CRC16_IBM(unsigned char *puchMsg, unsigned int usDataLen);
unsigned short CRC16_MAXIM(unsigned char *puchMsg, unsigned int usDataLen);
unsigned short CRC16_USB(unsigned char *puchMsg, unsigned int usDataLen);

void InvertUint8(unsigned char *dBuf,unsigned char *srcBuf);
void InvertUint16(unsigned short *dBuf,unsigned short *srcBuf);
void InvertUint32(unsigned int *dBuf,unsigned int *srcBuf);
#endif // CRC16_H
