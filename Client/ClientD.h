//
// Created by shimo on 09/09/2025.
//

#ifndef CLIENTD_H
#define CLIENTD_H


#include <array>
#include <string>
#include <cstdint>
#include "RSAWrapper.h"
#include "Base64Wrapper.h"
#include "utils.h"


class ClientD {
    std::string name;
    UUID16 uuid;
    RSAPrivateWrapper *rsaPrivate;
    uint8_t version = 1;

public:
    explicit ClientD(std::string name);
    ~ClientD() = default;
    std::string getName() const;
    UUID16 getUUID() const;
    std::string  getRSAPublic() const;
    std::string getPrivateKeyBase64() const;
    uint8_t getVersion() const;



    void setClientName(const std::string& newName);
    void setUUID(const UUID16& newUUID);
    void setRSAPrivateKeyFromBase64(const std::string& base64Key);






};



#endif //CLIENTD_H
