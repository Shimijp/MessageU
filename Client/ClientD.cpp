//
// Created by shimo on 09/09/2025.
//

#include "ClientD.h"
ClientD::ClientD(std::string name) {
    this->name = std::move(name);
    this->uuid = UUID16{};
    this->rsaPrivate = RSAPrivateWrapper();
}
std::string ClientD::getName()  const{
    return name;
}
UUID16 ClientD::getUUID() const {
    return uuid;
}
std::string ClientD::getRSAPublic() const {
    return rsaPrivate.getPublicKey();
}
std::string ClientD::getPrivateKeyBase64() const {
    return Base64Wrapper::encode(rsaPrivate.getPrivateKey());
}
uint8_t ClientD::getVersion() const {
    return version;
}
void ClientD::setUUID(const UUID16& newUUID) {
    this->uuid = newUUID;
}
void ClientD::setClientName(const std::string& newName) {
    this->name = newName;
}
void ClientD::setRSAPrivateKeyFromBase64(const std::string& base64Key) {
    this->rsaPrivate = RSAPrivateWrapper(Base64Wrapper::decode(base64Key));
}


