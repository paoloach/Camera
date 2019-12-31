#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#ifdef __cplusplus
extern "C" {
#endif

void settingsStartup(void);
const char *clientID(void);
const char *clientSecret(void);
const char *deviceCode(void);
const char * getAccessToken();
const char * getRefreshToken();
uint16_t clientIDLen(void);
uint16_t clientSecretLen(void);
uint16_t deviceCodeLen(void);
uint16_t getAccessTokenLen(void);
uint32_t getAccessTokenExpire();
time_t getAccessTokenExpireTime();
uint16_t getRefreshTokenLen();
uint32_t getPhotoPeriod();

void setClientId(const char *clientID, uint16_t len);
void setSecretId(const char *secretId, uint16_t len);
void setDeviceCode(const char *deviceCode, uint16_t len);
void setAccessToken(const char *accessToken);
void setAccessTokenExpire(uint32_t expire);
void setRefreshToken(const char *accessToken);
void setPhotoPeriod(uint32_t newPeriod);


extern const char *HOSTNAME;

#ifdef __cplusplus
}
#endif

#endif
