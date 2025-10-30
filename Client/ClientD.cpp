//
// Created by shimo on 09/09/2025.
//

#include "ClientD.h"

#include <stdexcept>

ClientD::ClientD() {
    name = "";
    uuid = UUID16{};
    rsaPrivate = nullptr;
}
ClientD::ClientD(std::string name) {
    this->name = std::move(name);
    this->uuid = UUID16{};
    this->rsaPrivate = new RSAPrivateWrapper();
}
std::string ClientD::getName()  const{
    return name;
}
UUID16 ClientD::getUUID() const {
    return uuid;
}
std::string ClientD::getRSAPublic() const {
    return rsaPrivate->getPublicKey();
}
std::string ClientD::getPrivateKeyBase64() const {
    return Base64Wrapper::encode(rsaPrivate->getPrivateKey());
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
    this->rsaPrivate =new  RSAPrivateWrapper(Base64Wrapper::decode(base64Key));
}
void updateMap(std::map<UUID16, ClientSvd>& svdClients, const std::vector<Client>& baseClients) {
    for (const auto& client : baseClients) {
        if (svdClients.find(client.uuid16) == svdClients.end()) {
            svdClients.emplace(client.uuid16, ClientSvd(client));
        }
    }
}
bool canSendToClient(const UUID16 uuid, const std::map<UUID16, ClientSvd> & clients) {
    const auto clientIt = clients.find(uuid);
    if(clientIt != clients.end()) {
        if(clientIt->second.hasSymKey()) {
            return true;
        }
    }
    return false;
}
std::string findNameFromUUID(UUID16 uuid, const std::map<UUID16, ClientSvd> & clients) {
    const auto clientIt = clients.find(uuid);
    if(clientIt != clients.end()) {
        return clientIt->second.name;
    }
    return "";
}
UUID16 findUUIDFromName(const std::string & name, const std::map<UUID16, ClientSvd> & clients) {
    for (const auto& [uuid, client] : clients) {
        if(client.name == name) {
            return uuid;
        }

    }
    throw std::runtime_error("Client with name " + name + " not found in client list.");
}
bool updateClientPubKey(const UUID16 uuid, const RSAPublicWrapper * wrapper, std::map<UUID16, ClientSvd> & clients) {
    const auto it  = clients.find(uuid);
    if(it != clients.end()) {
        it->second.base64PubKey = Base64Wrapper::encode( wrapper->getPublicKey());
        return true;
    }
    return false;
}
bool updateClientSymKey(const UUID16 uuid, const std::string & symKey, std::map<UUID16, ClientSvd> & clients) {
    const auto it  = clients.find(uuid);
    if( it != clients.end()) {
        it->second.symKey = symKey;
        return true;
    }
    return false;
}
RSAPublicWrapper * getClientPubKey( UUID16 uuid, const std::map<UUID16, ClientSvd> & clients) {
    const auto it  = clients.find(uuid);
    if( it != clients.end() && it->second.hasPubKey()) {
        return it->second.base64PubKey.empty() ? nullptr :
               new RSAPublicWrapper(Base64Wrapper::decode(it->second.base64PubKey));
    }
    return nullptr;
}
AESWrapper *getClientSymKey(UUID16 uuid, const std::map<UUID16, ClientSvd> &clients) {
    const auto it = clients.find(uuid);
    if (it != clients.end() && it->second.hasSymKey()) {
        return new AESWrapper(reinterpret_cast<const unsigned char*>(it->second.symKey.data()), AESWrapper::DEFAULT_KEYLENGTH);
    }
    return nullptr;
}
