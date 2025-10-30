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


// ClientD.h - Represents a client entity with cryptographic identity
// Handles client name, UUID, RSA keys, and versioning
class ClientD {
    std::string name; // Client's name
    UUID16 uuid;      // Unique identifier
    RSAPrivateWrapper *rsaPrivate; // RSA private key wrapper
    uint8_t version = 1; // Client version

public:
    explicit ClientD(std::string name);// Construct client with name
    ClientD(); // Default constructor
    ~ClientD() = default;
    std::string getName() const; // Get client name
    UUID16 getUUID() const; // Get client UUID
    std::string  getRSAPublic() const; // Get RSA public key (Base64)
    std::string getPrivateKeyBase64() const; // Get private key (Base64)
    uint8_t getVersion() const; // Get protocol version

    // Setters for client properties
    void setClientName(const std::string& newName);
    void setUUID(const UUID16& newUUID);
    void setRSAPrivateKeyFromBase64(const std::string& base64Key);
};
void updateMap(std::map<UUID16, ClientSvd>& svdClients, const std::vector<Client>& baseClients);
bool canSendToClient(UUID16 uuid, const std::map<UUID16, ClientSvd> & clients);
std::string findNameFromUUID(UUID16 uuid, const std::map<UUID16, ClientSvd> & clients);
UUID16 findUUIDFromName(const std::string & name, const std::map<UUID16, ClientSvd> & clients);
bool updateClientPubKey( UUID16 uuid, const RSAPublicWrapper * wrapper, std::map<UUID16, ClientSvd> & clients);
bool updateClientSymKey( UUID16 uuid, const std::string & symKey, std::map<UUID16, ClientSvd> & clients);
RSAPublicWrapper * getClientPubKey( UUID16 uuid, const std::map<UUID16, ClientSvd> & clients);
AESWrapper * getClientSymKey( UUID16 uuid, const std::map<UUID16, ClientSvd> & clients);
#endif //CLIENTD_H
